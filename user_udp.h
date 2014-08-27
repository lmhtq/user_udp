/*
 * 用于在解RTP包时处理SeqNum和Timestamp
 */

#define SERVERPORT 6789
#define LIVELEN 1000
#define BUFFLEN 32

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned long uint64_t;
typedef struct user_udp user_udp;

struct user_udp
{
    /*客户端参数*/
    uint64_t seqnum;
    uint64_t seqnum_array[LIVELEN];
    uint64_t remote_timestamp;
    uint64_t remote_timestamp_array[LIVELEN];
    uint64_t local_timestamp;
    uint64_t local_timestamp_array[LIVELEN];
    uint64_t cnt;
    float lossrate;
    float delay;
    char is_init;

    /*服务器端参数*/
    uint32_t bitrate;
    uint32_t last_bitrate;  
    uint32_t network_status;  
    
    /*Socket参数*/
    char buf[BUFFLEN];
    int socket_descriptor;
    struct sockaddr_in address;

};
