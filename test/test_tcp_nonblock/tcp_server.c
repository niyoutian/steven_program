#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcp_func.h"

pthread_t g_package_pthread;
fd_set g_fdset;
int g_max_sockfd;
int g_listen_sd;

/*
int send(SOCKET sock, const char *buf, int len, int flags);
sock 为要发送数据的套接字
buf 为要发送的数据的缓冲区地址
len 为要发送的数据的字节数
flags 为发送数据时的选项,最后的 flags 参数一般设置为 0 或 NULL

send函数会将缓冲区 buf 中的 len 个字节写入sock，成功则返回写入的字节数，失败则返回 -1
*/
int test_new_socket(int sockfd) {
    int len = 0;
    char buf[30]={0x02,0xFD,0x80,0x01,0x00,0x00,0x00,0x07,0x0E,0x80,0x10,0x12,0x22,0xF1,0x86};

    len = send(sockfd,buf,15,0);
    if (len < 0) {
        printf("test_new_socket socket %d send data faild %d, errno %d\n",sockfd, len, errno);
        FD_CLR(sockfd, &g_fdset);
        return -1;
    }
    printf("test_new_socket socket %d send data success %d\n",sockfd, len);

    return 0;
}

int test_send_data(int sockfd) {
    int len = 0;
    char buf[30]={0x02,0xFD,0x80,0x01,0x00,0x00,0x00,0x06,0x0E,0x80,0x10,0x12,0x10,0x02};

    len = send(sockfd,buf,14,0);
    if (len < 0) {
        printf("test_send_data socket %d send data faild %d, errno %d\n",sockfd, len, errno);
        FD_CLR(sockfd, &g_fdset);
        return -1;
    }
    printf("test_send_data socket %d send data success %d\n",sockfd, len);

    return 0;
}

int hancle_accept(void)
{
    struct sockaddr_in newAddr;
    memset(&newAddr, 0, sizeof(newAddr));
    int socklen = sizeof(newAddr);

    int sockNew = accept(g_listen_sd, (struct sockaddr *)&newAddr, &socklen);
    if (sockNew < 0) {
        printf("accept error\n");
        return -1;
    }

    char str[INET_ADDRSTRLEN];
    memset(str, 0, sizeof(str));

    inet_ntop(AF_INET, &(newAddr.sin_addr), str, INET_ADDRSTRLEN);

    printf("accept client connected, socketid = %d peeraddr:%s port:%d\n",sockNew,str,ntohs(newAddr.sin_port));

    

    FD_SET(sockNew, &g_fdset);
    g_max_sockfd = (sockNew > g_max_sockfd) ? sockNew : g_max_sockfd;

    sleep(2);
    test_new_socket(sockNew);

    return 0;
}

int handle_data(int sockfd)
{
    int received = 0;
    unsigned char buf[50] = {0};
    int i = 0;
    int send_len = 0;
    

    received = recv(sockfd, buf, sizeof(buf), 0);
    if (received == 0) { //对端关闭socket
        printf("peer close socket %d\n", sockfd);
        FD_CLR(sockfd, &g_fdset);
        close(sockfd);
    } else if (received < 0) {//socket error
        printf("socket %d error %d errno %d\n", sockfd, received, errno);
        FD_CLR(sockfd, &g_fdset);
        close(sockfd);
    } else {
        printf("socket %d receive %d message:\n", sockfd, received);
        for(i = 0; i < received; i++) {
            printf("0x%02x ",buf[i]);
        }
        printf("\n");
        if (buf[7] == 0x08) {
            test_send_data(sockfd);
        }
    }
    return 0;
}

void *handle_package(void *arg)
{
    fd_set read_set;
    fd_set exception_set;
    int ret = 0;
    int sock_index = 0;

    int socketFlag = fcntl(g_listen_sd, F_GETFL, 0);
    printf("fcntl %x %x\n",socketFlag, O_NONBLOCK);

    while (1) {
        FD_ZERO(&read_set);
        FD_ZERO(&exception_set);
        read_set = g_fdset;
        exception_set = g_fdset;
        printf("before select\n");
        ret = select(g_max_sockfd + 1, &read_set, NULL, &exception_set, NULL);
        printf("select ret =%d\n",ret);
        if (ret == 0) {
            printf("timeout\n");
            continue; 
        } else if (ret < 0) {
            printf("select error %d\n",ret);
            return NULL;
        }

        for (sock_index = 0; sock_index <= g_max_sockfd; sock_index++) {
            if (FD_ISSET(sock_index, &read_set)) {
                FD_CLR(sock_index, &read_set);
                if (sock_index == g_listen_sd) {
                    hancle_accept();
                } else {
                    handle_data(sock_index);
                }
            }
            if (FD_ISSET(sock_index, &exception_set)) {
                FD_CLR(sock_index, &exception_set);
                printf("socket %d exceptio\n",sock_index);
                close(sock_index);
                return((void *)0);
            }
        }
    }
	return((void *)0);
}

int test_server_case1(void)
{
    int socketfd = 0;
    int ret = 0;

    socketfd = create_serverSocket(TCP_SERVER_IP, TCP_SERVER_PORT);
    if (socketfd < 0) {
        perror("Error: Failed to create socket\n");
        return -1;
    }

    ret = listen(socketfd, 5);
    if (ret < 0) {
        perror("Error: Failed to listen socket\n");
        return -1;
    }

    printf("listen socket is %d address:%s port:%d\n",socketfd,TCP_SERVER_IP,TCP_SERVER_PORT);
    FD_SET(socketfd, &g_fdset);
    g_max_sockfd = socketfd;
    g_listen_sd = socketfd;

    ret = pthread_create(&g_package_pthread, NULL, (void *)handle_package, NULL);
    if (ret != 0) {
        printf("Failed to pthread_create %d\n",ret);
        close(socketfd);
        return -1;
    }



    pthread_join(g_package_pthread, NULL);

    printf("exit \n");

    return 0;
}

int main(int argc, char *const argv[])
{
    printf("enter tcp server\n");
    test_server_case1();

    return 0;
}