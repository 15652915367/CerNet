/***************************************************************************
 *                              CarGeo6 Project                            *
 *		C2CNet-IPv6 adaptation layer implementation		   *
 *			  conforming with GeoNet D2.2         		   *
 *                                                                         *
 *                      Copyright(C)2010 ESPRIT-INRIA                      *
 *                                                                         *
 *                                                                         *
 *   Authors:                                                              *
 *   Hichem BARGAOUI <barghich@gmail.com>                                  *
 *   Anouar CHEMEK <anouar.chemek@gmail.com>                               *
 *   Manabu TSUKADA <Manabu.Tsukada@inria.fr>                              *
 *   Thouraya TOUKABRI <thouraya.toukabri@inria.fr>                        *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
* @file itsnet_ip.c
* itsnet ip code.
* @author Hichem BARGAOUI
* @author Anouar CHEMEK
* @author Manabu TSUKADA
* @author Thouraya TOUKABRI
*/


#include <stdint.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <unistd.h>
#include <errno.h>
#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#else
#error "POSIX Thread Library required!"
#endif

#include <asm/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/if_ether.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <syslog.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/in_route.h>
#include "./include/linux/ip_mp_alg.h"
#include "./include/rt_names.h"
#include "./include/utils.h"

#include "position_sensor.h"
#include "itsnet_types.h"
#include "itsnet_header.h"
#include "itsnet_parser.h"
#include "itsnet_ip.h"
#include "geo_broadcast.h"
#include "geo_unicast.h"
#include "geo_anycast.h"
#include "itsnet.h"



#define max(a,b) ((a)>(b) ? (a):(b))
#define BUFF_SIZE 1500
#define TIME_SIZE 40


static pthread_mutex_t mutex;
static pthread_t itsnet_iplistener;

int preferred_family = AF_UNSPEC;
int resolve_hosts = 0;
int f1;
char dev1[7];
uint8_t dst_id[8];


struct rtnl_handle rth = { .fd = -1 };

static const char *mx_names[RTAX_MAX+1] = {

        [RTAX_MTU]      = "mtu",
        [RTAX_WINDOW]   = "window",
        [RTAX_RTT]      = "rtt",
        [RTAX_RTTVAR]   = "rttvar",
        [RTAX_SSTHRESH] = "ssthresh",
        [RTAX_CWND]     = "cwnd",
        [RTAX_ADVMSS]   = "advmss",
        [RTAX_REORDERING]="reordering",
        [RTAX_HOPLIMIT] = "hoplimit",
        [RTAX_INITCWND] = "initcwnd",
        [RTAX_FEATURES] = "features",
        [RTAX_RTO_MIN]  = "rto_min",
};

/**
* defines ip filter data structure
*/

static struct {

        int tb;
        int cloned;
        int flushed;
        char *flushb;
        int flushp;
        int flushe;
        int protocol, protocolmask;
        int scope, scopemask;
        int type, typemask;
        int tos, tosmask;
        int iif, iifmask;
        int oif, oifmask;
        int realm, realmmask;   
        inet_prefix rprefsrc;
        inet_prefix rvia;
        inet_prefix rdst;
        inet_prefix mdst;
        inet_prefix rsrc;
        inet_prefix msrc;
} filter;


/**
* convert_char_to_hexa
* @param char c
* @return uint8_t
*/ 


uint8_t convert(char c)
{
	switch(c)
	{

        	case '0':       return 0x00;
               	         	break;
        	case '1':       return 0x01;
                        	break;
        	case '2':       return 0x02;
                        	break;
        	case '3':       return 0x03;
                        	break;
        	case '4':       return 0x04;
                        	break;
        	case '5':       return 0x05;
                        	break;
        	case '6':       return 0x06;
                        	break;
        	case '7':       return 0x07;
                        	break;
        	case '8':       return 0x08;
                        	break;
        	case '9':       return 0x09;
                        	break;
		case 'a':       return 0x0a;
                        	break;
        	case 'A':       return 0x0a;
                        	break;
        	case 'b':       return 0x0b;
                        	break;
        	case 'B':       return 0x0b;
                        	break;
        	case 'c':       return 0x0c;
                        	break;
        	case 'C':       return 0x0c;
                        	break;
        	case 'd':       return 0x0d;
                        	break;
        	case 'D':       return 0x0d;
                        	break;
        	case 'e':       return 0x0e;
                        	break;
        	case 'E':       return 0x0e;
                        	break;
        	case 'f':       return 0x0f;
                        	break;
        	case 'F':       return 0x0f;
        	                break;
        	default:        return 1;

	}
}

struct GeoMulticastParam{
  int lat;
  int lng;
  int radius;
};

/**
* iproute_reset_filter
* @brief initializes the ip filter
* @param void
* @return void
*/

void iproute_reset_filter()
{	
	memset(&filter, 0, sizeof(filter));
	filter.mdst.bitlen = -1;
	filter.msrc.bitlen = -1;
}

/**
* inline int rtm_get_table
* @brief gets local ip routing table
* @param const struct rtmsg *r
* @param struct rtattr **tb
* @return static inline
*/

static inline int rtm_get_table(struct rtmsg *r, struct rtattr **tb) 
{
	
	__u32 table = r->rtm_table;
	if (tb[RTA_TABLE])
	table = *(__u32*) RTA_DATA(tb[RTA_TABLE]);
	return table;
}


