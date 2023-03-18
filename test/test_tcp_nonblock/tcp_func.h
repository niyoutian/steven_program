
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define TCP_SERVER_IP "172.100.10.167"
#define TCP_SERVER_PORT 13400
#define TCP_CLIENT_IP "172.100.10.165"
#define TCP_CLIENT_PORT 0

int create_clientSocket(char *local_ip, unsigned short local_port);
int create_serverSocket(char *local_ip, unsigned short local_port);