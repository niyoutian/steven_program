#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define UDP_SERVER_IP "172.100.10.165"
#define UDP_SERVER_PORT 30490
#define UDP_CLIENT_IP "172.100.10.167"
#define UDP_CLIENT_PORT 30490