/**
* find_nemo_ip_nexthop
* @brief This function determine the IP nexthop of the default route selected if NEMO is used
* it also uses the get_c2cid_from_ip_nexthop() function and return the correspondent C2C-ID
* @param const struct sockaddr_nl *who
* @param struct nlmsghdr *n
* @param void *arg
* @return int
*/


int find_nemo_ip_nexthop(const struct sockaddr_nl *who, struct nlmsghdr *n, void *arg)
{
	struct rtmsg *r = NLMSG_DATA(n);
	int len = n->nlmsg_len;
	struct rtattr * tb[RTA_MAX+1];
	char abuf[256];
	inet_prefix dst;
	inet_prefix src;
	inet_prefix prefsrc;
	inet_prefix via;
	int host_len = -1;
	static int ip6_multiple_tables;
	__u32 table;
	SPRINT_BUF(b1);
	static int hz;
        
                
	if (n->nlmsg_type != RTM_NEWROUTE && n->nlmsg_type != RTM_DELROUTE) 
	{
		fprintf(stderr, "Not a route: %08x %08x %08x\n",
				n->nlmsg_len, n->nlmsg_type, n->nlmsg_flags);
        	return 0;
	}

	if (filter.flushb && n->nlmsg_type != RTM_NEWROUTE)
        	return 0;

	len -= NLMSG_LENGTH(sizeof(*r));
	
	if (len < 0) 
	{
        	fprintf(stderr, "BUG: wrong nlmsg len %d\n", len);
        	return -1;
	}
        
	if (r->rtm_family == AF_INET6)
        	host_len = 128;
	else if (r->rtm_family == AF_INET)
        	host_len = 32;
	else if (r->rtm_family == AF_DECnet)
        	host_len = 16;
	else if (r->rtm_family == AF_IPX)
        	host_len = 80;
                        
	parse_rtattr(tb, RTA_MAX, RTM_RTA(r), len);
	table = rtm_get_table(r, tb);

	if (r->rtm_family == AF_INET6 && table != RT_TABLE_MAIN)
        	ip6_multiple_tables = 1;

	if (r->rtm_family == AF_INET6 && !ip6_multiple_tables) 
	{
        	if (filter.cloned) 
		{
                	if (!(r->rtm_flags&RTM_F_CLONED))
                        	return 0;
        	}
        
		if (filter.tb) 
		{
                	if (!filter.cloned && r->rtm_flags&RTM_F_CLONED)
                        	return 0;
                	if (filter.tb == RT_TABLE_LOCAL)
			{
                        	if (r->rtm_type != RTN_LOCAL)
                                	return 0;
                	} 
			else if (filter.tb == RT_TABLE_MAIN) 
			{
                        	if (r->rtm_type == RTN_LOCAL)
                                	return 0;
                	} 
			else 
			{
                        	return 0;
                	}
        	}
	} 
	else 
	{
        	if (filter.cloned) 
		{
			if (!(r->rtm_flags&RTM_F_CLONED))
                        	return 0;
        	}
        	
		if (filter.tb > 0 && filter.tb != table)
                	return 0;
	}

	if ((filter.protocol^r->rtm_protocol)&filter.protocolmask)
        	return 0;
	if ((filter.scope^r->rtm_scope)&filter.scopemask)
        	return 0;
	if ((filter.type^r->rtm_type)&filter.typemask)
        	return 0;
	if ((filter.tos^r->rtm_tos)&filter.tosmask)
        	return 0;
	if (filter.rdst.family &&
		(r->rtm_family != filter.rdst.family || filter.rdst.bitlen > r->rtm_dst_len))
        	return 0;
	if (filter.mdst.family &&
		(r->rtm_family != filter.mdst.family ||
			(filter.mdst.bitlen >= 0 && filter.mdst.bitlen < r->rtm_dst_len)))
        	return 0;
	if (filter.rsrc.family &&
		(r->rtm_family != filter.rsrc.family || filter.rsrc.bitlen > r->rtm_src_len))
        	return 0;
	if (filter.msrc.family &&
		(r->rtm_family != filter.msrc.family ||
     			(filter.msrc.bitlen >= 0 && filter.msrc.bitlen < r->rtm_src_len)))
        	return 0;
	if (filter.rvia.family && r->rtm_family != filter.rvia.family)
        	return 0;
	if (filter.rprefsrc.family && r->rtm_family != filter.rprefsrc.family)
        	return 0;

	memset(&dst, 0, sizeof(dst));
	dst.family = r->rtm_family;

	if (tb[RTA_DST])
        	memcpy(&dst.data, RTA_DATA(tb[RTA_DST]), (r->rtm_dst_len+7)/8);
	
	if (filter.rsrc.family || filter.msrc.family) 
	{
        	memset(&src, 0, sizeof(src));
        	src.family = r->rtm_family;
        	if (tb[RTA_SRC])
                	memcpy(&src.data, RTA_DATA(tb[RTA_SRC]), (r->rtm_src_len+7)/8);
	}

	if (filter.rvia.bitlen>0) 
	{
        	memset(&via, 0, sizeof(via));
        	via.family = r->rtm_family;
        	
		if (tb[RTA_GATEWAY])
                	memcpy(&via.data, RTA_DATA(tb[RTA_GATEWAY]), host_len/8);
	}

	if (filter.rprefsrc.bitlen>0) 
	{
        	memset(&prefsrc, 0, sizeof(prefsrc));
        	prefsrc.family = r->rtm_family;
        
	if (tb[RTA_PREFSRC])
		memcpy(&prefsrc.data, RTA_DATA(tb[RTA_PREFSRC]), host_len/8);
	}

	if (filter.rdst.family && inet_addr_match(&dst, &filter.rdst, filter.rdst.bitlen))
        	return 0;

	if (filter.mdst.family && filter.mdst.bitlen >= 0 && 
		inet_addr_match(&dst, &filter.mdst, r->rtm_dst_len))
        	return 0;

	if (filter.rsrc.family && inet_addr_match(&src, &filter.rsrc, filter.rsrc.bitlen))
        	return 0;

	if (filter.msrc.family && filter.msrc.bitlen >= 0 && 
		inet_addr_match(&src, &filter.msrc, r->rtm_src_len))
        	return 0;

	if (filter.rvia.family && inet_addr_match(&via, &filter.rvia, filter.rvia.bitlen))
        	return 0;

	if (filter.rprefsrc.family && inet_addr_match(&prefsrc, &filter.rprefsrc, filter.rprefsrc.bitlen))
        	return 0;

	if (filter.realmmask) 
	{
        	__u32 realms = 0;
        
		if (tb[RTA_FLOW])
                	realms = *(__u32*)RTA_DATA(tb[RTA_FLOW]);
        	if ((realms^filter.realm)&filter.realmmask)
                	return 0;
	}

	if (filter.iifmask) 
	{
        	int iif = 0;
        
		if (tb[RTA_IIF])
                	iif = *(int*)RTA_DATA(tb[RTA_IIF]);
        
		if ((iif^filter.iif)&filter.iifmask)
                	return 0;
	}

	if (filter.oifmask) 
	{
        	int oif = 0;
        	if (tb[RTA_OIF])
                oif = *(int*)RTA_DATA(tb[RTA_OIF]);
        
		if ((oif^filter.oif)&filter.oifmask)
                	return 0;
	}

	if (filter.flushb && 
		r->rtm_family == AF_INET6 && 
			r->rtm_dst_len == 0 &&
    				r->rtm_type == RTN_UNREACHABLE &&
    					tb[RTA_PRIORITY] &&
    						*(int*)RTA_DATA(tb[RTA_PRIORITY]) == -1)
        	return 0;

	if (tb[RTA_GATEWAY] && filter.rvia.bitlen != host_len) 
	{
        	format_host(r->rtm_family, RTA_PAYLOAD(tb[RTA_GATEWAY]),RTA_DATA(tb[RTA_GATEWAY]), abuf, sizeof(abuf));
        	get_c2cid_from_ip_nexthop(abuf);
	}

	return 0;

}

