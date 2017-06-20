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
 * @file geo_unicast.c
 * geo unicast code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "geo_unicast.h"
#include "itsnet_pseudonym.h"
#include "itsnet_security.h"
#include "itsnet_listener.h"
#include "position_sensor.h"

#include "location_table.h"
#include "itsnet_parser.h"
#include "location_service.h"
#include "store_forward.h"

#include "routing_manager.h"
#include "itsnet_ip.h"
#include "itsnet.h"


LIST_HEAD(location_packet_list);
LIST_HEAD(store_forward_packet_list);

struct list_head* node_list_entry;

/**
  * itsnet_geounicast_send
  * @param p itsnet_packet
  */
int itsnet_geounicast_send(struct itsnet_packet *p)
{
	int result=0;
	char* dest_mac;

/*	itsnet_processed_packet_list_add(&processed_packet_list,
	                                 p->payload.itsnet_unicast.source_position_vector.time_stamp,
	                                 p->payload.itsnet_unicast.source_position_vector.node_id);

	itsnet_processed_packet_list_print(&processed_packet_list);*/

	if (list_find_node(&neighbor_list,&(p->payload.itsnet_unicast.dest_node_id)) != NULL)
	{
		
		dest_mac=(list_find_node(&neighbor_list,&(p->payload.itsnet_unicast.dest_node_id)))->mac_id.address;
		if (  itsnet_packet_send(p,dest_mac) == -1 )
		{
			fprintf(stderr, "erreur envoie packet \n");
			result = -1;
		}
		free(p);

		fprintf(LOG, "\tNode destination is a neighbor (");
		print_node_id(p->payload.itsnet_unicast.dest_node_id);
		fprintf(LOG, ")\n");
		if (dbg_level > 0) fflush(LOG);
		return result;
		/*else, if the position of D is already provided by the upper layer or known from the
		location management table, then Dest_GeoLocation = geographical location of D
		*/
	}
	else if(p->payload.itsnet_unicast.dest_latitude == 0 && p->payload.itsnet_unicast.dest_longitude == 0)
	{

		itsnet_location_request(p);
		fprintf(LOG, "\tStarting Location Service psocess.... (finding node ");
		print_node_id(p->payload.itsnet_unicast.dest_node_id);
		fprintf(LOG, ")\n");
		if (dbg_level > 0) fflush(LOG);
		return 0;

	}
	else
	{
		fprintf(LOG, "The destination geo area is provided by the upper layer \n");
	}

	fprintf(LOG, "The destination Latitude & longitude is done \n");
	fprintf(LOG, "destination latitude %d\n destination longitude %d\n\n",p->payload.itsnet_unicast.dest_latitude,p->payload.itsnet_unicast.dest_longitude);
	fprintf(LOG, "starting forwarding process \n");

	if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0)
	{
		fprintf(LOG, "there are neighbours available around : forwarding \n");

		p->common_header.forwarder_position_vector.node_id=itsnet_select_forwarder(&neighbor_list,p->payload.itsnet_unicast.source_position_vector,p->payload.itsnet_unicast.dest_latitude,p->payload.itsnet_unicast.dest_longitude);
		
		if (p->common_header.forwarder_position_vector.node_id.id[6] != 0xca){
			result = -1;
			return result;
		}	
	
		dest_mac=(list_find_node(&neighbor_list,&(p->common_header.forwarder_position_vector.node_id)))->mac_id.address;
		if (  itsnet_packet_send(p,dest_mac) == -1 )
		{
			fprintf(stderr, "erreur envoie packet \n");
			result = -1;
		}
		free(p);
		if (dbg_level > 0) fflush(LOG);
		return result;

	}
	else
	{
		/*put P in the store and forward buffer*/
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
int itsnet_geounicast_handler(struct itsnet_packet *p)
{

	uint8_t brodcast_mac[MAX_LLA_LEN];
	struct itsnet_position_vector pos;
	int result=0;
	itsnet_node  node;
	itsnet_node  *n;
	itsnet_event e;
	struct timespec exp_in;
	tssetsec(exp_in, LOCATION_TABLE_ENTRY_EXP);
	struct timespec expire;
	char* dest_mac;

	clock_gettime(CLOCK_REALTIME, &expire);
	tsadd(expire, exp_in, expire);


	pos=get_position_vector();

	if ( check_security(p) == 1 && node_id_cmp(p->payload.itsnet_unicast.source_position_vector.node_id,get_ego_node_id()) != 0)
	{

		if (dbg_level > 4) fprintf(LOG, "security check succed \n");

		if(list_find_packet(&processed_packet_list,p->payload.itsnet_unicast.source_position_vector.time_stamp,p->payload.itsnet_unicast.source_position_vector.node_id) != NULL )
		{

			free(p);
			fprintf(LOG, " \n PACKET IS DROPPED BECAUSE PROCESSED \n");
			if (dbg_level > 0) fflush(LOG);
			return 0;
		}

		if(dbg_level > 4) fprintf(LOG, "packed not already processed  \n");

		itsnet_processed_packet_list_add(&processed_packet_list,
		                                 p->payload.itsnet_unicast.source_position_vector.time_stamp,
		                                 p->payload.itsnet_unicast.source_position_vector.node_id);
		/* update/add S & F*/

		/*TODO Source & Forwarder*/


		if(!node_id_cmp(get_ego_node_id(),p->payload.itsnet_unicast.dest_node_id))

		{
			/* Select the destination of received packet : IP or Transport*/
			fprintf(LOG, "\tThe packet is destinated to me (nodeId = ");
			print_node_id(p->payload.itsnet_unicast.dest_node_id);
			fprintf(LOG, ")\n");

			if(p->common_header.protocol_info.itsnet_next_header == transport)
			{
				/*Send packet to upper layer */
				fprintf(LOG, "\tNext header is *Transport*\n");
				result=itsnet_unicast_indication_send(p);
			}
			if(p->common_header.protocol_info.itsnet_next_header == ip)
			{
				fprintf(LOG, "\tNext header is *IP*\n");
				itsnet_ip_receive(p);
			}
			if (dbg_level > 0) fflush(LOG);
			return(result);

		}

		fprintf(LOG, "The packet is not destinated to me \n");

		if(list_find_node(&neighbor_list,&(p->payload.itsnet_unicast.dest_node_id)) != NULL)
		{
			p->common_header.forwarder_position_vector=pos;
			dest_mac=(list_find_node(&neighbor_list,&(p->payload.itsnet_unicast.dest_node_id)))->mac_id.address;
			if (  itsnet_packet_send(p,dest_mac) == -1 )
			{
				fprintf(LOG, "erreur envoie packet \n");
				result = -1;
			}
			fprintf(LOG, " \n PACKET IS SENT TO OTHER NEIGHBORS BECAUSE DESTINATION NODE ID IS A NEIGHBOR\n");
			free(p);
			if (dbg_level > 0) fflush(LOG);
			return result;

		}
		
		if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0)
		{

			if(node_id_cmp(p->common_header.forwarder_position_vector.node_id,get_ego_node_id()))
			{
				print_node_id(get_ego_node_id());
				fprintf(LOG, "I'm not the selected forwarder,the forwarder is : \n");
				print_node_id(p->common_header.forwarder_position_vector.node_id);
				fprintf(LOG, " \n PACKET IS DROPPED \n");
				free(p);
				if (dbg_level > 0) fflush(LOG);
				return 0;
			}

			fprintf(LOG, "I'm the selected forwarder \n");
			p->common_header.forwarder_position_vector=pos;
			p->common_header.forwarder_position_vector.node_id=itsnet_select_forwarder(&neighbor_list,pos,p->payload.itsnet_unicast.dest_latitude,p->payload.itsnet_unicast.dest_longitude);
			
			dest_mac=(list_find_node(&neighbor_list,&(p->common_header.forwarder_position_vector.node_id)))->mac_id.address;
			
			if (  itsnet_packet_send(p,dest_mac) == -1 )
			{
				fprintf(stderr, "erreur envoie packet \n");
				result = -1;
			}
			free(p);
			if (dbg_level > 0) fflush(LOG);
			return result;

		}
		else
		{

			/*put P in the store and forward buffer*/
			fprintf(LOG, "put P in the store and forward buffer\n");
					itsnet_store(p,0);

		}
	}
	else
	{
		fprintf(LOG, " \n PACKET IS DROPPED \n");
		if (dbg_level > 0) fflush(LOG);
		free(p);
		return 0;

	}

}
