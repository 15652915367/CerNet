#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <curl/curl.h>
#include "upload.h"

#include "itsnet_header.h"
#include "position_sensor.h"

#define UPLOADING_INTERVAL 1

#define MAX_BUF_SIZE 1000
#define ADDRESS_OF_SERVER   "http://[2001:da8:270:2018:f816:3eff:fe11:9211]/vanet/upload.php"

void static inline data2str(char* buff, itsnet_position_vector pos)
{
        sprintf(buff, "%d&%d&%d&%d&%d&%d", 1, pos.latitude, pos.longitude, pos.speed, pos.heading, pos.time_stamp);
}

void post2server(char *post_data)
{
        CURLcode  res;
        CURL *curl;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if(curl){
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
                curl_easy_setopt(curl, CURLOPT_URL, ADDRESS_OF_SERVER);
                curl_easy_setopt(curl, CURLOPT_POST, 1);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 1000L);
		curl_easy_setopt(curl, CURLOPT_ACCEPTTIMEOUT_MS, 5000L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 3000L);
                res = curl_easy_perform(curl);
                if(res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed:%s\n", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
        }
}

void uploading_data(itsnet_position_vector pos)
{
        char buff[MAX_BUF_SIZE];
        char *data_str = buff;
        data2str(data_str, pos);
        post2server(data_str);
        printf("\n--------------------------------------------------\n\n");
}

int sendto_remote(itsnet_position_vector pos){
	uploading_data(pos);
}

void* thread_upload(void* m){
	struct itsnet_position_vector pos;
	
	while (1) {
		pos=get_position_vector();
		sendto_remote(pos);
		sleep(1);
	}
}

/*void  main()
{
        //init();
        while(1){
                //get_gps();
                uploading_data();
        }
}*/

