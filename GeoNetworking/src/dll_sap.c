/***************************************************************************
 *                              CarGeo6 Project                            *
 *         C2CNet layer implementation conforming with GeoNet D2.2         *
 *                                                                         *
 *                      Copyright(C)2010 ESPRIT-INRIA                      *
 *                                                                         *
 *                                                                         *
 *   Authors:                                                              *
 *   Lamis AMAMOU <lamiss.amamou@gmail.com>		                   *
 *   Hichem BARGAOUI <barghich@gmail.com>                                  *
 *   Anouar CHEMEK <anouar.chemek@gmail.com>                               *
 *   							                   *
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
 * @file dll_sap.c
 * dll sap code.
 * @author Lamis AMAMOU
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

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

#include <sys/socket.h>
#include <sys/ioctl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>

#include "routing_manager.h"

#include "geo_unicast.h"
#include "geo_anycast.h"
#include "geo_broadcast.h"
#include "dll_sap.h"
#include "tqueue.h"
#include "itsnet_parser.h"
#include "itsnet.h"
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>



static int sock;/*Socketdescriptor*/
static struct sockaddr_ll socket_address;
static unsigned char src_mac[ETH_MAC_LEN]; /*our MAC address*/

static pthread_mutex_t mutex;
static pthread_t itsnet_packet_listener;
static void process_itsnet_packet(struct itsnet_packet *,struct mac_addr mac_id);
char * device=NULL;

//static unsigned char test_mac[ETH_MAC_LEN]={0x00,0x1B,0x9E,0X07,0Xc1,0xf3};


/**
 * @brief Get our Mac address
 *
 * This function permits to get device Mac address
 */
int get_mac_addr(void)
{

	struct ifreq ifr;
	int ifindex = 0;			/*Ethernet Interface index*/
	int i;
	/*retrieve device interface index*/
	device=Device_parse();
	strncpy(ifr.ifr_name, device, IFNAMSIZ);
	if (ioctl(sock, SIOCGIFINDEX, &ifr) == -1)
	{
		return -1;
	}
	ifindex = ifr.ifr_ifindex;

	/*Socketdescriptor*/

	/*retrieve corresponding MAC*/
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) == -1)
	{

		return -1;
	}
	for (i = 0; i < 6; i++)
	{
		src_mac[i] = ifr.ifr_hwaddr.sa_data[i];

	}

	return 0;
}

/**
 * @brief Try to open socket
 *
 * @returns 0 if succeed / < 0 if failed
 *
 * This function opens AF_PACKET socket
 */
int open_sock_raw(void)
{
	/*Try to open socket*/
	sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock != -1)
		return 0;

	return -1;
}

/**
 * @brief Prepare socket address
 *
 * This function permits to pf packet socket
 */
void create_raw_socket(void)
{
	device=Device_parse();
	/*prepare sockaddr_ll*/
	socket_address.sll_family   = PF_PACKET;
	socket_address.sll_protocol = htons(0x0707);
	socket_address.sll_ifindex  = if_nametoindex(device);
	socket_address.sll_halen    = 0;
	return;
}

static void *packet_listener(void *param)
{
	char buffer[BUF_SIZE];
	int length;				/*length of received packet*/
	unsigned char* etherhead = buffer;	/*Pointer to Ethenet Header*/
	struct ethhdr *eh = (struct ethhdr *)etherhead; /*Another pointer to ethernet header*/
	unsigned char* test = buffer+14;
	struct itsnet_packet *data;
	data = (struct itsnet_packet *)test;
	struct mac_addr mac_id;

	while (1)
	{
		/*Wait for incoming packet...*/
		length = recvfrom(sock, buffer, BUF_SIZE, 0, NULL, NULL);

		if (length == -1)
		{
			fprintf(stderr, "Error while receiving\n");
		}
		/*See if we should answer (Ethertype == 0x0707 && destination address == our MAC)*/
		if (eh->h_proto == 0x0707)
		{

			if ((memcmp( (const void*)eh->h_source, (const void*)src_mac, ETH_MAC_LEN) != 0)/*&&(memcmp( (const void*)eh->h_source, (const void*)test_mac, ETH_MAC_LEN) != 0)*/ )
			{
				memcpy(mac_id.address,eh->h_source,ETH_MAC_LEN);
				process_itsnet_packet(data,mac_id);
			}
			else
				fprintf(LOG, "My Packet\n");
		}
	}

	return;
}

