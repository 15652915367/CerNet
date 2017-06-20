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
 * @file itsnet_listener.c
 * itsnet listener code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */


#include <stdint.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <unistd.h>
#include <errno.h>
#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#else
#error "POSIX Thread Library required!"
#endif

#include <asm/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


#include "itsnet_listener.h"
#include "position_sensor.h"
#include "itsnet_header.h"
#include "itsnet_types.h"
#include "itsnet_pseudonym.h"
#include "location_table.h"
#include "itsnet.h"


#define SOCK_PATH "/tmp/upper_layer_socket"
#define SOCK_ITSNET_PATH "/tmp/itsnet_socket"


static pthread_mutex_t mutex;
static pthread_t itsnet_listener;


/**
   * send_socket
   * @param message  *m
   * @return static int
   */
static int send_socket(message  *m)
{
	int s, t, len;
	int length;
	struct sockaddr_un rem;
	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		return 1;
	}
	fprintf(LOG, "Trying to connect...\n");

	rem.sun_family = AF_UNIX;
	strcpy(rem.sun_path,  SOCK_PATH);
	len = strlen(rem.sun_path) + sizeof(rem.sun_family);
	if (connect(s, (struct sockaddr *)&rem, len) == -1)
	{
		perror("connect error");
		return 2;
	}

	fprintf(LOG, "Connected.\n");

	length=sizeof(*m);
	write (s, &length, sizeof (length));
	write (s, m, length);
	close(s);
	if (dbg_level > 0)  fflush(LOG);
	return 0;
}


/**
   * itsnet_configure_node_id_confirm_send
   * @param struct message  *m
   * @return int
   */

int itsnet_configure_node_id_confirm_send(struct message *m)
{

	int result;
	m->msg_hdr.sid=itsnet_management_sap;
	m->msg_hdr.opcode=itsnet_confirm;
	m->msg_hdr.aid=itsnet_configure_node_id;
	result=send_socket(m);
	return result;
}


/**
   * itsnet_get_node_id_confirm_send
   * @param void
   * @return int
   */
int itsnet_get_node_id_confirm_send()
{

	int result;
	struct itsnet_node_id node_id;

	struct message *m;

	int res;

	m=(message*)malloc(sizeof(message));
	memset(m,0,sizeof(message));

	m->msg_hdr.sid=itsnet_management_sap;
	m->msg_hdr.opcode=itsnet_confirm;
	m->msg_hdr.aid=itsnet_get_node_id;
	m->payload.itsnet_get_node_id_conf.node_id= get_ego_node_id();

	res=send_socket(m);

	return res;

}

/**
   * itsnet_configure_node_id_request_receive
   * @param struct message *m
   * @return int
   */

int itsnet_configure_node_id_request_receive(struct message *m)
{

	int result;
	struct message *msg;
	struct itsnet_node_id new_id;
	struct itsnet_node_id old_id;

	old_id=get_ego_node_id();

	set_ego_node_id(&(m->payload.itsnet_configure_node_id_req.node_id));

	msg=(message*)malloc(sizeof(message));
	memset(msg,0,sizeof(message));
	msg->payload.itsnet_configure_node_id_conf.old_id=old_id;
	msg->payload.itsnet_configure_node_id_conf.new_id=get_ego_node_id();
	msg->payload.itsnet_configure_node_id_conf.result=1;
	//fprintf(LOG, "NODEID:%llx\n",get_ego_node_id());
	result=itsnet_configure_node_id_confirm_send(msg);

	return result;
}


/**
   * itsnet_get_node_id_request_receive
   * @param struct message *m
   * @return int
   */

int itsnet_get_node_id_request_receive(struct message *m)
{
	int result;
	result=itsnet_get_node_id_confirm_send(m);
	return result;
}

/**
   * itsnet_get_neighbor_list_confirm_send
   * @param struct message *m
   * @return int
   */
