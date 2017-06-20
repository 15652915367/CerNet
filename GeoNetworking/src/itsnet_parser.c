/***************************************************************************
 *                              CarGeo6 Project                            *
 *         C2CNet layer implementation conforming with GeoNet D2.2         *
 *                                                                         *
 *                      Copyright(C)2010 ESPRIT-INRIA                      *
 *                                                                         *
 *                                                                         *
 *   Authors:                                                              *
 *   Hichem BARGAOUI <barghich@gmail.com>                                  *
 *   Anouar CHEMEK <anouar.chemek@gmail.com>                               *
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
 * @file itsnet_parser.c
 * itsnet conf code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */


#include <stdio.h>

#include <confuse.h>
#include <stdlib.h>
#include <net/ethernet.h>

#include "itsnet_parser.h"


static  mac_addr broadcast_mac_id;


/**
  * set ego_node_id
  * @param itsnet_node_id  id
  */

void set_broadcast_mac_id(struct mac_addr * mac_id)
{
	broadcast_mac_id=*(mac_id);
}


/**
  * get ego_node_id
  *@return itsnet_node_id
  */

mac_addr * get_broadcast_mac_id()
{
	return &broadcast_mac_id;
}


static cfg_opt_t opts[] =
{
	CFG_STR("Device",NULL, CFGF_NONE),
	CFG_INT("ItsnetDataSize", 1500, CFGF_NONE),
	CFG_STR("BroadcastMac", "ff:ff:ff:ff:ff:ff", CFGF_NONE),
	CFG_INT("MaxNeighbor", 10, CFGF_NONE),
	CFG_INT("DebugLevel", 3,   CFGF_NONE),
	CFG_INT("LocationTableEntry",5, CFGF_NONE),
	CFG_STR("EthPItsnet","0x707", CFGF_NONE),
	CFG_BOOL("SendBeacon", cfg_false, CFGF_NONE),
	CFG_BOOL("DetachFromTTY", cfg_false, CFGF_NONE),
	CFG_BOOL("IPNextHopCache", cfg_false, CFGF_NONE),
	CFG_STR("NodeId", "aa:aa:bb:bb:cc:cc:00:22", CFGF_NONE),
	CFG_STR("ItsnetLog", "/var/log/itsnet.log" , CFGF_NONE),
	CFG_INT("AllNodeMcast_Radius", 500, CFGF_NONE),
	CFG_STR("MLD_GroupAddress", "ff0e:0:0:0:0:0:0:2", CFGF_NONE),
	CFG_INT("MLD_GroupAddress_Radius",1000, CFGF_NONE),
	CFG_FLOAT("InitialLatitude", 0, CFGF_NONE),
	CFG_FLOAT("InitialLongitude", 0, CFGF_NONE),
	CFG_FLOAT("InitialAltitude", 0, CFGF_NONE),
	CFG_FLOAT("InitialSpeed", 0, CFGF_NONE),
	CFG_FLOAT("InitialHeading", 0, CFGF_NONE),
	CFG_INT("AutoWifiPort", 0, CFGF_NONE),
	CFG_INT("AutoWifiInterval", 0, CFGF_NONE),
	CFG_STR("AutoWifiIpAddr", 0, CFGF_NONE),
	CFG_END()
};

static cfg_t *cfg;

/**
   * itsnet_cfg_parse
   * @param file
   * @return int
   */
int itsnet_cfg_parse(char * file)
{
	int ret = 0;
	cfg = cfg_init(opts, CFGF_NONE);

	ret = cfg_parse(cfg, file);
	if (ret == CFG_FILE_ERROR)
	{
		fprintf(stderr, "Cannot find configuration file %s\n", file);
		exit(1);
	}
	else if (ret == CFG_PARSE_ERROR)
	{
		fprintf(stderr, "Parse error in configuration file %s\n", file);
		return -2;
	}
	return 0;
}


/**
   * itsnet_aton
   * @param char*
   * @return itsnet_node_id
   */
struct itsnet_node_id * itsnet_aton(const char *asc)
{
	size_t cnt;

	struct itsnet_node_id * node_id=(struct itsnet_node_id*) malloc (sizeof(struct itsnet_node_id));

	for (cnt = 0; cnt < 8; ++cnt)
	{
		unsigned int number;
		char ch;

		ch = _tolower (*asc++);
		if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
			return NULL;
		number = isdigit (ch) ? (ch - '0') : (ch - 'a' + 10);

		ch = _tolower (*asc);
		if ((cnt < 7 && ch != ':') || (cnt == 7 && ch != '\0' && !isspace (ch)))
		{
			++asc;
			if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
				return NULL;
			number <<= 4;
			number += isdigit (ch) ? (ch - '0') : (ch - 'a' + 10);

			ch = *asc;
			if (cnt < 7 && ch != ':')
				return NULL;
		}

		/* Store result.  */
		node_id->id[cnt] = (unsigned char) number;

		/* Skip ':'.  */
		++asc;
	}
	return node_id;

}





/**
   * itsnet_cfg_clean
   */
void itsnet_cfg_clean()
{
	cfg_free(cfg);
}

/* Config file accessors */

/**
   * Device_parse
   * @param void
   * @return char
   */
