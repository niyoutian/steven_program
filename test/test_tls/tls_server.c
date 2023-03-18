#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tls_func.h"

pthread_t g_package_pthread;
fd_set g_fdset;
int g_max_sockfd;
int g_listen_sd;


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

    printf("accept client connected, socketid = %d\n",sockNew);

    /*把建立好的socket和SSL结构联系起来*/
    bindSslAndAccept(sockNew);
    FD_SET(sockNew, &g_fdset);
    g_max_sockfd = (sockNew > g_max_sockfd) ? sockNew : g_max_sockfd;

    return 0;
}
/*
OpenSSL使用SSL_read() 函数来读取数据，跟使用read()一样，我们只需要简单的选择一个合适大小的缓冲，然后将它传递给SS L_read()函数。
注意到缓冲区的大小在此处并没有多么的重要，SSL_read() 和read()一样，返回可用的数据，哪怕它比请求的数据量小. 
另外，如果没有数据可以读取，读函数将会阻塞

如果返回值是0，并不表示没有数据可以读取，因为在没有数据可以读取的情况下，正如上面已经讨论过的一样, 我们的函数肯定会被阻塞住的。
所以,此处返回的0表明这个套接字已经被关闭了，当然也就没有任何数据可以读取了，所以我们退出循环。

如果返回值是一个负值，这时肯定发生了某种错误。我们只关心两种类型的错误：普通错误和提前关闭的错误，我们使用SSL_get_error()函数来决定得到的是那 
种类型的错误，差错处理在客户端的程序中非常的简单。所以对于大多数错误，我们仅仅使用berr_exit()函数来打印一行错误信息然后退出，
然后,提前关闭这种错 误需要进行特殊的处理.

*/
int hancle_package(int sockfd)
{
    int received = 0;
    char buf[1024];
    SSL *p_ssl = NULL;
    int TotalReceived = 0;
    static int count = 0;
    int ret = 0;

    memset(buf, 0, sizeof(buf));
    p_ssl = getSslFd();
    printf("recive socket %d data ssl %p\n",sockfd, p_ssl);

    received = SSL_read(p_ssl, buf, sizeof(buf));
    printf("SSL_read %d\n", received);
    if (received > 0) {
        TotalReceived += received;
        printf("SSL_read data %s\n", buf);
        ret = SSL_write(p_ssl, buf, strlen(buf));
        printf("%d bytes sent\n", ret);
    } else {
        int err = SSL_get_error(p_ssl, received);
        printf("SSL_read error %d\n", err);
        switch (err) {
            case SSL_ERROR_NONE://The TLS/SSL I/O operation completed. This result code is returned if and only if ret > 0.
            {
                // no real error, just try again...
                printf("SSL_ERROR_NONE \n");
                break;
            }
            case SSL_ERROR_ZERO_RETURN: 
            {
                // peer disconnected...
                printf("SSL_ERROR_ZERO_RETURN \n");
                break;
            } 
            case SSL_ERROR_WANT_READ:
            {
                // no data available right now, wait a few seconds in case new data arrives...
                printf("SSL_ERROR_WANT_READ \n");
            }
            case SSL_ERROR_WANT_WRITE: 
            {
                // socket not writable right now, wait a few seconds and try again...
                printf("SSL_ERROR_WANT_WRITE \n");
            }
            default:
                SSL_free(p_ssl);
                close(sockfd);
                FD_CLR(sockfd,&g_fdset);
                printf("error %i:%i\n", received, err); 
                break;
        }
        count++;
        if (count == 5) {
            exit(0);
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
                    hancle_package(sock_index);
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


int main(int argc, char *const argv[])
{
    struct sockaddr_in server_addr;
    int listen_sd = -1;
    int ret = 0;

    printf("hello\n");

    initSslLibrary(1);
    /* 创建一个 socket 用于 tcp 通信 */
    listen_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sd < 0) {

        perror("Error: Failed to create socket\n");
        return -1;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    int value = 1;
    ret = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char *)&value, sizeof(value));
    if (ret < 0) {
        perror("Error: Failed to setsockopt socket\n");
        return -1;
    }

    ret = bind(listen_sd, (struct sockaddr*) &server_addr,sizeof (server_addr));
    if (ret < 0) {
        perror("Error: Failed to bind socket\n");
        return -1;
    }

    ret = listen(listen_sd, 5);
    if (ret < 0) {
        perror("Error: Failed to listen socket\n");
        return -1;
    }

    printf("listen socket is %d\n",listen_sd);
    FD_SET(listen_sd, &g_fdset);
    g_max_sockfd = listen_sd;
    g_listen_sd = listen_sd;

    ret = pthread_create(&g_package_pthread, NULL, (void *)handle_package, NULL);
    if (ret != 0) {
        printf("Failed to pthread_create %d\n",ret);
        close(listen_sd);
        return -1;
    }
    pthread_join(g_package_pthread, NULL);

    printf("exit \n");
    return 0;
}