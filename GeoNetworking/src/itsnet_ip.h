/***************************************************************************
 *                              CarGeo6 Project                            *
 *              C2CNet-IPv6 adaptation layer implementation                *
 *                        conforming with GeoNet D2.2                      *
 *                                                                         *
 *                      Copyright(C)2010 ESPRIT-INRIA                      *
 *                                                                         *
 *                                                                         *
 *   Authors:                                                              *
 *   Hichem BARGAOUI <barghich@gmail.com>                                  *
 *   Anouar CHEMEK <anouar.chemek@gmail.com>                               *
 *   Manabu TSUKADA <Manabu.Tsukada@inria.fr>                              *
 *   Thouraya TOUKABRI <thouraya.toukabri@inria.fr>                        *
 *									   *
 *									   *
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
 * @file itsnet_ip.h
 * ip tunnel code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 * @author Manabu TSUKADA
 * @author Thouraya TOUKABRI
 */

#ifndef _ITSNET_IP_H_
#define _ITSNET_IP_H_ 1


int itsnet_iplistener_init(void);

int itsnet_ip_geobroadcast_tnl(short * buf, int l, uint32_t latitude, uint32_t longitude,double geo_area_size);

int itsnet_ip_geounicast_tnl(short * buf, int l,uint32_t latitude, uint32_t longitude,itsnet_node_id node_id);

int itsnet_ip_geoanycast_tnl(short * buf, int l, uint32_t latitude, uint32_t longitude,double geo_area_size);

void itsnet_ip_receive(struct itsnet_packet *p);

int get_c2cid_from_ip_nexthp(char *abuf);

uint8_t convert_char_to_hexa(char c);

int iproute_get(char *destination);

int tun_alloc(char *dev);

int find_default_route(char *default_route);

#endif	/* _ITSNET_IP_H_*/