int itsnet_get_neighbor_list_confirm_send()
{

	struct message *m;
	int result,neighbor_list_size,j;
	j=0;
	neighbor_list_size=0;
	const itsnet_node * node;
	itsnet_position_vector  neighbor_list_pos[MAX_NEIGHBOR_LIST];
	struct list_head* node_list_entry;

	m=(message*)malloc(sizeof(message));
	memset(m,0,sizeof(message));
	m->msg_hdr.sid=itsnet_management_sap;
	m->msg_hdr.opcode=itsnet_confirm;
	m->msg_hdr.aid=itsnet_get_neighbor_list;


	list_for_each(node_list_entry,&neighbor_list)
	{
		itsnet_node* i;
		i = list_entry(node_list_entry, struct itsnet_node, list);
		neighbor_list_pos[j]=i->pos_vector;
		neighbor_list_size +=1;
		j+=1;

	}


	m->payload.itsnet_get_neighbor_list_conf.neighbor_list_size=neighbor_list_size;

	for (j=0; j<neighbor_list_size; j++)
	{

		m->payload.itsnet_get_neighbor_list_conf.neighbor_list_pos[j]=neighbor_list_pos[j];
	}

	result=send_socket(m);

	return result;
}


/**
   * itsnet_get_neighbor_list_request_receive
   * @param struct message *m
   * @return int
   */
int itsnet_get_neighbor_list_request_receive(struct message *m)
{
	int result;
	result=itsnet_get_neighbor_list_confirm_send(m);
	return result;
}

/**
   * itsnet_get_position_vector_confirm_send
   * @param struct message *m
   * @return int
   */

int itsnet_get_position_vector_confirm_send(struct message *m)
{


	struct itsnet_position_vector pos;
	int res;
	pos= get_position_vector();

	m->msg_hdr.sid=itsnet_management_sap;
	m->msg_hdr.opcode=itsnet_confirm;
	m->msg_hdr.aid=itsnet_get_position_vector;
	m->payload.itsnet_get_position_vector_conf.position_vector=pos;
	res=send_socket(m);
	return res;
}


/**
   * itsnet_get_position_vector_request_receive
   * @param struct message *m
   * @return int
   */

int itsnet_get_position_vector_request_receive(struct message *m)
{

	int result;
	result=itsnet_get_position_vector_confirm_send(m);
	return result;
}


/**
   * itsnet_position_sensor_confirm_send
   * @param struct message *m
   * @return int
   */

int itsnet_position_sensor_confirm_send(struct message *m)
{

	return 0;
}


/**
   * itsnet_position_sensor_request_receive
   * @param struct message *m
   * @return int
   */
int itsnet_position_sensor_request_receive(struct message *m)
{
	pthread_mutex_lock(&mutex);

	position_data_update(m->payload.itsnet_position_sensor_req.position_information);

	pthread_mutex_unlock(&mutex);

	fflush(LOG);
	return 0;
}

/**
   * itsnet_event_indication_send
   * @param struct message *m
   * @return int
   */

int itsnet_event_indication_send(struct message *m)
{
	int res;
	res=send_socket(m);
	return res;

}

/**
   * itsnet_unicast_confirm_send
   * @param message
   * @return int
   */

int itsnet_unicast_confirm_send(int res)
{


	struct message *m;

	m=(message*)malloc(sizeof(message));

	memset(m,0,sizeof(message));

	m->msg_hdr.sid=itsnet_transport_sap;
	m->msg_hdr.opcode=itsnet_confirm;
	m->msg_hdr.aid=itsnet_unicast;

	m->payload.itsnet_unicast_conf.result=res;

	res=send_socket(m);
	return res;
}

/**
   * itsnet_geoanycast_confirm_send
   * @param message
   * @return int
   */

int itsnet_geoanycast_confirm_send(int res)
{


	struct message *m;

	m=(message*)malloc(sizeof(message));

	memset(m,0,sizeof(message));


	m->msg_hdr.sid=itsnet_transport_sap;
	m->msg_hdr.opcode=itsnet_confirm;
	m->msg_hdr.aid=itsnet_geoanycast;

	m->payload.itsnet_geoanycast_conf.result=res;

	res=send_socket(m);
	return res;
}

/**
   * itsnet_geobroadcast_confirm_send
   * @param message
   * @return int
   */
