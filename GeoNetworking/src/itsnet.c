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
 * @file itsnet.c
 * itsnet code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#else
#error "POSIX Thread Library required!"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include "tqueue.h"
#include "dll_sap.h"
#include "routing_manager.h"
#include "position_sensor.h"
#include "itsnet_listener.h"
#include "itsnet_ip.h"
#include "itsnet.h"
#include "location_table.h"
#include "position_sensor.h"
#include "itsnet_pseudonym.h"
#include "itsnet_parser.h"
#include "beaconing.h"
#include "auto_wifi.h"
#include "obd.h"
#include "upload.h"

LIST_HEAD(neighbor_list);

/**
 * clean memory
 * @return an integer
 */

int itsnet_cleanup()
{
	fclose(LOG);
	return 0;
}

/**
 * main function
 * @param argc
 * @param argv
 */

int itsnet_main (int argc, char **argv)
{

	dbg_level = DebugLevel_parse();
	pthread_t pthread_obd;	
	pthread_t pthread_upload;

	if ((LOG = fopen(ItsnetLog_parse(), "a")) == NULL) {
		fprintf(stderr, "file open error!!\n");
		exit(0);
	}

	if (taskqueue_init() < 0)
	{
		fprintf(stderr, "taskqueue_init failed\n");
		exit(1);
	}

	if (dll_sap_init(NULL) < 0)
	{
		fprintf(stderr, "dll_sap__init failed\n");
		exit(1);
	}

	if (itsnet_listener_init() < 0)
	{
		fprintf(stderr, "position_sensor_init failed\n");
		exit(1);
	}

	if (itsnet_iplistener_init() < 0)
	{
		fprintf(stderr,"itsnet_iplistener_init failed\n");
		exit(1);
	}

  	if (pthread_create(&pthread_obd, NULL, thread_obd, NULL) == -1){
		perror("Fail to create obd thread");
		return -1;
	}

	if (pthread_create(&pthread_upload, NULL, thread_upload, NULL) == -1){
		perror("Fail to create upload thread");
		return -1;
	}
	
	auto_wifi_init();	

	itsnet_init_conf();
	fprintf(LOG, "======= debug level is %d =======\n", dbg_level);

	if (dbg_level > 0) fflush(LOG);
	return 0;
}
/**
  *
  */

void itsnet_init_conf()
{
	set_ego_node_id(NodeId_parse());
	set_broadcast_mac_id(BroadcastMac_parse());
	itsnet_beacon_send();
}

