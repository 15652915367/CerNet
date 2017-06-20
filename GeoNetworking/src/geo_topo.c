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
 * @file geo_topo.c
 * geo topo code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "itsnet_pseudonym.h"
#include "itsnet_security.h"
#include "itsnet_listener.h"
#include "position_sensor.h"

#include "location_table.h"
#include "itsnet_parser.h"
#include "location_service.h"
#include "store_forward.h"
#include "position_calculation.h"

#include "geo_topo.h"
#include "routing_manager.h"
#include "itsnet_ip.h"
#include "itsnet.h"

struct list_head* node_list_entry;



/**
  * itsnet_geotopo_send
  * @param p itsnet_packet
  */
int itsnet_geotopo_send(struct itsnet_packet *p)
{
	int result=0;

	itsnet_processed_packet_list_add(&processed_packet_list,
	                                 p->payload.itsnet_tsb.source_position_vector.time_stamp,
	                                 p->payload.itsnet_tsb.source_position_vector.node_id);

	itsnet_processed_packet_list_print(&processed_packet_list);

	if (  itsnet_packet_send(p, get_broadcast_mac_id()) == -1 )
	{
		fprintf(stderr,"erreur envoie packet \n");
		result = -1;
	}
	free(p);
	return result;
}

/**
  * handle the given packet
  * @param p itsnet_packet
  */
int itsnet_geotopo_handler(struct itsnet_packet *p)
{


	struct itsnet_position_vector pos;
	int result=0;
	itsnet_node  node;
	itsnet_node  *n;
	itsnet_event e;

	pos=get_position_vector();

	if ( check_security(p) == 1 )
	{

		fprintf(LOG,"security chck succed \n");

		if(list_find_packet(&processed_packet_list,p->payload.itsnet_tsb.source_position_vector.time_stamp,p->payload.itsnet_tsb.source_position_vector.node_id) != NULL)
		{
			fprintf(LOG, "packed already processed drop it \n");
			if (dbg_level > 0) fflush(LOG);
			free(p);
			return 0;

		}

		fprintf(LOG, "packed not already processed  \n");

		itsnet_processed_packet_list_add(&processed_packet_list,
		                                 p->payload.itsnet_tsb.source_position_vector.time_stamp,
		                                 p->payload.itsnet_tsb.source_position_vector.node_id);



		/*Send packet to upper layer */
		fprintf(LOG, "C2C PACKET is RECEIVED \n");
		itsnet_geobroadcast_indication_send(p);

		fprintf(LOG, "decrease hoplimit value \n");
		p->common_header.hop_limit -=1;

		if(p->common_header.hop_limit == 0)
		{

			fprintf(LOG,"The limit of hop number \n packet dropped");
			free(p);
			if (dbg_level > 0) fflush(LOG);
			return result;

		}
		else
		{

			fprintf(LOG, "There is %d to achieve for geo Topobroadcast action \n",p->common_header.hop_limit);
			p->common_header.forwarder_position_vector=pos;
			p->common_header.forwarder_position_vector.node_id=get_ego_node_id();
			fprintf(LOG, "send packet in broadcast mode \n");
			if (  itsnet_packet_send(p, get_broadcast_mac_id()) == -1 )
			{
				fprintf(stderr, "erreur envoie packet \n");
				result = -1;
			}

			free(p);
			if (dbg_level > 0) fflush(LOG);
			return result;

		}

		if (dbg_level > 0) fflush(LOG);

	}

	else
	{
		free(p);
		return result;
	}

}
