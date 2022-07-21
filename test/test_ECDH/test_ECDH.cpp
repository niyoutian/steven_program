#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/ec.h>

#include <openssl/evp.h>
#include <openssl/pem.h>



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

# define B_FORMAT_TEXT   0x8000
# define FORMAT_UNDEF    0
# define FORMAT_TEXT    (1 | B_FORMAT_TEXT)     /* Generic text */
# define FORMAT_BINARY   2                      /* Generic binary */
# define FORMAT_BASE64  (3 | B_FORMAT_TEXT)     /* Base64 */
# define FORMAT_ASN1     4                      /* ASN.1/DER */
# define FORMAT_PEM     (5 | B_FORMAT_TEXT)


#define FILE_PUBA "ec_secp256k1_pubA.pem"
#define FILE_PRIA "ec_secp256k1_A.pem"
#define FILE_PUBB "ec_secp256k1_pubB.pem"
#define FILE_PRIB "ec_secp256k1_B.pem"



EC_KEY *getECDHprikey(const char *file)
{
	BIO *in = NULL;
	EC_KEY *eckey = NULL;
	
	in = BIO_new_file(file, "r");
	if (in == NULL) {
		printf("open %s failed\n",file);
		return NULL;
	}
	eckey = PEM_read_bio_ECPrivateKey(in, NULL, NULL, NULL);
    if (eckey == NULL) {
        printf("unable to load Key\n");
        BIO_free(in);
		return NULL;
    }
	printf("load priKey\n");

	BIO_free(in);
	return eckey;
}

const EC_POINT *getECDHpubkey(const char *file)
{
	BIO *in = NULL;
	EC_KEY *eckey = NULL;
	const EC_POINT *point = NULL;
	
	in = BIO_new_file(file, "r");
	if (in == NULL) {
		printf("open %s failed\n",file);
		return NULL;
	}
	eckey = PEM_read_bio_EC_PUBKEY(in, NULL, NULL, NULL);
    if (eckey == NULL) {
        printf("unable to load Key\n");
        BIO_free(in);
		return NULL;
    }
	printf("load pubKey\n");
	point = EC_KEY_get0_public_key(eckey); //传输给对方的公钥 

	BIO_free(in);
	return point;
}

unsigned char *genECDHsharedsecret2(EC_KEY *ecdh, const EC_POINT *point_peer)
{    
	int len;    
	int ret = 0;
	unsigned char *shared = (unsigned char *)malloc(ECDH_SIZE);  
	memset(shared,0,ECDH_SIZE);
	
	//ComputeKey   
	len =ECDH_compute_key(shared, ECDH_SIZE-1, point_peer, ecdh,NULL);
	if(0 == len){ 
		handleErrors();    
	}
	
	printf("len=%d\n",len);    
	disp("shared", shared, len);    
	
	return shared;
}



int testECDH_file(void) 
{    
	EC_KEY *PRIA_key = NULL;
	EC_KEY *PRIB_key = NULL;
	const EC_POINT *pointA = NULL;
	const EC_POINT *pointB = NULL;

	PRIA_key = getECDHprikey(FILE_PRIA);
	PRIB_key = getECDHprikey(FILE_PRIB);

	pointA = getECDHpubkey(FILE_PUBA);
	pointB = getECDHpubkey(FILE_PUBB);

//	const EC_GROUP *group = EC_KEY_get0_group(eckey);

	/* A 计算 shared secret */
	unsigned char *ECDH_keydataA = genECDHsharedsecret2(PRIA_key,pointB);
	/* B 计算 shared secret */
	unsigned char *ECDH_keydataB = genECDHsharedsecret2(PRIB_key,pointA);

	if (0==memcmp(ECDH_keydataA, ECDH_keydataB, ECDH_SIZE - 1)) //算出来的共享密钥必须相同
	{
		printf("------sharedsecret ok------\n");
	}

	printf("To the end\n"); 

end:	
	EC_KEY_free(PRIA_key);
	EC_KEY_free(PRIB_key);
	free(ECDH_keydataA);    
	free(ECDH_keydataB); 

	return 0;
}


int main()
{ 
	//testECDH();
	testECDH_file();
	return 0;
}