/**
* find_ip_nexthop
* @brief This function searches for IP nexthop.If the ip6nl interface is NEMO, IP nexthop is the default gateway.
* In this case, function find_default_route() is called.
* @param const struct sockaddr_nl *who
* @param struct nlmsghdr *n
* @param void *arg
* @return int
*/ 

int find_ip_nexthop(const struct sockaddr_nl *who, struct nlmsghdr *n, void *arg)
{

	struct rtmsg *r = NLMSG_DATA(n);
	int len = n->nlmsg_len;
	__u32 table;
	struct rtattr * tb[RTA_MAX+1];
	char abuf[256];
	int host_len = -1;
	char *dev=NULL;
#if 0
	char *c2c_id[4];
	char *tmpbuf=NULL;
	int i=0,s=0;
	uint8_t dst[16];
#endif

	if (n->nlmsg_type != RTM_NEWROUTE && n->nlmsg_type != RTM_DELROUTE)
	{
		fprintf(stderr, "Not a route: %08x %08x %08x\n",
			n->nlmsg_len, n->nlmsg_type, n->nlmsg_flags);
		return 0;
	}

	if (filter.flushb && n->nlmsg_type != RTM_NEWROUTE)
		return 0;

	len -= NLMSG_LENGTH(sizeof(*r));
	if (len < 0)
	{
		fprintf(stderr, "BUG: wrong nlmsg len %d\n", len);
		return -1;
	}

	if (r->rtm_family == AF_INET6)
	{
		host_len = 128;
	}
	else if (r->rtm_family == AF_INET)
	{
		fprintf(stderr, "IPv4!\n");
		return 0;
	}
	else if (r->rtm_family == AF_DECnet)
	{
		fprintf(stderr,"DECnet!\n");
		return 0;
	}
	else if (r->rtm_family == AF_IPX)
	{
		fprintf(stderr,"IPX!\n");
		return 0;
	}


	parse_rtattr(tb, RTA_MAX, RTM_RTA(r), len);
	table = rtm_get_table(r, tb);

	if (tb[RTA_OIF] && filter.oifmask != -1)
	{
		dev = ll_index_to_name(*(int*)RTA_DATA(tb[RTA_OIF]));
		if (strncmp(dev, "ip6tnl", 6) == 0)
		{
			fprintf(LOG, "\t(dev = %s) NEMO interface! -> show default route\n", dev);

			fprintf(LOG, "\t(Default route): \n");

			find_default_route("default");

		}
		else
		{
			fprintf(LOG, "\t(dev = %s) not NEMO interface!! -> show nexthop\n", dev);
			fprintf(LOG, "\t(IP next hop):");
			if (tb[RTA_GATEWAY] && filter.rvia.bitlen != host_len)
			{
				format_host(r->rtm_family, RTA_PAYLOAD(tb[RTA_GATEWAY]),
				RTA_DATA(tb[RTA_GATEWAY]), abuf, sizeof(abuf));
			}
			get_c2cid_from_ip_nexthop(abuf);
		}
	}
	
	if (dbg_level > 0)  fflush(LOG);
	return 0;

}


