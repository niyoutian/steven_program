#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>


typedef enum{
    CONN_SOCK_UDP,
    CONN_SOCK_TCP,
    CONN_CONSOLE	
}CONN_TYPE;
#define UDP_SERVER_IP "172.100.10.166"
#define UDP_SERVER_PORT 8888

void showUsage(void) 
{
    printf("Usage: \n");
    printf("Options: \n");
    printf(" -h, --help                            Print this help message and exit.\n");
    printf(" -t, --type=UDP/TCP/CONSOLE            set the channel type default UDP.\n");
    printf(" -i, --ip=172.100.10.166               set the ip address.\n");
    printf(" -p, --port=8888                       set the local port default 8888.\n");
}

int main(int argc, char* argv[])
{
    int channel_type = CONN_SOCK_UDP;
    struct in_addr local_addr;
    uint16_t local_port = UDP_SERVER_PORT;

    memset(&local_addr,0,sizeof(local_addr));

    char* const short_options = "ht:i:p:";
    struct option long_options[] = {
        { "help", no_argument, NULL, 'h' },
        { "type", required_argument, NULL, 't' },
        { "ip", required_argument, NULL, 'i' },
        { "port", required_argument, NULL, 'p' },
        { NULL, 0, NULL, 0},
    };

    if (argc < 2) {
        showUsage();
        return 0;
    }

    int c;
    while((c = getopt_long (argc, argv, short_options, long_options, NULL)) != -1) {
        switch (c) {
            case 'h':
                showUsage();
                return 0;
            case 't':
                if (strncmp(optarg,"UDP",3) == 0) {
                    channel_type = CONN_SOCK_UDP;
                } else if (strncmp(optarg,"TCP",3) == 0) {
                    channel_type = CONN_SOCK_TCP;
                } else if (strncmp(optarg,"CONSOLE",7) == 0) {
                    channel_type = CONN_CONSOLE;
                } else {
                    showUsage();
                    return 0;
                }
                break;
            case 'i':
                if (inet_pton(AF_INET, optarg, &local_addr) == 0) {
                    printf("invalid address %s\n",optarg);
                    showUsage();
                    return 0;
                }
                break;
            case 'p':
                local_port = (uint16_t)atoi(optarg);
                break;
            default:
                showUsage();
                return 0;
                
        }
    }

    printf("type:%d ip:%s port:%u\n",channel_type,inet_ntoa(local_addr), local_port);

    return 0;
}