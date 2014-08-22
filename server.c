#include "user_udp.c"

void fun(user_udp *server_udp)
{
	loop_thread(server_udp);
}

int main()
{
	int i = 0;
	user_udp server_udp;
	server_udp.is_init = 0;
	server_udp_init(&server_udp);
	//loop_thread(&server_udp);
	fun(&server_udp);
	/*for (i = 0; i < 10;)
	{
		
		recv_from_client(&server_udp);
		i++;
	}*/
	while(1);
	user_udp_de_init(&server_udp);
	return 0;
}