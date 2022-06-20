#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/x509.h>

void tSign()
{
	unsigned char sign_value[1024];	//保存签名值的数组
	unsigned int sign_len;			//签名值长度
	EVP_MD_CTX *mdctx = NULL;		//摘要算法上下文变量
	char mess1[] = "1234567890";;		//待签名的消息
	RSA *rsa=NULL;			//RSA结构体变量
	EVP_PKEY *evpKey=NULL;		//EVP KEY结构体变量
	int i;
	
#if 0
	rsa = RSA_generate_key(1024,RSA_F4,NULL,NULL);//产生一个1024位的RSA密钥
	if(rsa == NULL)
	{
		printf("gen rsa err\n");
		return;
	}

#else
	RSA* pRSApri = RSA_new();
	BIO* pBIOpri2 = BIO_new_file("rsa.key","r");
	d2i_RSAPrivateKey_bio(pBIOpri2, &pRSApri);
	BIO_free(pBIOpri2);
#endif
	evpKey = EVP_PKEY_new();//新建一个EVP_PKEY变量
	if(evpKey == NULL)
	{
		printf("EVP_PKEY_new err\n");
		RSA_free(rsa);
		return ;
	}
	if(EVP_PKEY_set1_RSA(evpKey,pRSApri) != 1)	//保存RSA结构体到EVP_PKEY结构体
	{
		printf("EVP_PKEY_set1_RSA err\n");
		RSA_free(rsa);
		EVP_PKEY_free(evpKey);
		return;
	}
	

	//以下是计算签名代码
	mdctx = EVP_MD_CTX_new();
	printf("mdctx=%p\n",mdctx);

	if(!EVP_SignInit_ex(mdctx, EVP_sha1(), NULL))//签名初始化;设置摘要算法;本例为SHA1
	{
		printf("err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	printf("EVP_SignInit_ex\n");
	if(!EVP_SignUpdate(mdctx, mess1, strlen(mess1)))//计算签名;摘要;Update
	{
		printf("err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	printf("EVP_SignUpdate\n");
	if(!EVP_SignFinal(mdctx,sign_value,&sign_len,evpKey))	//签名输出
	{
		printf("EVP_SignFinal err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	printf("消息\"%s\"的签名值是: \n",mess1);
	for(i = 0; i < sign_len; i++)
	{
		if(i%16==0)
			printf("\n%08xH: ",i);
		printf("%02x ", sign_value[i]);	
	}
	printf("\n");	
	EVP_MD_CTX_free(mdctx);
	
	printf("\n正在验证签名...\n");
	//以下是验证签名代码
	mdctx = EVP_MD_CTX_new();
	if(!EVP_VerifyInit_ex(mdctx, EVP_md5(), NULL))//验证初始化&#xff0c;设置摘要算法&#xff0c;一定要和签名一致。
	{
		printf("EVP_VerifyInit_ex err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	if(!EVP_VerifyUpdate(mdctx, mess1, strlen(mess1)))//验证签名&#xff08;摘要&#xff09;Update
	{
		printf("err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}	
	if(!EVP_VerifyFinal(mdctx,sign_value,sign_len,evpKey))//验证签名
	{
		printf("verify err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	else
	{
		printf("验证签名正确.\n");
	}
	//释放内存
	EVP_PKEY_free(evpKey);
	RSA_free(rsa);
	EVP_MD_CTX_free(mdctx);
	return;
}
int main()
{ 
	OpenSSL_add_all_algorithms();
	tSign();
	return 0;
}

