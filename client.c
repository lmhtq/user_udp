#include "user_udp.c"

int main(int argc, char* argv[])
{
	int i = 0;
	float lossrate = 0;
	//printf ("%d\n", argc);
	if (argc < 2)
		lossrate = 0.1;
	else
	{
		sscanf(argv[1], "%f", &lossrate);
	}

	user_udp client_udp;
	client_udp.is_init = 0;
	user_udp_init(&client_udp);
	//while(1)
	for (i = 0; i < 1; i++)
	{		
		set_timestamp_sn(&client_udp, (uint64_t)(3*i),(uint64_t)(2*i),(uint16_t)i);
		calc_lossrate_delay(&client_udp);
		client_udp.lossrate = lossrate;
		send_to_server(&client_udp);
	}
	printf("%d\n", (int)lossrate /10 );
	user_udp_de_init(&client_udp);
	return 0;
}