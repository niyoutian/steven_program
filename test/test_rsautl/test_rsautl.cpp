#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/x509.h>

void tSign()
{
	unsigned char sign_value[1024];	//����ǩ��ֵ������
	unsigned int sign_len;			//ǩ��ֵ����
	EVP_MD_CTX *mdctx = NULL;		//ժҪ�㷨�����ı���
	char mess1[] = "1234567890";;		//��ǩ������Ϣ
	RSA *rsa=NULL;			//RSA�ṹ�����
	EVP_PKEY *evpKey=NULL;		//EVP KEY�ṹ�����
	int i;
	
#if 0
	rsa = RSA_generate_key(1024,RSA_F4,NULL,NULL);//����һ��1024λ��RSA��Կ
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
	evpKey = EVP_PKEY_new();//�½�һ��EVP_PKEY����
	if(evpKey == NULL)
	{
		printf("EVP_PKEY_new err\n");
		RSA_free(rsa);
		return ;
	}
	if(EVP_PKEY_set1_RSA(evpKey,pRSApri) != 1)	//����RSA�ṹ�嵽EVP_PKEY�ṹ��
	{
		printf("EVP_PKEY_set1_RSA err\n");
		RSA_free(rsa);
		EVP_PKEY_free(evpKey);
		return;
	}
	

	//�����Ǽ���ǩ������
	mdctx = EVP_MD_CTX_new();
	printf("mdctx=%p\n",mdctx);

	if(!EVP_SignInit_ex(mdctx, EVP_sha1(), NULL))//ǩ����ʼ��;����ժҪ�㷨;����ΪSHA1
	{
		printf("err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	printf("EVP_SignInit_ex\n");
	if(!EVP_SignUpdate(mdctx, mess1, strlen(mess1)))//����ǩ��;ժҪ;Update
	{
		printf("err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	printf("EVP_SignUpdate\n");
	if(!EVP_SignFinal(mdctx,sign_value,&sign_len,evpKey))	//ǩ�����
	{
		printf("EVP_SignFinal err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	printf("��Ϣ\"%s\"��ǩ��ֵ��: \n",mess1);
	for(i = 0; i < sign_len; i++)
	{
		if(i%16==0)
			printf("\n%08xH: ",i);
		printf("%02x ", sign_value[i]);	
	}
	printf("\n");	
	EVP_MD_CTX_free(mdctx);
	
	printf("\n������֤ǩ��...\n");
	//��������֤ǩ������
	mdctx = EVP_MD_CTX_new();
	if(!EVP_VerifyInit_ex(mdctx, EVP_md5(), NULL))//��֤��ʼ��&#xff0c;����ժҪ�㷨&#xff0c;һ��Ҫ��ǩ��һ�¡�
	{
		printf("EVP_VerifyInit_ex err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	if(!EVP_VerifyUpdate(mdctx, mess1, strlen(mess1)))//��֤ǩ��&#xff08;ժҪ&#xff09;Update
	{
		printf("err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}	
	if(!EVP_VerifyFinal(mdctx,sign_value,sign_len,evpKey))//��֤ǩ��
	{
		printf("verify err\n");
		EVP_PKEY_free(evpKey);
		RSA_free(rsa);
		return;
	}
	else
	{
		printf("��֤ǩ����ȷ.\n");
	}
	//�ͷ��ڴ�
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

