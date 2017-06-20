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
 * @file itsnet_pseudonym.c
 * itsnet pseudonym code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#include <stdio.h>
#include <stdlib.h>

#include "itsnet_pseudonym.h"
#include "itsnet_parser.h"


static  itsnet_node_id ego_node_id;


/**
  * set ego_node_id
  * @param itsnet_node_id  id
  */

void set_ego_node_id(struct itsnet_node_id * node_id)
{
	ego_node_id=*(node_id);
}


/**
  * get ego_node_id
  *@return itsnet_node_id
  */

itsnet_node_id  get_ego_node_id()
{
	return ego_node_id;
}


int node_id_cmp(struct itsnet_node_id node_id1,struct itsnet_node_id  node_id2)
{
	int i;


	for(i=0; i<8; i++)
	{

		if(node_id1.id[i] != node_id2.id[i])
			return -1;

	}

	return 0;


}
