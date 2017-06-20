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
 * @file routing_manager.c
 * routing manager code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <unistd.h>
#include <errno.h>


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "routing_manager.h"
#include "itsnet.h"
#include "position_sensor.h"

struct timeval stv, etv;

itsnet_processed_packet* list_find_packet(
    struct list_head * processed_packet_list,
    itsnet_time_stamp ts,
    struct itsnet_node_id node_id)
{
	struct list_head* packet_list_entry;
	list_for_each(packet_list_entry, processed_packet_list)
	{
		itsnet_processed_packet* i;
		i = list_entry(packet_list_entry, struct itsnet_processed_packet, list);
		if ((memcmp(&node_id,&(i->source_node_id),sizeof(itsnet_node_id)) == 0)&&(ts==i->time_stamp))
		{
			fprintf(LOG, "\talready processed packet \n");
			if (dbg_level > 0)  fflush(LOG);
			return i;
		}
	}
	return NULL;

}

itsnet_routing_packet* list_find_routing_packet(
    struct list_head * routing_packet_list,
    itsnet_time_stamp ts,
    struct itsnet_node_id node_id)
{
	struct list_head* packet_list_entry;
	list_for_each(packet_list_entry, routing_packet_list)
	{
		itsnet_routing_packet* i;
		i = list_entry(packet_list_entry, struct itsnet_routing_packet, list);
		if ((memcmp(&node_id,&(i->source_node_id),sizeof(itsnet_node_id)) == 0)&&(ts==i->time_stamp))
		{
			fprintf(LOG, "\talready routing packet \n");
			if (dbg_level > 0)  fflush(LOG);
			return i;
		}
	}
	return NULL;

}

int itsnet_processed_packet_list_add(
    struct list_head* processed_packet_list,
    itsnet_time_stamp ts,
    struct itsnet_node_id node_id)
{

	itsnet_processed_packet * new_packet;
	struct timespec exp_in;
	tssetmsec(exp_in,20);  /** TODO */

	if(list_size(processed_packet_list) > MAX_PROCESSED_PACKET)
	{
		return 1;
	}
	else
	{
		if ( (new_packet = malloc(sizeof(struct itsnet_processed_packet))) == NULL)
		{
			return -1;
		}

		new_packet->time_stamp=ts;
		new_packet->source_node_id=node_id;
		new_packet->expires=exp_in;
		INIT_LIST_HEAD(&(new_packet->tqe.list));

		fprintf (LOG, "\titsnet_processed_packet_list_add (source node id=");
		print_node_id(new_packet->source_node_id);
		fprintf(LOG, ")  time_stamp=%d\n", new_packet->time_stamp);

		add_task_rel(&exp_in, &(new_packet->tqe), processed_packet_remove);

		list_add_tail(&new_packet->list, processed_packet_list);
		if (dbg_level > 0)  fflush(LOG);
		return 0;
	}
}

int itsnet_routing_packet_list_add(
    struct list_head* routing_packet_list,
    itsnet_time_stamp ts,
    struct itsnet_node_id node_id,
		struct itsnet_packet *packet,
		int intval)
{

	itsnet_routing_packet * new_packet;
	struct timespec exp_in;
	struct itsnet_position_vector pos;

	pos = get_position_vector();

	if(intval != -1)
	{
		tssetmsec(exp_in,intval);  /** TODO */

		if(list_size(routing_packet_list) > MAX_CONTENTION_PACKET - 1)
		{
			return 1;
		}
		else
		{
			if ( (new_packet = malloc(sizeof(struct itsnet_routing_packet))) == NULL)
			{
				return -1;
			}

			new_packet->time_stamp=ts;
			new_packet->source_node_id=node_id;
			new_packet->expires=exp_in;
			new_packet->pre_forward_node =	
					packet->common_header.forwarder_position_vector.node_id;
			new_packet->tlong = (etv.tv_sec-stv.tv_sec)*1000.0+(etv.tv_usec-stv.tv_usec)/1000.0+ intval;
			stv.tv_sec = stv.tv_usec = etv.tv_sec = etv.tv_usec = 0;

			INIT_LIST_HEAD(&(new_packet->tqe.list));
			memcpy(&(new_packet->packet), packet, sizeof(struct itsnet_packet));

			fprintf (LOG, "\titsnet_routing_packet_list_add (source node id=");
			print_node_id(new_packet->pre_forward_node);
			fprintf(LOG, ")  time_stamp=%d\n", new_packet->time_stamp);

			new_packet->packet.common_header.forwarder_position_vector=pos;	
   		add_task_rel(&exp_in, &(new_packet->tqe), routing_packet_broadcast);

			list_add_tail(&new_packet->list, routing_packet_list);
			if (dbg_level > 0)  fflush(LOG);
			return 0;
		}
	}
	
	else
	{
		if(list_size(routing_packet_list) > MAX_CONTENTION_PACKET - 1)
		{
			return 1;
		}
		else
		{
			if ( (new_packet = malloc(sizeof(struct itsnet_routing_packet))) == NULL)
			{
				return -1;
			}

			new_packet->time_stamp=ts;
			new_packet->source_node_id=node_id;
			new_packet->expires=exp_in;
			new_packet->pre_forward_node =	
					packet->common_header.forwarder_position_vector.node_id;
			new_packet->tlong = (etv.tv_sec-stv.tv_sec)*1000.0+(etv.tv_usec-stv.tv_usec)/1000.0;
			stv.tv_sec = stv.tv_usec = etv.tv_sec = etv.tv_usec = 0;
			INIT_LIST_HEAD(&(new_packet->tqe.list));
			memcpy(&(new_packet->packet), packet, sizeof(struct itsnet_packet));

			fprintf (LOG, "\titsnet_routing_packet_list_add (source node id=");
			print_node_id(new_packet->source_node_id);
			fprintf(LOG, ")  time_stamp=%d\n", new_packet->time_stamp);

			//add_task_rel(&exp_in, &(new_packet->tqe), routing_packet_broadcast);

			list_add_tail(&new_packet->list, routing_packet_list);
			if (dbg_level > 0)  fflush(LOG);
			return 0;
		}
	}
}

