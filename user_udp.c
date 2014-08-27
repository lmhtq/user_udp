/*
 * 实现部分
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include "user_udp.h"

/*insert sort*/
void ins_sort(uint64_t arr[], uint32_t len)
{
	uint32_t i,j;
	uint64_t tmp;
	for (i = 1; i < len; i++)
	{
		tmp = arr[i];
		for (j = i - 1;j >= 0 && arr[j] > tmp; j--)
			arr[j+1] = arr[j];
		arr[j+1] = tmp;
	}
}

/*记录远端和本地时间戳和序号*/
void set_timestamp_sn(user_udp *usr_udp, uint64_t remote, uint64_t local, uint16_t sn)
{
	uint32_t i;
   	usr_udp->remote_timestamp = remote;
    	usr_udp->local_timestamp = local;
    	usr_udp->seqnum = (uint64_t)sn;
    	if (usr_udp->cnt < LIVELEN)// its maximum is LIVELEN
    	{
    		usr_udp->remote_timestamp_array[usr_udp->cnt] = usr_udp->remote_timestamp;
    		usr_udp->local_timestamp_array[usr_udp->cnt] = usr_udp->local_timestamp;
    		usr_udp->seqnum_array[usr_udp->cnt] = usr_udp->seqnum;
    		usr_udp->cnt++;
    	}
    	else
    	{
    		for (i = 1; i < LIVELEN; i++)
    			usr_udp->remote_timestamp_array[i - 1] = usr_udp->remote_timestamp_array[i];
    		usr_udp->remote_timestamp_array[LIVELEN - 1] = usr_udp->remote_timestamp;

    		for (i = 1; i < LIVELEN; i++)
    			usr_udp->local_timestamp_array[i - 1] = usr_udp->local_timestamp_array[i];
    		usr_udp->local_timestamp_array[LIVELEN - 1] = usr_udp->local_timestamp;

    		for (i = 1; i < LIVELEN; i++)
    			usr_udp->seqnum_array[i - 1] = usr_udp->seqnum_array[i];
    		usr_udp->seqnum_array[LIVELEN - 1] = usr_udp->seqnum;
    	}
    	ins_sort(usr_udp->remote_timestamp_array, usr_udp->cnt);
    	ins_sort(usr_udp->local_timestamp_array, usr_udp->cnt);
    	ins_sort(usr_udp->seqnum_array, usr_udp->cnt);
}

/*初始化Socket Client*/
void user_udp_init(user_udp *usr_udp)
{
	if (usr_udp->is_init == 1)
		return;
    	usr_udp->cnt = 0;
    
    bzero(&usr_udp->address, sizeof(usr_udp->address));
    usr_udp->address.sin_family = AF_INET;
    usr_udp->address.sin_addr.s_addr = inet_addr("192.168.1.111");
    usr_udp->address.sin_port = htons(SERVERPORT);

    usr_udp->socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);

    usr_udp->is_init = 1;
    usr_udp->bitrate = 1200;
    usr_udp->last_bitrate = usr_udp->bitrate;
    memset(usr_udp->buf, 0, sizeof(BUFFLEN));
}

/*初始化Socket Server*/
void server_udp_init(user_udp *server_udp)
{
	if (server_udp->is_init == 1)
		return;

    bzero(&server_udp->address, sizeof(server_udp->address));
    server_udp->address.sin_family = AF_INET;
    server_udp->address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_udp->address.sin_port = htons(SERVERPORT);

    server_udp->socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    bind(server_udp->socket_descriptor, (struct sockaddr *)&server_udp->address,
    		sizeof(server_udp->address) );

    server_udp->is_init = 1;
    server_udp->bitrate = 50;
    server_udp->last_bitrate = server_udp->bitrate;
    server_udp->network_status = 0;
    memset(server_udp->buf, 0, sizeof(BUFFLEN));
    
}


/*销毁Socket Client/Server */
void user_udp_de_init(user_udp *usr_udp)
{
    usr_udp->cnt = 0;
    close(usr_udp->socket_descriptor);
    usr_udp->is_init = 0;
}

/*记录序号
void set_seqnum(user_udp *usr_udp, uint64_t sn)
{
    usr_udp->seqnum = sn;
}
*/

/*计算丢包率和时延*/
void calc_lossrate_delay(user_udp *usr_udp)
{
	uint64_t interval;
	if (usr_udp->cnt == LIVELEN)
    {
    		/*
    		 * calc lossrate
    		 */
    		interval = usr_udp->seqnum_array[LIVELEN-1] - usr_udp->seqnum_array[0];
    		usr_udp->lossrate = 100.0*(1 - (float)LIVELEN / (float)interval);

    		/*
    		 * calc delay
    		 */
    		usr_udp->delay = 0;
    }
}

/*计算时延
void calc_delay(user_udp *usr_udp)
{
    ;
}
*/

/*计算比特率*/
void calc_bitrate(user_udp *server_udp)
{
	static uint32_t cnt = 0;
	if (server_udp->lossrate < 1)
	{
		cnt++;
	}
	else
	{
		cnt = 0;
		if (server_udp->bitrate >= 100)
			server_udp->bitrate -= 50;
		
	}
	if (cnt >= 5)
	{
		server_udp->bitrate += 50;
		cnt = 0;
	}
	/*
	uint32_t bit_array[5] = {2000,1500,1000,500,100};
	uint32_t ind = (uint32_t)server_udp->lossrate / 10;
	if (ind >= 5)
		ind = 4;
	server_udp->bitrate = bit_array[ind];
	*/
}

/*向服务器反馈网络状况*/
void send_to_server(user_udp *usr_udp)
{
	uint32_t bit_array[5] = {2000,1500,1000,500,100};
	
	sprintf(usr_udp->buf, "%f %f\0", usr_udp->lossrate, usr_udp->delay);
    sendto(usr_udp->socket_descriptor, usr_udp->buf, 
           sizeof(usr_udp->buf), 0, 
           (struct sockaddr *)&usr_udp->address, sizeof(usr_udp->address));
}


/*output recv*/
void recv_from_client(user_udp *server_udp)
{
	float delay, lossrate;
	uint32_t len = sizeof(server_udp->address);
	recvfrom(server_udp->socket_descriptor, server_udp->buf, 
		sizeof(server_udp->buf), 0, (struct sockaddr *)&server_udp->address,&len);
	sscanf(server_udp->buf,"%f %f", &lossrate, &delay);
	server_udp->lossrate = lossrate;
	server_udp->delay = delay;

	calc_bitrate(server_udp);
	printf("Receive latest lossrate:%f\%\n", server_udp->lossrate);
}

void recv_thread(user_udp *server_udp)
{
		//pthread_detach(pthread_self());
	while (1)
	{
		printf("udp receiver thread id:%lu\n", pthread_self());
	
		recv_from_client(server_udp);
		//printf("ssssssssssss\n");
	
	}
}

void loop_thread(user_udp *server_udp)
{
	pthread_t id;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&id, &attr, (void *)recv_thread, server_udp );
	//void *status;
	//pthread_create(&id, NULL, (void *)recv_from_client, server_udp );
	//pthread_join(id, &status);
	//pthread_detach(id);

}
