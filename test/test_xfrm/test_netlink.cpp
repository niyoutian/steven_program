#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "xfrmKernelNetlink.h"

/*
htons()作用是将端口号由主机字节序转换为网络字节序的整数值。(host to net)
inet_addr()作用是将一个IP字符串转化为一个网络字节序的整数值，用于sockaddr_in.sin_addr.s_addr。
inet_ntoa()作用是将一个sin_addr结构体输出成IP字符串(network to ascii)。比如：


*/

int main(int argc, char* argv[])
{
	u32_t spi = 0;
	xfrmKernelInterface *netlink = new xfrmKernelNetlink(); 

	printf("netlink\n");
	mxLogInit("net");
	netlink->createSocket(0);
	//https://blog.csdn.net/qingzhuyuxian/article/details/79736821
	struct sockaddr_in src_addrV4;
	struct sockaddr_in dst_addrV4;
	memset(&src_addrV4, 0 ,sizeof(struct sockaddr_in));
	memset(&dst_addrV4, 0 ,sizeof(struct sockaddr_in));
	src_addrV4.sin_family = AF_INET;
	dst_addrV4.sin_family = AF_INET;
	src_addrV4.sin_addr.s_addr = inet_addr("172.20.3.22");//172.20.3.22
	dst_addrV4.sin_addr.s_addr = inet_addr("172.20.3.30"); //172.20.3.30
	
	netlink->getSpi((struct sockaddr *)&src_addrV4,(struct sockaddr *)&dst_addrV4,50,spi);
	return 0;
}

