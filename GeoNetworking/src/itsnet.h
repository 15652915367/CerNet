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
 * @file itsnet.h
 * itsnet code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */


int dbg_level;
FILE *LOG;

struct geoDestination {
        int AN_Mcast_radius;
	char* MLD_group;
	int MLD_group_radius;
};
struct geoDestination geoDst;

/*
struct itsnet_config {
	int aaaa;
	int bbbb;
}
struct itsnet_config conf;
*/

#ifndef _ITSNET_H_
#define _ITSNET_H_ 1

int itsnet_cleanup();

int itsnet_main (int argc, char **argv);

void itsnet_init_conf();

#endif	/* _ITSNET_H_ */