/**
* get_c2cid_from_ip_nexthop
* @param char *abuf
* @return int
*/ 

int get_c2cid_from_ip_nexthop(char *abuf)
{

	char *c2c_id4[4];  //0000:0000:0000:CA02
	char *c2c_id2[8];  //00:00:00:00:00:00:CA:02
	char *tmpbuf=NULL;
	int i=0,s=0;
	

	// IP next hop, or default route when NEMO is used
	fprintf(LOG, "\t%s", abuf);

	if(dbg_level > 4) 
		fprintf(LOG, "------- finding C2C NET ID from IPv6 address --------\n");

	for(i=3; i>=0 ; i--)
	{
		tmpbuf = strrchr (abuf, ':');

		if (tmpbuf != NULL)
		{
			if (strcmp(tmpbuf, strrchr (abuf, '::')) == 0 )
			{
				/* '::' in the IPv6 address*/
				char ID[4];
				c2c_id4[i] = tmpbuf+1;

				// first two charactors
				if (strlen(c2c_id4[i]) <= 2)
				{
					c2c_id2[i*2] = "00";
				}
				else if (strlen(c2c_id4[i]) == 3)
				{
					sprintf(ID, "%.1s\0", c2c_id4[i]);
					c2c_id2[i*2] = ID;
				}
				else if (strlen(c2c_id4[i]) == 4)
				{
					sprintf(ID, "%.2s\0", c2c_id4[i]);
					c2c_id2[i*2] = ID;
				}

				// second two charactors
				c2c_id2[i*2+1] = c2c_id4[i]+ (strlen(c2c_id4[i])-2);
				i--;

				while( i>=0)
				{
					c2c_id4[i]="0000";
					c2c_id2[i*2]="00";
					c2c_id2[i*2+1]="00";
					i--;
				}
				break;	
			}

			c2c_id4[i] = tmpbuf+1;
			*tmpbuf='\0';
		}
	}


	if (dbg_level > 4)
	{
		fprintf(LOG, "\n(C2CNet DST)(4) %s %s %s %s\n", c2c_id4[0],c2c_id4[1],c2c_id4[2],c2c_id4[3]);
	}

	fprintf(LOG, "   (C2CNet DST)(2) %s %s %s %s %s %s %s %s\n", 
			c2c_id2[0],c2c_id2[1],c2c_id2[2],c2c_id2[3],
				c2c_id2[4],c2c_id2[5],c2c_id2[6],c2c_id2[7]); 

	for(i=0;i<8;i++)
	{
		dst_id[i] = strtol(c2c_id2[i], NULL, 16);
	}

#if 0
	for(i=0;i<4;i++)
	{
		for(s=0;s<strlen(c2c_id[i]+1);s++)
		{
			dst[4*i+s+4-strlen(c2c_id[i]+1)]=convert_char_to_hexa(c2c_id[i][s+1]);
		}
	}

	for(i=0;i<8;i++)
		dst_id[i]=(dst[2*i] << 4)|(dst[2*i+1] & 0x0f);

#endif


	if (dbg_level > 0)  fflush(LOG);
	return 0;

}

/**
* iproute_get
* @param char *destination
* @return int
*/

int iproute_get(char *destination)
{
	struct
	{
		struct nlmsghdr 	n;
		struct rtmsg 		r;
		char 	  	buf[1024];
	} req;

	inet_prefix addr;

	if(dbg_level > 4) fprintf(LOG, "------- lookuping routing table ---------------------\n");

	memset(&req, 0, sizeof(req));

	iproute_reset_filter();

	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
	req.n.nlmsg_flags = NLM_F_REQUEST;
	req.n.nlmsg_type = RTM_GETROUTE;
	req.r.rtm_family = preferred_family;
	req.r.rtm_table = 0;
	req.r.rtm_protocol = 0;
	req.r.rtm_scope = 0;
	req.r.rtm_type = 0;
	req.r.rtm_src_len = 0;
	req.r.rtm_dst_len = 0;
	req.r.rtm_tos = 0;

	if(get_prefix(&addr, destination, req.r.rtm_family) == 1)
		return 1;

	if (req.r.rtm_family == AF_UNSPEC)
	{
		req.r.rtm_family = addr.family;
	}

	if (addr.bytelen)
	{
		addattr_l(&req.n, sizeof(req), RTA_DST, &addr.data, addr.bytelen);
	}

	req.r.rtm_dst_len = addr.bitlen;

	if (req.r.rtm_dst_len == 0)
	{
		fprintf(stderr, "need at least destination address\n");
		return 1;
	}

	ll_init_map(&rth);

	if (rtnl_talk(&rth, &req.n, 0, 0, &req.n, NULL, NULL) < 0)
	{
		fprintf(stderr, "iproute_get: rtnl_talk failed!!\n");
		return 2;
	}

	if (find_ip_nexthop(NULL, &req.n, (void*)stdout) < 0)
	{
		fprintf(LOG, "An error 2 :-)\n");
		return 1;
	}

	if (dbg_level > 0)  fflush(LOG);

}