void itsnet_processed_packet_list_print_node(
    const itsnet_processed_packet * packet)
{
	fprintf(LOG, "source node id : ");
	print_node_id(packet->source_node_id);
	fprintf(LOG, "\ttime_stamp : %d \n",packet->time_stamp);
	if (dbg_level > 0)  fflush(LOG);
}

void itsnet_processed_packet_list_print(
    const struct list_head * processed_packet_list)
{
	struct list_head* packet_list_entry;
	list_for_each(packet_list_entry, processed_packet_list)
	{
		itsnet_processed_packet * i;
		i = list_entry(packet_list_entry, struct itsnet_processed_packet, list);
		itsnet_processed_packet_list_print_node(i);

	}
}


void itsnet_routing_packet_list_print_node(
    const itsnet_routing_packet * packet)
{
	
	fprintf(LOG, "source node id : ");
	print_node_id(packet->source_node_id);
	fprintf(LOG, "\ttime_stamp : %d \n",packet->time_stamp);
	fprintf(LOG, "\tforwarder : ");
	print_node_id(packet->pre_forward_node);
	fprintf(LOG, "\n");
  fprintf(LOG, "\ttlong : %.3f \n", packet->tlong);
	if (dbg_level > 0)  fflush(LOG);
}

void itsnet_routing_packet_list_print(
    const struct list_head * routing_packet_list)
{
	struct list_head* packet_list_entry;
	fprintf(LOG, "======================================= routing list ========================================\n");
	list_for_each(packet_list_entry, routing_packet_list)
	{
		itsnet_routing_packet * i;
		i = list_entry(packet_list_entry, struct itsnet_routing_packet, list);
		itsnet_routing_packet_list_print_node(i);

	}
	fprintf(LOG, "==============================================================================================\n");

}

void processed_packet_remove(
    struct tq_elem *tqe)
{

	itsnet_processed_packet * packet;
	packet=  tq_data(tqe,  itsnet_processed_packet, tqe);
	list_del(&packet->list);
	free(packet);

}

void routing_packet_remove(
		struct tq_elem *tqe)
{
	itsnet_routing_packet * routing_packet;
	routing_packet = tq_data(tqe, itsnet_routing_packet, tqe);

	list_del(&routing_packet->list);
	//free(&routing_packet->packet);
	free(routing_packet);

}

void routing_packet_broadcast(
		struct tq_elem *tqe)
{
	itsnet_routing_packet * routing_packet;
	itsnet_packet * p;
	struct timespec exp_in;

	tssetmsec(exp_in, 5000);	

	routing_packet = tq_data(tqe, itsnet_routing_packet, tqe);
	p = &(routing_packet->packet);	

	fprintf(LOG, "contention_packet_broadcast\n");
	if (itsnet_packet_send(p, get_broadcast_mac_id()) == -1)
	{
		fprintf(stderr, "erreur envoie packet\n");
	}

	//add_task_rel(&exp_in, &(routing_packet->tqe), routing_packet_remove);
  //gettimeofday(etv, NULL);

	fflush(LOG);
	
	//routing_packet_remove(tqe);
}