int itsnet_geobroadcast_confirm_send(int res)
{


	struct message *m;

	m=(message*)malloc(sizeof(message));

	memset(m,0,sizeof(message));

	m->msg_hdr.sid=itsnet_transport_sap;
	m->msg_hdr.opcode=itsnet_confirm;
	m->msg_hdr.aid=itsnet_geobroadcast;

	m->payload.itsnet_geobroadcast_conf.result=res;

	res=send_socket(m);
	return res;
}

/**
   * itsnet_geobroadcast_confirm_send
   * @param message
   * @return int
   */
int itsnet_geotopo_confirm_send(int res)
{

	struct message *m;

	m=(message*)malloc(sizeof(message));

	memset(m,0,sizeof(message));

	m->msg_hdr.sid=itsnet_transport_sap;
	m->msg_hdr.opcode=itsnet_confirm;
	m->msg_hdr.aid=itsnet_geotopo;

	m->payload.itsnet_geotopo_conf.result=res;

	res=send_socket(m);
	return res;
}


/**
   * itsnet_geounicast_request_receive
   * @param struct message *m
   * @return int
   */

int itsnet_geounicast_request_receive(struct message *m)
{
	int result;

	struct itsnet_packet *p;

	struct itsnet_position_vector pos;

	pos=get_position_vector();


	p=(itsnet_packet*)malloc(sizeof(itsnet_packet));
	itsnet_node_id nodeId;


	if (p==NULL)
	{
		fprintf(stderr, "erreur allocation \n");
		result = -1;
	}
	else
	{

		/*See Section 9 : C2CNet packets format*/
		memset(p,0,sizeof(itsnet_packet));
		p->common_header.protocol_info.itsnet_header_type=itsnet_unicast_id;
		p->common_header.traffic_class=CLASS01;
		p->common_header.protocol_info.itsnet_header_subtype=0;
		p->common_header.flags=OBU_SECURITY_ENABLED;
		p->common_header.payload_lenght=ITSNET_DATA_SIZE;
		p->common_header.hop_limit=255;
		p->common_header.forwarder_position_vector=pos;
		p->common_header.protocol_info.itsnet_next_header=transport;

		p->payload.itsnet_unicast.source_position_vector=pos;
		p->payload.itsnet_unicast.source_position_vector.node_id=get_ego_node_id();
		/*get information from application ( Data and node destination, "see message structure"  )*/
		p->payload.itsnet_unicast.dest_latitude=m->payload.itsnet_unicast_req.destination_latitude;
		p->payload.itsnet_unicast.dest_longitude=m->payload.itsnet_unicast_req.destination_longitude;
		p->payload.itsnet_unicast.dest_node_id=m->payload.itsnet_unicast_req.destination_node_id;
		memcpy(p->payload.itsnet_unicast.payload,m->payload.itsnet_unicast_req.data,ITSNET_DATA_SIZE);

		result=itsnet_geounicast_send(p);

	}
//	result=itsnet_geobroadcast_confirm_send(result);
	return result;
}

/**
   * itsnet_geoanycast_request_receive
   * @param struct message *m
   * @return int
   */

int itsnet_geoanycast_request_receive(struct message *m)
{

	int result;
	struct itsnet_packet *p;
	struct itsnet_position_vector pos;
	pos=get_position_vector();
	p=(itsnet_packet*)malloc(sizeof(itsnet_packet));
	itsnet_node_id nodeId;

	if (p==NULL)
	{
		fprintf(stderr,"erreur allocation \n");
		result = -1;
	}
	else
	{

		p->common_header.protocol_info.itsnet_header_type=itsnet_geoanycast_id;
		p->common_header.traffic_class=CLASS01;
		p->common_header.protocol_info.itsnet_header_subtype=m->payload.itsnet_geoanycast_req.geo_area.geo_area_type;
		p->common_header.flags=OBU_SECURITY_ENABLED;
		p->common_header.payload_lenght=ITSNET_DATA_SIZE;
		p->common_header.hop_limit=255;
		p->common_header.forwarder_position_vector=pos;
		p->common_header.forwarder_position_vector.node_id=get_ego_node_id();
		p->common_header.protocol_info.itsnet_next_header=transport;

		p->payload.itsnet_geoanycast.source_position_vector=pos;
		p->payload.itsnet_geoanycast.source_position_vector.node_id=get_ego_node_id();
		p->payload.itsnet_geoanycast.dest_latitude=m->payload.itsnet_geoanycast_req.geo_area.latitude;
		p->payload.itsnet_geoanycast.dest_longitude=m->payload.itsnet_geoanycast_req.geo_area.longitude;
		p->payload.itsnet_geoanycast.geo_area_size=m->payload.itsnet_geoanycast_req.geo_area.geo_area_size;
		memcpy(p->payload.itsnet_geoanycast.payload,m->payload.itsnet_geoanycast_req.data,ITSNET_DATA_SIZE);

		result=itsnet_geoanycast_send(p);

	}
//	result=itsnet_geobroadcast_confirm_send(result);
	return result;
}