/**
* tun_alloc
* @param char *dev
* @return int
*/


int tun_alloc(char *dev)
{
	struct ifreq ifr;
	int fd, err;

	if( (fd = open("/dev/net/tun", O_RDWR)) < 0 )
		return -1;

	memset(&ifr, 0, sizeof(ifr));

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	*         IFF_TAP   - TAP device
	*         IFF_NO_PI - Do not provide packet information
	*/
	//	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;  /* manabu */

	if( *dev )
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 )
	{
		close(fd);
		return err;
	}

	strcpy(dev, ifr.ifr_name);
	return fd;

}


/**
* iplistener
* @param void
* @return void
*/

static void *iplistener(void *arg)
{

short buf[BUFF_SIZE];
struct GeoMulticastParam mcast_param;
int l,fm;
fd_set fds;
int i=0, m=0, n=0, j=0,k=0;
uint16_t src[8],dst[8];
char destination[40];
struct itsnet_position_vector pos;



fprintf(LOG, "--------------- waiting to send packet from (");
for(i=0; i<7; i++) fprintf(LOG, "%c", dev1[i]);
fprintf(LOG, ")\n");

	for(i=0; i<7; i++) 
		fprintf(LOG, "%c", dev1[i]);

	fprintf(LOG, ")\n");

	fm = f1 +1;

	if (rtnl_open(&rth, 0) < 0)
		exit(1);

	while(1)
	{
		FD_ZERO(&fds);
		FD_SET(f1, &fds);

		select(fm, &fds, NULL, NULL, NULL);

		if( FD_ISSET(f1, &fds) )
		{
			l = read(f1,buf,sizeof(buf));
			fprintf(LOG, "R ==> %s %i bytes IP packet ", dev1, l);
			m=0;
			for(i=0; i<l ; i++)
			{
				/* TCP/IP network byte order to host byte order 
				   (which is little-endian on Intel processors) */

				// Don't Use this for ITSNET-AMD
				//buf[i] = htons(buf[i]);
				if (i>3 && i<12)
				{
					// Use this for ITSNET-AMD
					src[m] = htons(buf[i]);
					if(dbg_level > 4) fprintf(LOG, " %x :",src[m]);
					m++;
				}
			}

			n=0;
			for(i=0; i<l ; i++)
			{
				/* Parse Destination address */
				if (i>11 && i<20)
				{
					// Use this for ITSNET-AMD
					dst[n] = htons(buf[i]);
					if(dbg_level > 0) fprintf(LOG, " %x :",dst[n]);
					n++;
				}

			}

		}

		if ((dst[0] >> 8)== 0xff )
		{
			/*************************Send Geobroadcast************************/
        	        fprintf(LOG, "(Multicast packet)\n");
                	fprintf(LOG, "\t(SRC) %x:%x:%x:%x:%x:%x:%x:%x --> ",src[0],src[1],src[2],src[3],src[4],src[5],src[6],src[7]);
                	fprintf(LOG, "(DST) %x:%x:%x:%x:%x:%x:%x:%x\n",dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7]);
                	sprintf(&destination, "%x:%x:%x:%x:%x:%x:%x:%x",dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7]);

                	if (strcmp(destination, "ff02:0:0:0:0:0:0:1") == 0)
			{

                        	fprintf(LOG, "\tAll node multicast address (FF02::1)\n");
                        	// Resolving position and Radius
                        	pos=get_position_vector();
                        	itsnet_ip_geobroadcast_tnl(&buf,l, pos.latitude, pos.longitude, geoDst.AN_Mcast_radius);
                        	fprintf(LOG, "\tDestination (Lat = %d, Long = %d, Radius = %d m)\n",pos.latitude, pos.longitude, geoDst.AN_Mcast_radius);

                	}
			else if (strcmp(destination, geoDst.MLD_group) == 0)
			{
                        	fprintf(LOG, "\tMLD proxy group (FF0e::2)\n");
                        	// Resolving position and Radius
                        	pos=get_position_vector();
                        	itsnet_ip_geobroadcast_tnl(&buf,l, pos.latitude, pos.longitude, geoDst.MLD_group_radius);
                        	fprintf(LOG, "\tDestination (Lat = %d, Long = %d, Radius = %d m)\n",pos.latitude, pos.longitude, geoDst.MLD_group_radius);

                	}
			else if (strcmp(destination, "ff35:0:0:0:0:0:0:1") == 0)
			{
													fprintf(LOG, "\tMcast group (FF35::1)\n");
													memcpy(&mcast_param, &buf[24], 12);
													itsnet_ip_geobroadcast_tnl(&buf,l, mcast_param.lat, mcast_param.lng, mcast_param.radius);
                        	fprintf(LOG, "\tDestination (Lat = %d, Long = %d, Radius = %d m)\n", mcast_param.lat, mcast_param.lng, mcast_param.radius);	
			}
			else
			{
                        fprintf(LOG, "destination = %s is NOT equal with ff02::1 nor ff0e::2\n",destination);
                	}


		}
		else
		{

			/*************************Send Geounicast************************/
			itsnet_node_id nodeID;

			fprintf(LOG, "(Unicast packet)\n");

			/* Dispatching packet to C2C layer */
			fprintf(LOG, "\t(SRC) %x:%x:%x:%x:%x:%x:%x:%x --> ",
				src[0],src[1],src[2],src[3],src[4],src[5],src[6],src[7]);
			fprintf(LOG, "(DST) %x:%x:%x:%x:%x:%x:%x:%x\n",
				dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7]);
			sprintf(&destination, "%x:%x:%x:%x:%x:%x:%x:%x",
				dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7]);
			dst_id[6] = dst[7]>>8;
			dst_id[7] = dst[7];

			//iproute_get(destination);

			printf("dst_id:%x-%x-%x-%x-%x-%x-%x-%x\n",
				dst_id[0],
				dst_id[1],
				dst_id[2],
				dst_id[3],
				dst_id[4],
				dst_id[5],
				dst_id[6],
				dst_id[7]);

			//iproute_get(destination);

			for(j=0; j<8; j++) 
			{
				nodeID.id[j]=dst_id[j];
				if (dbg_level > 4) fprintf(LOG, " %2x ",nodeID.id[j]);
			}

			fprintf(LOG, "\n GEoUNicast encapsulation...   \n");
            		//fprintf(LOG, "longueur %d \n",l);

			itsnet_ip_geounicast_tnl(&buf,l,0,0,nodeID);

            		/* print buffer on sending */
            		for(j=0; j<l; j++)
			{
            			fprintf(LOG, " %2x :",buf[j]&0xffff);
			}

		}

