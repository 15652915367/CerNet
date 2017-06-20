/***************************************************************************
 *                              CarGeo6 Project                            *
 *         C2CNet layer implementation conforming with GeoNet D2.2         *
 *                                                                         *
 *                      Copyright(C)2010 ESPRIT-INRIA                      *
 *                                                                         *
 *                                                                         *
 *   Authors:                                                              *
 *   Lamis AMAMOU <lamiss.amamou@gmail.com>                                *
 *   Hichem BARGAOUI <barghich@gmail.com>                                  *
 *   Anouar CHEMEK <anouar.chemek@gmail.com>                               *
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
 * @file dll_sap.h
 * dll sap code.
 * @author Lamis AMAMOU
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#ifndef _DLL_SAP_H_
#define _DLL_SAP_H_ 1

#include "itsnet_header.h"


#define ETH_P_ITSNET 0x0707



/* see linux/if_ether.h */
#define ETH_MAC_LEN        ETH_ALEN      /* Octets in one ethernet addr   */
#define ETH_HEADER_LEN     ETH_HLEN      /* Total octets in header.       */
#define ETH_MIN_FRAME_LEN  ETH_ZLEN      /* Min. octets in frame sans FCS */
#define ETH_USER_DATA_LEN  ETH_DATA_LEN  /* Max. octets in payload        */
#define ETH_MAX_FRAME_LEN  ETH_FRAME_LEN /* Max. octets in frame sans FCS */




#define BUF_SIZE 1514
#define ETH_P_NULL	   0x0707



/**
   * thread packet listener
   * @param *arg
   */
static void *packet_listener(void *arg);

/**
   * data link layer sap init
   * @param int
   */
int dll_sap_init(void*);

/**
   * dll_sap_cleanup
   * @param void
   */
static void dll_sap_cleanup(void);

/**
   * itsnet_packet_send
   * @param itsnet_packet packet
   */
void itsnet_packet_send(struct itsnet_packet *, char*);


int open_sock_raw(void);

int get_mac_addr(void);

#endif	/* _DLL_SAP_H_*/
