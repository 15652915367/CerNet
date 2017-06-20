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
 * @file geo_anycast.c
 * geo anycast code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "geo_anycast.h"
#include "itsnet_pseudonym.h"
#include "itsnet_security.h"
#include "itsnet_listener.h"
#include "position_sensor.h"

#include "location_table.h"
#include "itsnet_parser.h"
#include "location_service.h"
#include "store_forward.h"
#include "position_calculation.h"
#include "routing_manager.h"
#include "itsnet_ip.h"
#include "itsnet.h"

LIST_HEAD(processed_packet_list);

//init 
LIST_HEAD(routing_packet_list);

struct list_head* node_list_entry;



/**
   * itsnet_geoanycast_send
   * @param p itsnet_packet
   */



int itsnet_geoanycast_send(struct itsnet_packet *p)
{


	int result=0;
	char* dest_mac;

	/*itsnet_processed_packet_list_add(&processed_packet_list,
	                                 p->payload.itsnet_geoanycast.source_position_vector.time_stamp,
	                                 p->payload.itsnet_geoanycast.source_position_vector.node_id);

	itsnet_processed_packet_list_print(&processed_packet_list);*/

	if(isrelevant(p->payload.itsnet_geoanycast.source_position_vector.latitude,p->payload.itsnet_geoanycast.source_position_vector.longitude,p->payload.itsnet_geoanycast.dest_latitude,p->payload.itsnet_geoanycast.dest_longitude,p->payload.itsnet_geoanycast.geo_area_size))
	{
		fprintf(LOG, "belong to geo area  \n");
		free(p);
		return 0;
	}
	else if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0)
	{

		fprintf(LOG, "The Packet should be forwarded to the destination \n");
		p->common_header.forwarder_position_vector.node_id=itsnet_select_forwarder(&neighbor_list,p->common_header.forwarder_position_vector,p->payload.itsnet_geoanycast.dest_latitude,p->payload.itsnet_geoanycast.dest_longitude);
		
		dest_mac=(list_find_node(&neighbor_list,&(p->common_header.forwarder_position_vector.node_id)))->mac_id.address;
		if (  itsnet_packet_send(p,dest_mac) == -1 )
		{
			fprintf(LOG, "erreur envoie packet \n");
			result = -1;
		}
		free(p);
		return result;

	}
	else
	{
		printf("no neighbor put in store and forward \n");
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
int itsnet_geoanycast_handler(struct itsnet_packet *p)
{

	struct itsnet_position_vector pos;
	int result;
	itsnet_node  node;
	itsnet_node  *n;
	itsnet_event e;
	char* dest_mac;

	pos=get_position_vector();

	fprintf(LOG, "NodeId of the forwarder \n");
	print_node_id(p->common_header.forwarder_position_vector.node_id);


	if ( check_security(p) == 1 && node_id_cmp(get_ego_node_id(),p->payload.itsnet_geoanycast.source_position_vector.node_id) != 0)
	{
		fprintf(LOG, "security check succed \n");

		if(list_find_packet(&processed_packet_list,p->payload.itsnet_geoanycast.source_position_vector.time_stamp,p->payload.itsnet_geoanycast.source_position_vector.node_id) != NULL )
		{
			fprintf(LOG, "packed already processed drop it \n");
			free(p);
			return 0;

		}

		fprintf(LOG, "packed not already processed  \n");

		itsnet_processed_packet_list_add(&processed_packet_list,
		                                 p->payload.itsnet_geoanycast.source_position_vector.time_stamp,
		                                 p->payload.itsnet_geoanycast.source_position_vector.node_id);

		/* update/add S & F*/

		/*TODO Source & Forwarder*/

		if(!isrelevant(pos.latitude,pos.longitude,p->payload.itsnet_geoanycast.dest_latitude,
		               p->payload.itsnet_geoanycast.dest_longitude,p->payload.itsnet_geoanycast.geo_area_size) &&
		        isrelevant(p->common_header.forwarder_position_vector.latitude,p->common_header.forwarder_position_vector.longitude,
		                   p->payload.itsnet_geoanycast.dest_latitude,p->payload.itsnet_geoanycast.dest_longitude,
		                   p->payload.itsnet_geoanycast.geo_area_size))
		{

			fprintf(LOG, "J does not belong to the geo-area & forwarder is belong to the geo-area\n");
			free(p);
			return 0;

		}

		fprintf(LOG, "J  belong to the geo-area \n");

		if(isrelevant(pos.latitude,pos.longitude,p->payload.itsnet_geoanycast.dest_latitude,
		              p->payload.itsnet_geoanycast.dest_longitude,p->payload.itsnet_geoanycast.geo_area_size))
		{
			/* Select the destination of received packet : IP or Transport*/
			fprintf(LOG, "The packet is destinated to this geo-area \n");
			fprintf(LOG, "JUST SEND PACKET TO UPPER LAYER or TO THE IP HANDLER");
			fprintf(LOG, "The next header is : %d\n",p->common_header.protocol_info.itsnet_next_header);
			if(p->common_header.protocol_info.itsnet_next_header == transport)
			{
				/*Send packet to upper layer */
				result=itsnet_geoanycast_indication_send(p);
			}

			if(p->common_header.protocol_info.itsnet_next_header == ip)
			{
				/*Send packet to IP packet handler */
				fprintf(LOG, "IP PACKET IN C2C PACKET is RECEIVED \n");
				itsnet_ip_receive(p);
			}
			if (dbg_level) fflush (LOG);
			free(p);
			return result;
		}

		fprintf(LOG, "Start forwarding Parts \n");


		if(!isrelevant(p->common_header.forwarder_position_vector.latitude,p->common_header.forwarder_position_vector.longitude,
		               p->payload.itsnet_geoanycast.dest_latitude,p->payload.itsnet_geoanycast.dest_longitude,p->payload.itsnet_geoanycast.geo_area_size))

		{

			if(node_id_cmp(p->common_header.forwarder_position_vector.node_id,get_ego_node_id()))
			{
				print_node_id(p->common_header.forwarder_position_vector.node_id);
				fprintf(LOG, "I'm not the selected forwarder \n");
				return 0;

			}
			fprintf(LOG, "The packet should  be forwarded towards D\n");

			if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0)
			{
				p->common_header.forwarder_position_vector=pos;
				p->common_header.forwarder_position_vector.node_id=itsnet_select_forwarder(&neighbor_list,pos,p->payload.itsnet_geoanycast.dest_latitude,p->payload.itsnet_geoanycast.dest_longitude);
				
				dest_mac=(list_find_node(&neighbor_list,&(p->common_header.forwarder_position_vector.node_id)))->mac_id.address;
				if (  itsnet_packet_send(p,dest_mac) == -1 )
				{
					fprintf(stderr, "erreur envoie packet \n");
					result = -1;
				}

				free(p);
				return result;


			}
			else
			{
				/*put P in the store and forward buffer*/
				fprintf(LOG, "put P in the store and forward buffer\n");
				itsnet_store(p,0);

			}

		}
	if (dbg_level > 0) fflush(LOG);
	}
	else
	{
		free(p);
		return 0;
	}
}