#if 0
		itsnet_node_id nodeID;
		fprintf(LOG, "(Unicast packet)\n");

		/* Dispatching packet to C2C layer */
		fprintf(LOG, "\t(SRC) %x:%x:%x:%x:%x:%x:%x:%x --> ",
			src[0],src[1],src[2],src[3],src[4],src[5],src[6],src[7]);
		fprintf(LOG, "(DST) %x:%x:%x:%x:%x:%x:%x:%x\n",
			dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7]);
		sprintf(&destination, "%x:%x:%x:%x:%x:%x:%x:%x",
			dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7]);

		iproute_get(destination);
		
		for(j=0; j<8; j++) 
		{
			nodeID.id[j]=dst_id[j];
			printf("%x:");
			if (dbg_level > 4) fprintf(LOG, " %2x ",nodeID.id[j]);
		}
		printf("\n");
		fflush(stdout);

		if (dbg_level > 4) fprintf(LOG, "\n GEoUNicast encapsulation...   \n");
			//fprintf(LOG, "longueur %d \n",l);
		itsnet_ip_geounicast_tnl(&buf,l,0,0,nodeID);

		if (dbg_level > 4)
		{
			/* print buffer on sending */
			for(j=0; j<l; j++)
			{
				fprintf(LOG, " %2x :",buf[j]&0xffff);
			}
		}
#endif

		if (dbg_level > 0)  fflush(LOG);
	}

	rtnl_close(&rth);

}

/**
* find_default_route
* @brief This functions searches the default route to use in case of NEMO and calls find_nemo_ip_nexthop() function
* @param char *default_route
* @return int
*/


int find_default_route(char *default_route)
{
	//int do_ipv6 = preferred_family;
	int do_ipv6 = AF_INET6;


	iproute_reset_filter();
	filter.tb = RT_TABLE_MAIN;
	get_prefix(&filter.mdst, default_route, do_ipv6);
	filter.rdst = filter.mdst;
	if (do_ipv6 == AF_UNSPEC && filter.tb)
		do_ipv6 = AF_INET;

	ll_init_map(&rth);

	if (!filter.cloned) 
	{
		if (rtnl_wilddump_request(&rth, do_ipv6, RTM_GETROUTE) < 0) 
		{
			perror("Cannot send dump request");
			exit(1);
		}
	}

	if (rtnl_dump_filter(&rth, find_nemo_ip_nexthop, LOG, NULL, NULL) < 0) 
	{
		fprintf(stderr, "Dump terminated\n");
		exit(1);
	}

return 0;

}



/**
* itsnet_iplistener_init
* @brief This function creates a TUN/TAP interface and creates a thread for the iplistener function.
* @param void
* @return int
*/


int itsnet_iplistener_init(void)
{


	strcpy(dev1,"tun0");

	if ((f1 = tun_alloc(dev1)) < 0)
	{
		fprintf(stderr,"Cannot allocate TAP device for sending \n");
		exit(1);
	}

	pthread_mutexattr_t mattrs;
	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs,PTHREAD_MUTEX_TIMED_NP);

	if (pthread_mutex_init(&mutex, &mattrs) || pthread_create(&itsnet_iplistener, NULL,iplistener, NULL))
		return -1;


	return 0;
}


/**
* itsnet_ip_geobroadcast_tnl
* @param short * buf
* @param int l
* @param uint32_t latitude
* @param uint32_t longitude
* @param double geo_area_size
* @return int
*/


