#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "xfrmKernelNetlink.h"

/*
htons()�����ǽ��˿ں��������ֽ���ת��Ϊ�����ֽ��������ֵ��(host to net)
inet_addr()�����ǽ�һ��IP�ַ���ת��Ϊһ�������ֽ��������ֵ������sockaddr_in.sin_addr.s_addr��
inet_ntoa()�����ǽ�һ��sin_addr�ṹ�������IP�ַ���(network to ascii)�����磺


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