char* Device_parse(void)
{
	return ((char*)cfg_getstr(cfg, "Device"));
}


/**
   * ItsnetDataSize_Parse
   * @param void
   * @return int
   */
int ItsnetDataSize_parse(void)
{
	return (cfg_getint(cfg,"ItsnetDataSize"));

}

/**
   * BroadcastMac_Parse
   * @param void
   * @return address
   */
mac_addr * BroadcastMac_parse(void)
{
	struct ether_addr *ea = NULL;
	ea = ether_aton(cfg_getstr(cfg, "BroadcastMac"));
	if(ea == NULL)
		return ea;
}

/**
   * NodeId_parse
   * @param void
   * @return itsnet_node_id
   */
struct itsnet_node_id * NodeId_parse(void)
{
	struct itsnet_node_id * ea;
	ea = itsnet_aton(cfg_getstr(cfg, "NodeId"));
	return ea;
}


/**
   * MLD_GroupAddress_parse
   * @param void
   * @return char
   */
char* MLD_GroupAddress_parse(void)
{
	return ((char*)cfg_getstr(cfg, "MLD_GroupAddress"));
}

/*
        CFG_INT("AllNodeMcast_Radius", 500, CFGF_NONE),
        CFG_STR("MLD_GroupAddress", "ff0e:0:0:0:0:0:0:2", CFGF_NONE),
        CFG_INT("MLD_GroupAddress_Radius",1000, CFGF_NONE),
*/

/**
   * AllNodeMcast_Radius_parse
   * @param void
   * @return int
   */
int AllNodeMcast_Radius_parse(void)
{
	return (cfg_getint(cfg,"AllNodeMcast_Radius"));
}

/**
   * MLD_GroupAddress_Radius_parse
   * @param void
   * @return int
   */
int MLD_GroupAddress_Radius_parse(void)
{
	return (cfg_getint(cfg,"MLD_GroupAddress_Radius"));
}



/**
   * ItsnetLog_parse
   * @param void
   * @return char
   */
char* ItsnetLog_parse(void)
{
	return ((char*)cfg_getstr(cfg, "ItsnetLog"));
}


/**
   * DebugLevel_parse
   * @param void
   * @return int
   */
int DebugLevel_parse(void)
{
	return (cfg_getint(cfg,"DebugLevel"));
}

/**
   * MaxNeighbor_parse
   * @param void
   * @return int
   */
int MaxNeighbor_parse(void)
{
	return (cfg_getint(cfg,"MaxNeighbor"));

}


/**
   * LocationTableEntry_parse
   * @param void
   * @return int
   */
int LocationTableEntry_parse(void)
{
	return (cfg_getint(cfg,"LocationTableEntry"));
}


/**
   * EthPItsnet_parse
   * @param void
   * @return char
   */
char* EthPItsnet_parse(void)
{
	return ((char*)cfg_getstr(cfg, "EthPItsnet"));

}


/**
   * SendBeacon_parse
   * @param void
   * @return int
   */
int SendBeacon_parse(void)
{
	return (cfg_getbool(cfg, "SendBeacon"));
}

/**
   * DetachFromTTY_parse
   * @param void
   * @return int
   */
int DetachFromTTY_parse(void)
{

	return (cfg_getbool(cfg, "DetachFromTTY"));
}

/**
   * DetachFromTTY_parse
   * @param void
   * @return int
   */
int IPNextHopCache_parse(void)
{

	return (cfg_getbool(cfg, "IPNextHopCache"));
}

/*** InitialLatitude_parse
   * @param void
   * @return float
   */
float InitialLatitude_parse(void)
{
	return (cfg_getfloat(cfg, "InitialLatitude"));
}

/*** InitialLongitude_parse
   * @param void
   * @return float
   */
float InitialLongitude_parse(void)
{
	return (cfg_getfloat(cfg, "InitialLongitude"));
}

/*** InitialAltitude_parse
   * @param void
   * @return float
   */
float InitialAltitude_parse(void)
{
	return (cfg_getfloat(cfg, "InitialAltitude"));
}

/*** InitialSpeed_parse
   * @param void
   * @return float
   */
float InitialSpeed_parse(void)
{
	return (cfg_getfloat(cfg, "InitialSpeed"));
}

/*** InitialHeading_parse
   * @param void
   * @return float
   */
float InitialHeading_parse(void)
{
	return (cfg_getfloat(cfg, "InitialHeading"));
}
/*
InitialLatitude = 48.4
InitialLongitude = 10.002
InitialAltitude = 48
InitialSpeed = 24
InitialHeading = 36
*/

/*** AutoWifiPort_parse
   * @param void 
   * @return int 
   */
int AutoWifiPort_parse(void)
{
      return (cfg_getint(cfg, "AutoWifiPort"));
}

/*** AutoWifiInterval_parse 
   * @param void
   * @return int 
   */
int AutoWifiInterval_parse(void)
{
      return (cfg_getint(cfg, "AutoWifiInterval"));
}

/*** AutoWifiIpAddr_parse 
   * @param void 
   * @return char 
   */
char *AutoWifiIpAddr_parse(void)
{
      return (cfg_getstr(cfg, "AutoWifiIpAddr"));
}

