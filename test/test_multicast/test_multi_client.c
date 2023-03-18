#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "test_multi_func.h"

int test_client_case1(void)
{
    int socketfd = 0;
    int ret = 0;
    struct sockaddr_in peerAddr;
    struct ip_mreq mreq;


    socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketfd < 0) {
        printf("create socket failed\n");
        return -1;
    }

#if 1
	int flag = 1;
	ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
    if (ret < 0) {
        printf("Error: Failed to setsockopt socket\n");
        close(socketfd);
        return -1;
    }
#endif
#if 1
    memset(&mreq, 0, sizeof(struct ip_mreq));
    inet_pton(AF_INET, (const char *)"224.244.224.245", &(mreq.imr_multiaddr));
    inet_pton(AF_INET, (const char *)UDP_CLIENT_IP, &(mreq.imr_interface));
    ret = setsockopt(socketfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq));
    if (ret < 0) {
        printf("Error: Failed to setsockopt socket\n");
        close(socketfd);
        return -1;
    }

	struct sockaddr_in srcaddr;
	memset(&srcaddr, 0, sizeof(struct sockaddr_in));
	srcaddr.sin_family = AF_INET;
    inet_pton(AF_INET, (const char *)UDP_CLIENT_IP, &(srcaddr.sin_addr));
	srcaddr.sin_port = htons(UDP_CLIENT_PORT);
    ret = bind(socketfd, (struct sockaddr*)&srcaddr, sizeof(struct sockaddr_in));
    if (ret != 0) {
        printf("Fail to bind socket %d, ret %d, errno: %d\n", socketfd, ret, errno);
        close(socketfd);
        return -1;
    }
#endif
    // set up destination address
    struct sockaddr_in dst_addr;
    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
#if 0  //unicast
    inet_pton(AF_INET, (const char *)UDP_SERVER_IP, &(dst_addr.sin_addr));
    dst_addr.sin_port = htons(UDP_SERVER_PORT);
#else //multicast
    inet_pton(AF_INET, (const char *)"224.244.224.245", &(dst_addr.sin_addr));
    dst_addr.sin_port = htons(UDP_SERVER_PORT);
#endif

    int send_len  = 0;
    for (int i = 0; i < 5; i++) {
        char buffer[64];
        memset(buffer, '\0', sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "Hello, World! Sequence: %u", i & 0xFF);
        send_len = sendto(socketfd, buffer, strlen(buffer), 0, (struct sockaddr*) &dst_addr, sizeof(dst_addr));
        printf("%d sendto send_len=%d\n",i, send_len);
        sleep(1);
    }

    return 0;
}

int main(int argc, char *const argv[])
{
    printf("enter multicase client\n");
    test_client_case1();
    return 0;
}