int itsnet_ip_geobroadcast_tnl(short * buf, int l, uint32_t latitude, uint32_t longitude,double geo_area_size)
{

	int result,i,j;
	struct itsnet_packet *p;
	struct itsnet_position_vector pos;

	pos=get_position_vector();
	p=(itsnet_packet*)malloc(sizeof(itsnet_packet));

	if (p==NULL)
	{
		fprintf(stderr,"erreur allocation \n");
		result = -1;
	}
	else
	{
		memset(p,0,sizeof(itsnet_packet));
		p->common_header.protocol_info.itsnet_header_type=itsnet_geobroadcast_id;
		p->common_header.traffic_class=CLASS01;
		p->common_header.protocol_info.itsnet_header_subtype=ITSNET_GEOBCAST_CIRCLE;
		p->common_header.flags=OBU_SECURITY_ENABLED;
		p->common_header.payload_lenght=l+ sizeof(struct itsnet_common_header)+sizeof(struct itsnet_geobroadcast_t) - 2*ITSNET_DATA_SIZE;
		p->common_header.hop_limit=255;
		p->common_header.forwarder_position_vector=pos;
		p->common_header.protocol_info.itsnet_next_header=ip;
		p->payload.itsnet_geobroadcast.source_position_vector=pos;
		p->payload.itsnet_geobroadcast.dest_latitude=latitude;
		p->payload.itsnet_geobroadcast.dest_longitude=longitude;
		p->payload.itsnet_geobroadcast.geo_area_size=geo_area_size;

	}

	if (dbg_level > 4)
	{
		fprintf(LOG, "!!!!!!!!!!!!!!!!!!itsnet_ip_geobroadcast_tnl ===> "); 
		print_node_id(p->payload.itsnet_geobroadcast.source_position_vector.node_id);
		fprintf(LOG, "\n"); 
		print_node_id(pos.node_id); fprintf(LOG, "\n"); 
	}

	memcpy(p->payload.itsnet_geobroadcast.payload,buf,l);
	result=itsnet_geobroadcast_send(p);

return result;

}

/**
* itsnet_ip_geounicast_tnl
* @param short * buf
* @param int l
* @param uint32_t latitude
* @param uint32_t longitude
* @param itsnet_node_id node_id
* @return int
*/


int itsnet_ip_geounicast_tnl(short *buf, int l,uint32_t latitude, uint32_t longitude,itsnet_node_id node_id)
{

	int j;
	int result;
	itsnet_packet *p;
	struct itsnet_position_vector pos;

	pos=get_position_vector();
	p=(itsnet_packet*)malloc(sizeof(itsnet_packet));

	if (p==NULL)
	{
		fprintf(stderr,"erreur allocation \n");
		result = -1;
	}
	else
	{
		memset(p,0,sizeof(itsnet_packet));
		p->common_header.protocol_info.itsnet_header_type=itsnet_unicast_id;
		p->common_header.traffic_class=CLASS01;
		p->common_header.protocol_info.itsnet_header_subtype=0;
		p->common_header.flags=OBU_SECURITY_ENABLED;
		p->common_header.payload_lenght=l+ sizeof(struct itsnet_common_header)+sizeof(struct itsnet_unicast_t) - 2*ITSNET_DATA_SIZE;
		p->common_header.hop_limit=255;
		p->common_header.forwarder_position_vector=pos;
		p->common_header.protocol_info.itsnet_next_header=ip;

		p->payload.itsnet_unicast.source_position_vector=pos;
		p->payload.itsnet_unicast.dest_latitude=latitude;
		p->payload.itsnet_unicast.dest_longitude=longitude;
		p->payload.itsnet_unicast.dest_node_id=node_id;
	}


	//memcpy(p->payload.itsnet_unicast.payload,buf,l);

	for(j=0; j<(l/2); j++)
	{
		p->payload.itsnet_unicast.payload[j] = buf[j];
	}

	result=itsnet_geounicast_send(p);

	return result;
}

/**
* itsnet_ip_geoanycast_tnl
* @param short * buf
* @param int l
* @param uint32_t latitude
* @param uint32_t longitude
* @param double geo_area_size
* @return int
*/


int itsnet_ip_geoanycast_tnl(short * buf, int l, uint32_t latitude, uint32_t longitude,double geo_area_size)
{
	int result;
	struct itsnet_packet *p;
	struct itsnet_position_vector pos;

	pos=get_position_vector();
	p=(itsnet_packet*)malloc(sizeof(itsnet_packet));

	if (p==NULL)
	{
		fprintf(stderr,"erreur allocation \n");
		result = -1;
	}
	else
	{

		memset(p,0,sizeof(itsnet_packet));
		p->common_header.protocol_info.itsnet_header_type=itsnet_geoanycast_id;
		p->common_header.traffic_class=CLASS01;
		p->common_header.protocol_info.itsnet_header_subtype=ITSNET_GEOANYCAST_CIRCLE;
		p->common_header.flags=OBU_SECURITY_ENABLED;
		p->common_header.payload_lenght=l+ sizeof(struct itsnet_common_header)+sizeof(struct itsnet_geoanycast_t) - 2*ITSNET_DATA_SIZE;
		p->common_header.hop_limit=255;
		p->common_header.forwarder_position_vector=pos;
		p->common_header.protocol_info.itsnet_next_header=ip;

		p->payload.itsnet_geoanycast.source_position_vector=pos;
		p->payload.itsnet_geoanycast.dest_latitude=latitude;
		p->payload.itsnet_geoanycast.dest_longitude=longitude;
		p->payload.itsnet_geoanycast.geo_area_size=geo_area_size;

	}

	memcpy(p->payload.itsnet_geoanycast.payload,buf,l);
	result=itsnet_geoanycast_send(p);

	return result;

}


/**
* itsnet_ip_receive
* @param struct itsnet_packet *p
* @return int
*/