int dll_sap_init(void* data)
{
	pthread_mutexattr_t mattrs;
	pthread_mutexattr_init(&mattrs);

	fprintf(LOG, "dll_sap_init...\n");

	if (open_sock_raw()<0) /*Open socket to receive its packet*/
		return -1;

	create_raw_socket();

	if (get_mac_addr()<0) /*Try to get specified DEVICE Mac address*/
		return -1;

	fprintf(LOG, "start packet_listener\n");

	if (pthread_mutex_init(&mutex, &mattrs) || pthread_create(&itsnet_packet_listener, NULL, packet_listener, NULL))
	{
		return -1;
	}
	fprintf(LOG, "dll_sap_init\n");
	if(dbg_level > 0) fflush(LOG);
	return 0;
}


/**
 * @brief Send itsnet packet
 *
 * @param packet its packet to send
 * @param dest_mac destination mac address
 *
 * This function permits to send packet to a desrination mac address
 *
 */
void itsnet_packet_send(struct itsnet_packet *packet, char* dest_mac)
{

	char * buffer = (char*)malloc(BUF_SIZE); 			/*Buffer for ethernet frame*/
	struct itsnet_ll_frame *ll_frame;
	int packet_size;

	memset(buffer,0,ETH_FRAME_LEN);
	unsigned char* etherhead = buffer;					/*Pointer to ethenet header*/
	struct ethhdr *eh = (struct ethhdr *)etherhead;		/*Another pointer to ethernet header*/
	int sent;
	pthread_mutex_lock(&mutex);

	socket_address.sll_addr[0]  = dest_mac[0];
	socket_address.sll_addr[1]  = dest_mac[1];
	socket_address.sll_addr[2]  = dest_mac[2];
	socket_address.sll_addr[3]  = dest_mac[3];
	socket_address.sll_addr[4]  = dest_mac[4];
	socket_address.sll_addr[5]  = dest_mac[5];
	socket_address.sll_addr[6]  = 0x00;
	socket_address.sll_addr[7]  = 0x00;

	/*prepare buffer*/

	memcpy((void*)buffer,(void*)dest_mac, ETH_ALEN);
	memcpy((void*)(buffer+ETH_ALEN), (void*)src_mac, ETH_ALEN);
	eh->h_proto = ETH_P_NULL;
	/*fill it with its data....*/
	packet_size=packet->common_header.payload_lenght;
	memcpy((void*)(buffer+14),packet,packet_size);

	/*send packet*/

	sent = sendto(sock, buffer,(packet_size + 14), 0, (struct sockaddr*)&socket_address, sizeof(socket_address));

	if (sent == -1)
	{
		return;
	}

	fprintf(LOG, "S <== %s itsnet_packet_send (MAC=%x)\n", device, dest_mac[0]);
	pthread_mutex_unlock(&mutex);

	free(buffer);
	if (dbg_level > 0) fflush(LOG);

	return;
}

static void process_itsnet_packet(struct itsnet_packet *p,struct mac_addr mac_id)
{


	struct itsnet_packet *packet;

	packet=(struct itsnet_packet*) malloc (sizeof(struct itsnet_packet));
	memcpy(packet,p, sizeof(*p));

	switch (packet->common_header.protocol_info.itsnet_header_type)
	{

	case itsnet_any_id:
		fprintf(LOG, "R ==> %s value of packet unspecified \n", device);
		break;
	case itsnet_beacon_id:
		fprintf(LOG, "R ==> %s value of packet Beacon\n", device);
		itsnet_beacon_handler(packet,mac_id);
		break;
	case itsnet_unicast_id:
		fprintf(LOG, "R ==> %s value of packet Geo-unicast \n", device);
		itsnet_geounicast_handler(packet);
		break;
	case itsnet_geoanycast_id:
		fprintf(LOG, "R ==> %s value of packet Geo-anycast \n", device);
		itsnet_geoanycast_handler(packet);
		break;
	case itsnet_geobroadcast_id:
		fprintf(LOG, "R ==> %s value of packet Geo-broadcast \n", device);
		itsnet_geobroadcast_handler(packet);
		break;
	case itsnet_tsb_id:
		fprintf(LOG, "R ==> %s value of packet Topologically-scoped broadcast\n", device);
		itsnet_geotopo_handler(packet);
		break;
	case itsnet_ls_id:
		fprintf(LOG, "R ==> %s value of packet Location service\n", device);
		itsnet_location_handler(packet);
		break;
	default:
		free(packet);
		fprintf(LOG, "R ==> %s Unknown packet type\n", device);
		break;
	}
	if (dbg_level > 0) fflush(LOG);

}
