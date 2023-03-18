#include <stdio.h>
#include "tcp_func.h"

/*
struct sockaddr_in sa;
char str[INET_ADDRSTRLEN];

store this IP address in sa:
 int inet_pton(int af, const char *src,void *dst)
inet_pton(AF_INET, "192.0.2.33", &(sa.sin_addr));

now get it back and print it
inet_ntop(AF_INET, &(sa.sin_addr), str, INET_ADDRSTRLEN);
*/
int create_clientSocket(char *local_ip, unsigned short local_port)
{
	struct sockaddr_in localAddr;
    int sock_fd = -1;
    int value = 1;
    int ret = 0;

	memset(&localAddr, 0, sizeof(struct sockaddr_in));
	

    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(local_port);
    inet_pton(AF_INET, (const char *)local_ip, &(localAddr.sin_addr));

    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock_fd < 0) {
        return -1;
    }

    ret = bind(sock_fd, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (ret != 0) {
        printf("bind ret %d, error: %d \n", ret, errno);
        close(sock_fd);
        return -1;
    }

	struct sockaddr_in localInfo;
	socklen_t   sock_len = sizeof(localInfo);
    ret = getsockname(sock_fd, (struct sockaddr *)&localInfo, &sock_len);
	if (ret < 0) {
		printf("getsockname ret %d, error: %d \n", ret, errno);
        close(sock_fd);
		return -1;
	}

    return sock_fd;
}

int create_serverSocket(char *local_ip, unsigned short local_port)
{
    struct sockaddr_in peerAddr;
	struct sockaddr_in localAddr;
    int sock_fd = -1;
    int value = 1;
    int ret = 0;

	memset(&peerAddr, 0, sizeof(struct sockaddr_in));
	memset(&localAddr, 0, sizeof(struct sockaddr_in));
	

    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(local_port);
    inet_pton(AF_INET, (const char *)local_ip, &(localAddr.sin_addr));

    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock_fd < 0) {
        return -1;
    }

    ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&value, sizeof(value));
    if (ret < 0) {
        perror("Error: Failed to setsockopt socket\n");
        return -1;
    }

    ret = bind(sock_fd, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (ret != 0) {
        printf("Fail to bind socket %d, ret %d, errno: %d\n", sock_fd, ret, errno);
        close(sock_fd);
        return -1;
    }


	struct sockaddr_in localInfo;
	socklen_t   sock_len = sizeof(localInfo);
    ret = getsockname(sock_fd, (struct sockaddr *)&localInfo, &sock_len);
    if (ret != 0) {
        printf("Fail to getsockname socket %d, ret %d, errno: %d\n", sock_fd, ret, errno);
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}