#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tls_func.h"

fd_set g_fdset;
int g_max_sockfd;

int main(int argc, char *const argv[])
{
    struct sockaddr_in server_addr;

    printf("hello\n");

    initSslLibrary(0);
    printf("SSL_ERROR_NONE=%d\n",SSL_ERROR_NONE);
    printf("SSL_ERROR_ZERO_RETURN=%d\n",SSL_ERROR_ZERO_RETURN);
    printf("SSL_ERROR_WANT_READ=%d\n",SSL_ERROR_WANT_READ);
    printf("SSL_ERROR_WANT_WRITE=%d\n",SSL_ERROR_WANT_WRITE);
    printf("SSL_ERROR_WANT_CONNECT=%d\n",SSL_ERROR_WANT_CONNECT);
    printf("SSL_ERROR_WANT_ACCEPT=%d\n",SSL_ERROR_WANT_ACCEPT);
    printf("SSL_ERROR_WANT_X509_LOOKUP=%d\n",SSL_ERROR_WANT_X509_LOOKUP);
    printf("SSL_ERROR_SSL=%d\n",SSL_ERROR_SSL);
    /* 创建一个 socket 用于 tcp 通信 */
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {

        perror("Error: Failed to create socket\n");
        return -1;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    int ret = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("create socket failed\n");
        return -1;
    }

    /*把建立好的socket和SSL结构联系起来*/
    bindSslAndConnect(socket_fd);
    
    FD_SET(socket_fd, &g_fdset);
    g_max_sockfd = (socket_fd > g_max_sockfd) ? socket_fd : g_max_sockfd;

    SSL *p_ssl = NULL;
    p_ssl = getSslFd();
    printf("======send=====\n");

    char buf[100] = {0};
    strncpy(buf, "I am client", strlen("I am client"));
    ret = SSL_write(p_ssl, buf, strlen("I am client"));
    printf("%d bytes sent\n", ret);

    printf("======recv=====\n");
    memset(buf, 0, sizeof(buf));
    ret = SSL_read(p_ssl, buf, sizeof(buf));
    printf("%d bytes read\n", ret);

    if (ret > 0) {
        printf("%s\n", buf);
    } 

    return 0;
}