/**
   * itsnet_geobroadcast_request_receive
   * @param struct message *m
   * @return int
   */

int itsnet_geobroadcast_request_receive(struct message *m)
{

	int result;
	struct itsnet_packet *p;
	struct itsnet_position_vector pos;
	pos=get_position_vector();
	p=(itsnet_packet*)malloc(sizeof(itsnet_packet));
	struct itsnet_node_id nodeId;

	if (p==NULL)
	{
		fprintf(stderr,"erreur allocation \n");
		result = -1;
	}
	else
	{

		memset(p,0,sizeof(itsnet_packet));
		p->common_header.protocol_info.itsnet_header_type=itsnet_geobroadcast_id;
		p->common_header.traffic_class=CLASS01;
		p->common_header.protocol_info.itsnet_header_subtype=m->payload.itsnet_geobroadcast_req.geo_area.geo_area_type;
		p->common_header.flags=OBU_SECURITY_ENABLED;
		p->common_header.payload_lenght=ITSNET_DATA_SIZE;
		p->common_header.hop_limit=255;
		p->common_header.forwarder_position_vector=pos;
		p->common_header.forwarder_position_vector.node_id=get_ego_node_id();
		p->common_header.protocol_info.itsnet_next_header=transport;

		p->payload.itsnet_geobroadcast.source_position_vector=pos;
		p->payload.itsnet_geobroadcast.dest_latitude=m->payload.itsnet_geobroadcast_req.geo_area.latitude;
		p->payload.itsnet_geobroadcast.dest_longitude=m->payload.itsnet_geobroadcast_req.geo_area.longitude;
		p->payload.itsnet_geobroadcast.geo_area_size=m->payload.itsnet_geobroadcast_req.geo_area.geo_area_size;

		/*memcpy(p->payload.itsnet_geobroadcast.payload,m->payload.itsnet_geobroadcast_req.data,1482);*/
		memcpy(p->payload.itsnet_geobroadcast.payload,m->payload.itsnet_geobroadcast_req.data,ITSNET_DATA_SIZE);


		result=itsnet_geobroadcast_send(p);

	}
//	result=itsnet_geobroadcast_confirm_send(result);
	return result;
}



/**
   * itsnet_geobroadcast_request_receive
   * @param struct message *m
   * @return int
   */

int itsnet_geotopo_request_receive(struct message *m)
{

	int result;
	struct itsnet_packet *p;
	struct itsnet_position_vector pos;
	pos=get_position_vector();
	p=(itsnet_packet*)malloc(sizeof(itsnet_packet));
	struct itsnet_node_id nodeId;

	if (p==NULL)
	{
		fprintf(stderr,"erreur allocation \n");
		result = -1;
	}
	else
	{

		memset(p,0,sizeof(itsnet_packet));
		p->common_header.protocol_info.itsnet_header_type=itsnet_tsb_id;
		p->common_header.traffic_class=CLASS01;
		p->common_header.protocol_info.itsnet_header_subtype=0;
		p->common_header.flags=OBU_SECURITY_ENABLED;
		p->common_header.payload_lenght=ITSNET_DATA_SIZE;
		p->common_header.hop_limit=m->payload.itsnet_geotopo_req.hop_limit;
		p->common_header.forwarder_position_vector=pos;
		p->common_header.forwarder_position_vector.node_id=get_ego_node_id();
		p->common_header.protocol_info.itsnet_next_header=transport;

		p->payload.itsnet_tsb.source_position_vector=pos;

		memcpy(p->payload.itsnet_tsb.payload,m->payload.itsnet_geotopo_req.data,ITSNET_DATA_SIZE);

		result=itsnet_geotopo_send(p);

	}
//	result=itsnet_geotopobroadcast_confirm_send(result);
	return result;
}

