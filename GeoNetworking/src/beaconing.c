/***************************************************************************
 *				CarGeo6 Project		                   *
 * 	   C2CNet layer implementation conforming with GeoNet D2.2         *
 *									   *
 *   			Copyright(C)2010 ESPRIT-INRIA		 	   *
 * 	            							   *
 *                                              	                   *
 *   Authors:								   *
 *   Hichem BARGAOUI <barghich@gmail.com>                                  *
 *   Anouar CHEMEK <anouar.chemek@gmail.com>                               *
 *  							                   *
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
 * @file beaconing.c
 * beaconing code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#include "itsnet_event_service.h"
#include "beaconing.h"
#include "itsnet_security.h"
#include "location_table.h"
#include "position_sensor.h"
#include "itsnet_parser.h"
#include "tqueue.h"
#include "itsnet.h"
#include "routing_manager.h"

#include "itsnet_pseudonym.h"

static struct tq_elem tqe;


/**
   * itsnet_beacon_send
   * @param itsnet_beacon_send packet
   */

int itsnet_beacon_send()
{
	struct itsnet_packet *p;
	struct timespec exp_in;
	tssetmsec(exp_in,TIME_BEACON_INTERVAL);
	struct itsnet_position_vector pos;
	pos=get_position_vector();
	p=(itsnet_packet*)malloc(sizeof(itsnet_packet));
	memset(p,0,sizeof(itsnet_packet));
	if (p==NULL)
	{
		fprintf(stderr, "erreur allocation \n");
		return 0;
	}
	else
	{

		p->common_header.protocol_info.itsnet_header_type=itsnet_beacon_id;
		p->common_header.traffic_class=CLASS03;
		p->common_header.protocol_info.itsnet_header_subtype=ITSNET_BEACON;
		p->common_header.flags=OBU_SECURITY_ENABLED;
		p->common_header.payload_lenght= sizeof(struct itsnet_beacon_t) + sizeof(struct itsnet_common_header);
		p->common_header.hop_limit=1;
		p->common_header.forwarder_position_vector=pos;
		p->common_header.protocol_info.itsnet_next_header=transport;
		p->common_header.txpower=27;
		p->common_header.protocol_info.itsnet_version=1;
		p->common_header.forwarder_position_vector.node_id= get_ego_node_id();
		p->common_header.protocol_info.itsnet_next_header=other;

		p->payload.itsnet_beacon.source_position_vector=pos;
		p->payload.itsnet_beacon.source_position_vector.node_id= get_ego_node_id();

		if (  itsnet_packet_send(p,get_broadcast_mac_id()) == -1 )
		{
			fprintf(stderr,"erreur envoie packet \n");
		}

		fprintf(LOG ,"\tBEACON (itsnet_beacon_send)\n");

		free(p);

	}

	itsnet_routing_packet_list_print(&routing_packet_list);
	itsnet_neighbor_list_print(&neighbor_list);
	INIT_LIST_HEAD(&(tqe.list));
	add_task_rel(&exp_in, &tqe, itsnet_beacon_send);
	return 0;
}



/**
   * handle the received packet
   * @param itsnet_packet packet
   * @brief When a node receives a beacon, it performs the following processing steps:
   *If security check fails, the packet will be dropped.
   *If the beacon comes from a node that is not included in the location table, a new entry will
   *be created for this node, and location information from the common C2C NET header will
   *be written into the entry.
   *If the beacon comes from a node that is already included in the location table, the location
   *information will be updated with information from the common C2C NET header.
   */
int itsnet_beacon_handler (struct itsnet_packet *p,struct mac_addr mac_id)
{
	itsnet_node  node;
	itsnet_node  *n;
	itsnet_event e;
	struct timespec exp_in;
	tssetsec(exp_in, LOCATION_TABLE_ENTRY_EXP);
	struct timespec expire;

	clock_gettime(CLOCK_REALTIME, &expire);
	tsadd(expire, exp_in, expire);

	if(p==NULL)
			return 0;
	
	if ( check_security(p) == 1 )
	{
/*						
			if (	
      				//(mac_id.address[5] == 0xf0) ||
          			(mac_id.address[5] == 0xf3)
          			//(mac_id.address[5] == 0x73) ||
          			//(mac_id.address[5] == 0xf4) ||
         			//(mac_id.address[5] == 0xee) ||
				//(mac_id.address[5] == 0xed) ||
				//(mac_id.address[5] == 0xef) ||
				//(mac_id.address[5] == 0xec) ||
				//(mac_id.address[5] == 0xf2) ||
				//(mac_id.address[5] == 0x7d) 		
			)
			return 0;
*/			

		node.mac_id=mac_id;
		node.node_id=p->payload.itsnet_beacon.source_position_vector.node_id;
		node.expires=expire;
		n=list_find_node(&neighbor_list,&(p->payload.itsnet_beacon.source_position_vector.node_id));

		if (n==NULL)
		{

			e.type=itsnet_new_neighbor;
			e.event.new_neighbor=p->payload.itsnet_beacon.source_position_vector;
			itsnet_event_send(e);

			node.pos_vector=p->payload.itsnet_beacon.source_position_vector;
			itsnet_neighbor_list_add(&neighbor_list,&node);

		}
		else
		{
			n->pos_vector=p->payload.itsnet_beacon.source_position_vector;
			n->expires=expire;
			itsnet_neighbor_list_update_node(n);

		}

	}
return 0;
}
