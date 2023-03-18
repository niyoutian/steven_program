#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcp_func.h"

int test_client_case1(void)
{
    int socketfd = 0;
    int ret = 0;
    struct sockaddr_in peerAddr;

    memset(&peerAddr, 0, sizeof(struct sockaddr_in));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(13400);
    inet_pton(AF_INET, (const char *)"172.100.10.167", &(peerAddr.sin_addr));

    socketfd = create_clientSocket("172.100.10.165", 11111);
    if (socketfd < 0) {
        perror("Error: Failed to create socket\n");
        return -1;
    }

    ret = connect(socketfd,(struct sockaddr*)&peerAddr, sizeof(struct sockaddr_in));
    printf("connect ret %d, error: %d \n", ret, errno);
    if (ret < 0) {
        printf("Failed to connect\n");
        close(socketfd);
        return -1;
    }


    printf("connect socket is %d to address:%s \n",socketfd,"172.100.10.167");
    sleep(60);
    int error2 = 100;
    socklen_t len2 = sizeof (error2);
    int flag2 = getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &error2, &len2);
    printf("flag2 %d error2=%d\n",flag2,error2);
    close(socketfd);
#if 0
    ret = pthread_create(&g_package_pthread, NULL, (void *)handle_package, NULL);
    if (ret != 0) {
        printf("Failed to pthread_create %d\n",ret);
        close(socketfd);
        return -1;
    }

    FD_SET(socketfd, &g_fdset);
    g_max_sockfd = socketfd;
    g_listen_sd = socketfd;

    pthread_join(g_package_pthread, NULL);

    printf("exit \n");
#endif
    pause();
    return 0;
}

int main(int argc, char *const argv[])
{
    printf("enter tcp server\n");
    test_client_case1();

    return 0;
}