/**
   * itsnet_unicast_indication_send
   * @param struct itsnet_packet *p
   * @return int
   */
int itsnet_unicast_indication_send(struct itsnet_packet *p)
{
	int res;
	struct message *m;

	m=(message*)malloc(sizeof(message));

	memset(m,0,sizeof(message));

	m->msg_hdr.sid=itsnet_transport_sap;
	m->msg_hdr.aid=itsnet_unicast;
	m->msg_hdr.opcode=itsnet_indication;


	memcpy(m->payload.itsnet_unicast_ind.data,p->payload.itsnet_unicast.payload,MES_PAYLOAD_LEN);
	m->payload.itsnet_unicast_ind.source_node_id=p->payload.itsnet_unicast.source_position_vector.node_id;
	res=send_socket(m);
	return res;
}


/**
   * itsnet_geoanycast_indication_send
   * @param struct itsnet_packet *p
   * @return int
   */

int itsnet_geoanycast_indication_send(struct itsnet_packet *p)
{
	int res;
	struct message *m;

	m=(message*)malloc(sizeof(message));

	memset(m,0,sizeof(message));

	m->msg_hdr.sid=itsnet_transport_sap;
	m->msg_hdr.aid=itsnet_geoanycast;
	m->msg_hdr.opcode=itsnet_indication;


	memcpy(m->payload.itsnet_geoanycast_ind.data,p->payload.itsnet_geoanycast.payload,MES_PAYLOAD_LEN);
	m->payload.itsnet_geoanycast_ind.source_node_id=p->payload.itsnet_geoanycast.source_position_vector.node_id;
	res=send_socket(m);
	return res;
}


/**
   * itsnet_geobroadcast_indication_send
   * @param struct itsnet_packet *p
   * @return int
   */
int itsnet_geobroadcast_indication_send(struct itsnet_packet *p)
{
	int res;
	struct message *m;

	m=(message*)malloc(sizeof(message));

	memset(m,0,sizeof(message));

	m->msg_hdr.sid=itsnet_transport_sap;
	m->msg_hdr.aid=itsnet_geobroadcast;
	m->msg_hdr.opcode=itsnet_indication;

	memcpy(m->payload.itsnet_geobroadcast_ind.data,p->payload.itsnet_geobroadcast.payload,MES_PAYLOAD_LEN);
	m->payload.itsnet_geobroadcast_ind.source_node_pos.node_id=p->payload.itsnet_geobroadcast.source_position_vector.node_id;
	m->payload.itsnet_geobroadcast_ind.forwarder_node_pos.node_id=p->common_header.forwarder_position_vector.node_id;
	res=send_socket(m);
	return res;

}

int itsnet_geotopo_indication_send(struct itsnet_packet *p)
{
	int res;
	struct message *m;

	m=(message*)malloc(sizeof(message));

	memset(m,0,sizeof(message));

	m->msg_hdr.sid=itsnet_transport_sap;
	m->msg_hdr.aid=itsnet_geotopo;
	m->msg_hdr.opcode=itsnet_indication;

	memcpy(m->payload.itsnet_geotopo_ind.data,p->payload.itsnet_tsb.payload,MES_PAYLOAD_LEN);

	m->payload.itsnet_geotopo_ind.source_node_pos.node_id=p->payload.itsnet_tsb.source_position_vector.node_id;
	m->payload.itsnet_geotopo_ind.forwarder_node_pos.node_id=p->common_header.forwarder_position_vector.node_id;
	res=send_socket(m);
	return res;

}

/**
   * itsnet_get_security_param_request_receive
   * @param struct message *m
   * @return int
   */

int itsnet_get_security_param_request_receive(struct message *m)
{

	/** TODO*/
	return 0;
}

/**
   * itsnet_configure_security_param_request_receive
   * @param struct message *m
   * @return int
   */

