#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "test_multi_func.h"


pthread_t g_package_pthread;
fd_set g_fdset;
int g_max_sockfd;
int g_listen_sd = -1;


int handle_data(int sockfd)
{
    int received = 0;
    unsigned char buff[50] = {0};
    int i = 0;
    int send_len = 0;
    struct msghdr msg = { 0 };
    struct sockaddr_in saddr = { 0 };
    int addr_len = sizeof(struct sockaddr_in);
    struct iovec iov[1] = { 0 };
	char ctrl_buffer[CMSG_SPACE(sizeof(struct in_pktinfo)) + CMSG_SPACE(sizeof(int))] = { 0 };
	struct cmsghdr* cmh = (struct cmsghdr*)ctrl_buffer;

    //用于获取辅助数据
    struct cmsghdr *pcmsg = NULL;
   //2个存放ip地址的缓冲区,可用可不用
    char route_ip_buf[16];
    char dst_ip_buf[16];
    
	msg.msg_name = &saddr;
	msg.msg_namelen = addr_len;
	msg.msg_iov = &iov[0];
	msg.msg_iovlen = 1;
	msg.msg_control = cmh;
	msg.msg_controllen = sizeof(ctrl_buffer);
	iov[0].iov_base = buff;
	iov[0].iov_len = sizeof(buff);

	received = recvmsg(sockfd, &msg, 0);
    if (received > 0) {
        printf("received len=%d\n",received);

        //开始获取辅助数据,由于辅助数据可以是一个也可以是一个数组,因此循环;
        for(pcmsg = CMSG_FIRSTHDR(&msg) ; pcmsg != NULL ; pcmsg = CMSG_NXTHDR(&msg,pcmsg)) {
            //判断是否是包信息
            if(pcmsg->cmsg_level == IPPROTO_IP && pcmsg->cmsg_type == IP_PKTINFO) {
                unsigned char * pData = CMSG_DATA(pcmsg);
                struct in_pktinfo * pInfo = (struct in_pktinfo *)pData;
                inet_ntop(AF_INET,&pInfo->ipi_addr,dst_ip_buf,sizeof(dst_ip_buf));
                inet_ntop(AF_INET,&pInfo->ipi_spec_dst,route_ip_buf,sizeof(route_ip_buf));
                //下面都是打印信息

                printf("route ip :%s, dst ip:%s , ifindex:%d\n" , route_ip_buf,dst_ip_buf, pInfo->ipi_ifindex);
            }
        }
        printf("recive:%s\n",buff);
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
                    //hancle_accept();
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

/*
struct ip_mreq 
{ 
      struct in_addr imn_multiaddr; //加入或者退出的广播组IP地址
      struct in_addr imr_interface; //加入或者退出的网络接口IP地址
}
    选项IP_ADD_MEMBERSHIP用于加入某个广播组，之后就可以向这个广播组发送数据或者从广播组接收数据。此选项的值为mreq结构，
成员imn_multiaddr是需要加入的广播组IP地址，成员imr_interface是本机需要加入广播组的网络接口IP地址。
    使用IP_ADD_MEMBERSHIP选项每次只能加入一个网络接口的IP地址到多播组，但并不是一个多播组仅允许一个主机IP地址加入，
可以多次调用IP_ADD_MEMBERSHIP选项来实现多个IP地址加入同一个广播组，或者同一个IP地址加入多个广播组。当imr_ interface为INADDR_ANY时，选择的是默认组播接口。
————————————————
原文链接：https://blog.csdn.net/Windgs_YF/article/details/109806547
*/
int test_server_case1(void)
{
    int socketfd = 0;
    int ret = 0;
    struct ip_mreq mreq;


    memset(&mreq, 0, sizeof(struct ip_mreq));

    socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketfd < 0) {
        printf("create socket failed\n");
        return -1;
    }


	int flag = 1;
	ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
    if (ret < 0) {
        printf("Error: Failed to setsockopt socket\n");
        close(socketfd);
        return -1;
    }

    flag = 1;
    ret = setsockopt(socketfd,IPPROTO_IP,IP_PKTINFO,&flag,sizeof(flag));
    if (ret < 0) {
        printf("Error: Failed to setsockopt socket\n");
        close(socketfd);
        return -1;
    }

    inet_pton(AF_INET, (const char *)"224.244.224.245", &(mreq.imr_multiaddr));
    inet_pton(AF_INET, (const char *)UDP_SERVER_IP, &(mreq.imr_interface));
    //inet_pton(AF_INET, (const char *)"192.168.146.130", &(mreq.imr_interface));
    printf("my ip %s\n","192.168.146.130");
    //mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    ret = setsockopt(socketfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq));
    if (ret < 0) {
        printf("Error: Failed to setsockopt socket\n");
        close(socketfd);
        return -1;
    }

	struct sockaddr_in srcaddr;
	memset(&srcaddr, 0, sizeof(struct sockaddr_in));
	srcaddr.sin_family = AF_INET;
    //inet_pton(AF_INET, (const char *)UDP_SERVER_IP, &(srcaddr.sin_addr));
	srcaddr.sin_port = htons(UDP_SERVER_PORT);

#if 0
	char multi_loop = 0;
	setsockopt(sock->socket, IPPROTO_IP, IP_MULTICAST_LOOP, &multi_loop, sizeof(multi_loop));
	struct in_addr localInterface;
	localInterface.s_addr = htonl(ch->local_addr.ip);
	setsockopt(sock->socket, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&localInterface, sizeof(localInterface));
#endif

    ret = bind(socketfd, (struct sockaddr*)&srcaddr, sizeof(struct sockaddr_in));
    if (ret != 0) {
        printf("Fail to bind socket %d, ret %d, errno: %d\n", socketfd, ret, errno);
        close(socketfd);
        return -1;
    }

    FD_SET(socketfd, &g_fdset);
    g_max_sockfd = socketfd;

    ret = pthread_create(&g_package_pthread, NULL, (void *)handle_package, NULL);
    if (ret != 0) {
        printf("Failed to pthread_create %d\n",ret);
        close(socketfd);
        return -1;
    }

    pthread_join(g_package_pthread, NULL);
    close(socketfd);
    printf("exit \n");
    return 0;
}

int main(int argc, char *const argv[])
{
    printf("enter multicase server\n");

    test_server_case1();

    return 0;
}