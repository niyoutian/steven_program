#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/ec.h>

#include <openssl/evp.h>


#define ECDH_SIZE   256


void handleErrors()
{    
	printf("Error occurred.\n");
}

void disp(const char *str, const void *pbuf, const int size)
{    
	int i=0;   
	
	if(str !=NULL){        
		printf("%s:\n", str);    
	}    
	if(pbuf !=NULL && size > 0){        
		for(i=0;i<size;i++)            
			printf("%02x ", *((unsigned char *)pbuf+i));        
		putchar('\n');    
	}    
	putchar('\n');
}

EC_KEY *genECDHpubkey(unsigned char *pubkey, size_t& lenn)
{    
	int len; 
	int ret = 0;
	
	//Generate Public    
	EC_KEY * ecdh = EC_KEY_new_by_curve_name(NID_secp256k1);//NID_secp256k1 
	/*或者
	EC_KEY *ecdh = EC_KEY_new();
	EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1); // NID_secp256k1
	EC_KEY_set_group(ecdh, group);
	*/

	
	ret = EC_KEY_generate_key(ecdh);  

	
	const EC_POINT *point = EC_KEY_get0_public_key(ecdh); //传输给对方的公钥   
	const EC_GROUP *group = EC_KEY_get0_group(ecdh);        

	BIGNUM *x = BN_new();
	BIGNUM *y = BN_new();
	if (EC_POINT_get_affine_coordinates_GFp(group, point, x, y, NULL))
	{
		BN_print_fp(stdout, x);
		putc('\n', stdout);
		BN_print_fp(stdout, y);
		putc('\n', stdout);
	}
	BN_free(x);
	BN_free(y);
	

	//将公钥由POINT格式转成OCT字符串
	if(0 == (len =EC_POINT_point2oct(group, point,POINT_CONVERSION_COMPRESSED, pubkey,ECDH_SIZE, NULL))) {
		handleErrors();
	}
	printf("len=%d\n",len);  

	disp("pubkey", pubkey, len);  

	
	lenn = len;
	return ecdh;
}

unsigned char *genECDHsharedsecret(EC_KEY *ecdh,unsigned char *peerkey,size_t secret_len)
{    
	int len;    
	int ret = 0;
	unsigned char *shared = (unsigned char *)malloc(ECDH_SIZE);  
	memset(shared,0,ECDH_SIZE);
	const EC_GROUP *group = EC_KEY_get0_group(ecdh); 
	
	//ComputeKey    
	EC_POINT *point_peer =EC_POINT_new(group);    
	ret = EC_POINT_oct2point(group, point_peer, peerkey,secret_len, NULL);  
	printf("genECDHsharedsecret ret = %d\n",ret);
	if(ret == 0) {
		handleErrors();
		return NULL;
	}
 
	if(0 == (len =ECDH_compute_key(shared, ECDH_SIZE-1, point_peer, ecdh,NULL))){ 
		handleErrors();    
	}
	
	printf("len=%d\n",len);    
	disp("shared", shared, len);    
	
	return shared;
}


//https://blog.csdn.net/houwenbin1986/article/details/99683910
//本地计算的方式
int testECDH(void) 
{    
	unsigned char *keydata = (unsigned char *)malloc(ECDH_SIZE);    
	unsigned char *keydata2 = (unsigned char *)malloc(ECDH_SIZE);  
	size_t len1 = 0;
	size_t len2 = 0;
	
	EC_KEY *ecdh =genECDHpubkey(keydata, len1);    
	EC_KEY *ecdh2 =genECDHpubkey(keydata2, len2);   
	
	unsigned char *ECDH_keydata = genECDHsharedsecret(ecdh2, keydata,len1);    
	unsigned char *ECDH_keydata2 = genECDHsharedsecret(ecdh, keydata2,len2);  

	if (0==memcmp(ECDH_keydata, ECDH_keydata2, ECDH_SIZE - 1)) //算出来的共享密钥必须相同
	{
		printf("------sharedsecret ok------\n");
	}

	printf("To the end\n"); 

	
	free(keydata);    
	free(keydata2);        
	EC_KEY_free(ecdh);    
	EC_KEY_free(ecdh2);        
	free(ECDH_keydata);    
	free(ECDH_keydata2);    
	
	return 0;
}


int main()
{ 
	testECDH();

	return 0;
}


