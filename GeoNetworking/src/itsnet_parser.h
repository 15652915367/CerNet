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
 * @file itsnet_parser.h
 * itsnet conf code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#ifndef _ITSNET_PARSER_H_
#define _ITSNET_PARSER_H_    1

#include <stdint.h>
#include "itsnet_header.h"



void set_broadcast_mac_id(struct mac_addr * mac_id);
mac_addr * get_broadcast_mac_id();

int itsnet_cfg_parse(char * file);

void itsnet_cfg_clean();

char* Device_parse(void);

int ItsnetDataSize_parse(void);


mac_addr * BroadcastMac_parse(void);


int MaxNeighbor_parse(void);


int LocationTableEntry_parse(void);


char* EthPItsnet_parse(void);


int SendBeacon_parse(void);


int DebugLevel_parse(void);


int DetachFromTTY_parse(void);
int IPNextHopCache_parse(void);

int AutoWifiPort_parse(void);

int AutoWifiInterval_parse(void);

char *AutoWifiIpAddr_parse(void);

struct itsnet_node_id * itsnet_aton(const char *asc);

struct itsnet_node_id *  NodeId_parse(void);
char* ItsnetLog_parse(void);

int AllNodeMcast_Radius_parse(void);
char* MLD_GroupAddress_parse(void);
int   MLD_GroupAddress_Radius_parse(void);
float InitialLatitude_parse(void);
float InitialLongitude_parse(void);
float InitialSpeed_parse(void);
float InitialHeading_parse(void);
float InitialAltitude_parse(void);

#endif /* _ITSNET_PARSER_H_  */
