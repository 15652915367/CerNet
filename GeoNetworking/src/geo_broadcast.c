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
 * @file geo_broadcast.c
 * geo broadcast code.
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

#include "geo_broadcast.h"
#include "routing_manager.h"
#include "itsnet_ip.h"
#include "itsnet.h"

#include "cbf.h"

struct list_head* node_list_entry;

/**
   * itsnet_geobroadcast_send
   * @param p itsnet_packet
   */

int itsnet_geobroadcast_send(struct itsnet_packet *p)
{

	int result=0;
	char* dest_mac;

	/*itsnet_processed_packet_list_add(&processed_packet_list,
	                                 p->payload.itsnet_geobroadcast.source_position_vector.time_stamp,
	                                 p->payload.itsnet_geobroadcast.source_position_vector.node_id);

	itsnet_processed_packet_list_print(&processed_packet_list);*/

	if(isrelevant(p->payload.itsnet_geobroadcast.source_position_vector.latitude,p->payload.itsnet_geobroadcast.source_position_vector.longitude,p->payload.itsnet_geobroadcast.dest_latitude,p->payload.itsnet_geobroadcast.dest_longitude,p->payload.itsnet_geobroadcast.geo_area_size))
	{

		fprintf(LOG, "\tbelong to geo area send brodacast mode \n");
		if ( itsnet_packet_send(p, get_broadcast_mac_id()) == -1 )
		{
			fprintf(stderr,"erreur envoie packet \n");
			result = -1;
		}

		free(p);

		if (dbg_level > 0) fflush(LOG);
		return result;
	}
	else if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0)
	{
		//pl
		return(itsnet_greedy_send(p));
		//return(itsnet_contention_send(p));
	}
	else
	{
		fprintf(LOG, "no neighbor put in store and forward \n");
		itsnet_store(p,0);
	}
	free(p);
	if (dbg_level > 0) fflush(LOG);
	return result;
}



/**
   * handle the given packet
   * @param p itsnet_packet
   */

int itsnet_geobroadcast_handler(struct itsnet_packet *p)
{

	struct itsnet_position_vector pos;
	int result=0;
	itsnet_node  node;
	itsnet_node  *n;
	itsnet_event e;
	char* dest_mac;

	pos=get_position_vector();

	if ( check_security(p) == 1 && node_id_cmp(p->payload.itsnet_geobroadcast.source_position_vector.node_id, get_ego_node_id()) != 0)
	{

		if (dbg_level > 4) fprintf(LOG, "security chck succed \n");

		if(list_find_packet(&processed_packet_list, p->payload.itsnet_geobroadcast.source_position_vector.time_stamp,
				p->payload.itsnet_geobroadcast.source_position_vector.node_id) != NULL )
		{
			fprintf(LOG, "packed already processed drop it \n");
			free(p);
			return 0;

		}
		
		if (dbg_level > 4) fprintf(LOG, "packed not already processed  \n");

		itsnet_processed_packet_list_add(&processed_packet_list,
		                                 p->payload.itsnet_geobroadcast.source_position_vector.time_stamp,
		                                 p->payload.itsnet_geobroadcast.source_position_vector.node_id);

		/* update/add S & F*/

		/*TODO Source & Forwarder  Step 3 : Geonet Specification*/

		if(!isrelevant(pos.latitude,pos.longitude,p->payload.itsnet_geobroadcast.dest_latitude,
		               p->payload.itsnet_geobroadcast.dest_longitude,p->payload.itsnet_geobroadcast.geo_area_size) &&
		        isrelevant(p->common_header.forwarder_position_vector.latitude,p->common_header.forwarder_position_vector.longitude,
		                   p->payload.itsnet_geobroadcast.dest_latitude,p->payload.itsnet_geobroadcast.dest_longitude,
		                   p->payload.itsnet_geobroadcast.geo_area_size))
		{

			fprintf(LOG, "J does not belong to the geo-area & forwarder is belong to the geo-area\n");
			free(p);
			return 0;

		}

		fprintf(LOG, "\tI'm in the geo-area \n");

		if(isrelevant(pos.latitude,pos.longitude,p->payload.itsnet_geobroadcast.dest_latitude,
		              p->payload.itsnet_geobroadcast.dest_longitude,p->payload.itsnet_geobroadcast.geo_area_size))
		{
			fprintf(LOG, "\tThe packet is destinated to this geo-area \n");
			/* Select the destination of received packet : IP or Transport*/

			fprintf(LOG, "\tThe next header is : %d\n",p->common_header.protocol_info.itsnet_next_header);
			if(p->common_header.protocol_info.itsnet_next_header == transport)
			{
				/*Send packet to upper layer */
				fprintf(LOG, "C2C TRANSPORT PACKET is RECEIVED\n");
				itsnet_geobroadcast_indication_send(p);
			}
			if(p->common_header.protocol_info.itsnet_next_header == ip)
			{
				fprintf(LOG, "\tIP PACKET IN C2C PACKET is RECEIVED \n");
				itsnet_ip_receive(p);
			}


			/*update last forward J in common header*/
			//p->common_header.forwarder_position_vector=pos;


			/*if (  itsnet_packet_send(p, get_broadcast_mac_id()) == -1 )
			{
				fprintf(stderr, "erreur envoie packet \n");
				result = -1;
			}*/
			//pl
			return(itsnet_contention_handler(p));
			//return(itsnet_greedy_handler(p));

			free(p);
			if (dbg_level > 0) fflush(LOG);
			return result;
		}


		if(dbg_level > 4) fprintf(LOG, "Start forwarding Parts \n");

		if(!isrelevant(p->common_header.forwarder_position_vector.latitude,p->common_header.forwarder_position_vector.longitude,
		               p->payload.itsnet_geobroadcast.dest_latitude,p->payload.itsnet_geobroadcast.dest_longitude,p->payload.itsnet_geobroadcast.geo_area_size))

		{
			/*Todo forwarding & update last forwarder J in common header*/

			if(node_id_cmp(p->common_header.forwarder_position_vector.node_id,get_ego_node_id()))
			{
				print_node_id(p->common_header.forwarder_position_vector.node_id);
				printf(LOG, "I'm not the selected forwarder \n");
				return 0;

			}
			if (dbg_level > 4) fprintf(LOG, "The packet should  be forwarded towards D\n");


			if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0)
			{
				//pl
				return(itsnet_greedy_handler(p));	
				//return(itsnet_contention_handler(p));
			}
			else
			{

				fprintf(LOG, "put P in the store and forward buffer\n");

				/*put P in the store and forward buffer*/
				itsnet_store(p,0);

			}
		}
		if (dbg_level > 0) fflush(LOG);
	}

	else
	{
		free(p);
		return result;
	}
}