void itsnet_ip_receive(struct itsnet_packet *p)
{

	int result,fm;
	int i=0,l=0,n=0;
	short buf[BUFF_SIZE];
	char destination[40];

	int x,y;
	uint16_t src[8],dst[8];

	fd_set fds;

	l=p->common_header.payload_lenght;

	if (dbg_level > 4)
	{
		fprintf(LOG, "---------------------------Allocated devices: ");
		for(i=0; i<7; i++) fprintf(LOG, "%c", dev1[i]);
		fprintf(LOG, "\n");

	}

	fm = f1 +1;

	FD_ZERO(&fds);
	FD_SET(f1, &fds);

	select(fm,NULL, &fds, NULL, NULL);

	if( FD_ISSET(f1, &fds) )
	{

		switch (p->common_header.protocol_info.itsnet_header_type)
		{

		case itsnet_geobroadcast_id:
			l=l-(sizeof(struct itsnet_common_header)+sizeof(struct itsnet_geobroadcast_t) - 2*ITSNET_DATA_SIZE);

			for(i=0; i<l; i++)
			{
				if(dbg_level>4) fprintf(LOG, " %2x : ",p->payload.itsnet_geobroadcast.payload[i]& 0xffff);
				//buf[i]=ntohs(p->payload.itsnet_geobroadcast.payload[i]);
				// Use this for ITSNET-AMD
				buf[i]=p->payload.itsnet_geobroadcast.payload[i];
			}

			pthread_mutex_lock(&mutex);

			n=write(f1,buf,sizeof(buf));
			pthread_mutex_unlock(&mutex);

			fprintf(LOG, "S <== %s IP packet (%i bytes)\n", dev1, n);
#if 1
			x=0;
			for(i=0; i<l ; i++)
			{

				/* TCP/IP network byte order to host byte order 
				   (which is little-endian on Intel processors) */

				// Don't Use this for ITSNET-AMD
				//buf[i] = htons(buf[i]);

				/* Parse Source address */
				if (i>3 && i<12)
				{
					src[x] = buf[i];
					if(dbg_level > 4) fprintf(LOG, " %x :",src[x]);
					x++;
				}
			}

			y=0;

			for(i=0; i<l ; i++)
			{
				/* Parse Destination address */
				if (i>11 && i<20)
				{
					dst[y] = buf[i];
					if(dbg_level > 4) fprintf(LOG, " %x :",dst[y]);
					y++;
				}

			}

			fprintf(LOG, "\t(SRC) %x:%x:%x:%x:%x:%x:%x:%x --> ",
					src[0],src[1],src[2],src[3],src[4],src[5],src[6],src[7]);
			fprintf(LOG, "(DST) %x:%x:%x:%x:%x:%x:%x:%x\n",
					dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7]);
			sprintf(&destination, "%x:%x:%x:%x:%x:%x:%x:%x",
					dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7]);
#endif

			break;


		case itsnet_unicast_id:

			l=l-(sizeof(struct itsnet_common_header)+sizeof(struct itsnet_unicast_t) - 2*ITSNET_DATA_SIZE);
			for(i=0; i<l; i++)
			{
				if (dbg_level > 4) fprintf(LOG, "%2x : ",p->payload.itsnet_unicast.payload[i]& 0xffff);
				//buf[i]=ntohs(p->payload.itsnet_unicast.payload[i]);

				// Use this for ITSNET-AMD
				buf[i]=p->payload.itsnet_unicast.payload[i];

			}

			pthread_mutex_lock(&mutex);
			n=write(f1,buf,l);
			pthread_mutex_unlock(&mutex);

			fprintf(LOG, "S <== %s IP packet (%i bytes)\n", dev1, n);
#if 1
			x=0;

			for(i=0; i<l ; i++)
			{
				/* TCP/IP network byte order to host byte order 
				   (which is little-endian on Intel processors) */

				// Don't Use this for ITSNET-AMD
				//buf[i] = htons(buf[i]);

				/* Parse Source address */
				if (i>3 && i<12)
				{
					src[x] = buf[i];
					if(dbg_level > 4) fprintf(LOG, " %x :",src[x]);
					x++;
				}
			}

			y=0;
			for(i=0; i<l ; i++)
			{
				/* Parse Destination address */
				if (i>11 && i<20)
				{
					dst[y] = buf[i];
					if(dbg_level > 4) fprintf(LOG, " %x :",dst[y]);
					y++;
				}

			}

			fprintf(LOG, "\t(SRC) %x:%x:%x:%x:%x:%x:%x:%x --> ",src[0],src[1],src[2],src[3],src[4],src[5],src[6],src[7]);
			fprintf(LOG, "(DST) %x:%x:%x:%x:%x:%x:%x:%x\n",dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7]);

#endif

			break;



		case itsnet_geoanycast_id:

			l=l-(sizeof(struct itsnet_common_header)+sizeof(struct itsnet_geoanycast_t) - 2*ITSNET_DATA_SIZE);
			for(i=0; i<l; i++)
			{
				fprintf(LOG, "%2x : ",p->payload.itsnet_geoanycast.payload[i]& 0xffff);
				//buf[i]=ntohs(p->payload.itsnet_geoanycast.payload[i]);
				// Use this for ITSNET-AMD
				buf[i]=p->payload.itsnet_geoanycast.payload[i];

			}

			pthread_mutex_lock(&mutex);
			n=write(f1,buf,l/2);
			pthread_mutex_unlock(&mutex);

			break;



		default:
		{
			fprintf(LOG, "unknown packet\n");

		}

		}

	if (dbg_level > 0)  fflush(LOG);}


}