int itsnet_configure_security_param_request_receive(struct message *m)
{

	/** TODO*/
	return 0;
}


/**
   * socket_recv
   * @param struct message *m
   * @return void
   */
static void socket_recv(struct message *m)
{

	int result;
	result = 1;
	switch (m->msg_hdr.aid)
	{
	case itsnet_configure_node_id:
		fprintf(LOG, "configure node\n");
		result=itsnet_configure_node_id_request_receive(m);
		break;
	case itsnet_get_node_id:
		result=itsnet_get_node_id_request_receive(m);
		fprintf(LOG, "itsnet_get_node_id  \n");
		break;
	case itsnet_get_neighbor_list:
		result=itsnet_get_neighbor_list_request_receive(m);
		fprintf(LOG, "itsnet_get_neighbor_list \n");
		break;
	case itsnet_get_position_vector :
		result=itsnet_get_position_vector_request_receive(m);
		fprintf(LOG, "itsnet_get_position_vector \n");
		break;
	case itsnet_position_sensor :
		result=itsnet_position_sensor_request_receive(m);
		break;
	case itsnet_unicast :
		result=itsnet_geounicast_request_receive(m);
		fprintf(LOG, "itsnet_unicast \n");
		break;
	case itsnet_geoanycast :
		fprintf(LOG, "itsnet_geoanycast \n");
		result=itsnet_geoanycast_request_receive(m);
		break;
	case itsnet_geobroadcast :
		fprintf(LOG, "itsnet_geobroadcast \n");
		result=itsnet_geobroadcast_request_receive(m);
		break;
	case itsnet_geotopo :
		fprintf(LOG, "itsnet_geotopo \n");
		result=itsnet_geotopo_request_receive(m);
		break;
	case itsnet_get_security_param :
		result=itsnet_get_security_param_request_receive(m);
		fprintf(LOG, "itsnet_get_security_param \n");
		break;
	case itsnet_configure_security_param :
		result=itsnet_configure_security_param_request_receive(m);
		fprintf(LOG, "itsnet_configure_security_param \n");
		break;
	default:
		fprintf(LOG, "unknown type\n");
		break;
	}
	if (dbg_level > 0)  fflush(LOG);
	free (m);
}



/**
   * itsnet listener init
   * @param void
   * @return int
   */
int itsnet_listener_init(void)
{
	pthread_mutexattr_t mattrs;
	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs,PTHREAD_MUTEX_TIMED_NP);

	if (pthread_mutex_init(&mutex, &mattrs) ||
	        pthread_create(&itsnet_listener, NULL, listener, NULL))
		return -1;
	return 0;
}



/**
   * server
   * @param int client_socket
   * @return int
   */

int server (int client_socket)
{
	while (1)
	{
		int length;
		/*		char * buffer;*/
		struct message* msg;
		if (read (client_socket, &length, sizeof (length)) == 0)
			return 0;
		msg = (struct message*) malloc (length);
		memset(msg,0,sizeof(message));
		read (client_socket, msg, length);
		socket_recv(msg);
	}
}


/**
   * listener
   * @param void
   * @return void
   */
static void *listener(void *arg)
{
	int s, s2, t, len;
	struct sockaddr_un local, remote;
	int client_sent_quit_message;
	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path,SOCK_ITSNET_PATH);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(s, (struct sockaddr *)&local, len) == -1)
	{
		perror("bind");
		exit(1);
	}

	if (listen(s, 5) == -1)
	{
		perror("listen");
		exit(1);
	}

	while (1)
	{
		int done, n;
		if(dbg_level > 4) fprintf(LOG, "Waiting for a connection...\n");
		t = sizeof(remote);

		if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1)
		{
			perror("accept");
			fprintf(stderr, "error\n");
		}
		if(dbg_level > 4) fprintf(LOG, "Connected.1\n");
		server (s2);
		close(s2);
		if (dbg_level > 0)  fflush(LOG);
	}
	close (s);
	unlink (SOCK_ITSNET_PATH);
	if (dbg_level > 0)  fflush(LOG);

}



/**
   *  itsnet listener cleanup
   * @param void
   * @return void
   */
void itsnet_listener_cleanup(void)
{

}
