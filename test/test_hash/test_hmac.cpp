#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hashSha2.h"
#include "hashHmac.h"

/* echo -n "abcde" |openssl dgst -md5 
 * (stdin)= ab56b4d92b40713acc5af89985d4b786
 * echo -n "abcde" | openssl dgst -sha1
 * (stdin)= 03de6c570bfe24bfc328ccd7ca46b76eadaf4334
 * echo -n "abcde" | openssl dgst -sha224
 * (stdin)= bdd03d560993e675516ba5a50638b6531ac2ac3d5847c61916cfced6
 * echo -n "abcde" | openssl dgst -sha256
 * (stdin)= 36bbe50ed96841d10443bcb670d6554f0a34b761be67ec9c4a8ad2c0c44ca42c
 * echo -n "abcde" | openssl dgst -sha384
 * (stdin)= 4c525cbeac729eaf4b4665815bc5db0c84fe6300068a727cf74e2813521565abc0ec57a37ee4d8be89d097c0d2ad52f0
 * echo -n "abcde" | openssl dgst -sha512
 * (stdin)= 878ae65a92e86cac011a570d4c30a7eaec442b85ce8eca0c2952b5e3cc0628c2e79d889ad4d5c7c626986d452dd86374b6ffaa7cd8b67665bef2289a5c70b0a1
 * echo -n "abcde" | openssl dgst -sha256 -hmac "123456"
 * (stdin)= 99a5016fdb72a79f5ee487ef22536ba1e8f7aa50178758ce3bce4e2f50807b09
 */
//PRF_HMAC_MD5

/* 
(gdb) x/64xb fixed_nonce.ptr
0x7fffc4002360:	0x09	0xff	0xcb	0x80	0xfe	0x33	0xf9	0x95
0x7fffc4002368:	0xc5	0x32	0x6e	0x66	0x13	0xba	0xc7	0x86
0x7fffc4002370:	0x4f	0x12	0x7c	0x4a	0x57	0x2a	0xdb	0x00
0x7fffc4002378:	0xae	0xa1	0xa1	0x5a	0xbf	0x8c	0xbd	0x4d
0x7fffc4002380:	0x8b	0xc2	0xed	0xd8	0x92	0x2f	0x04	0x1a
0x7fffc4002388:	0xce	0x47	0x84	0xf3	0xfd	0x06	0x48	0x64
0x7fffc4002390:	0x67	0xaa	0x1b	0x4d	0x1c	0x2a	0x4c	0x72
0x7fffc4002398:	0x47	0x33	0x51	0x47	0x62	0x78	0xf5	0x66

(gdb) x/128xb secret.ptr
0x7fffc4001fd0:	0xa2	0xf9	0x0e	0xfd	0x32	0x83	0x1a	0x06
0x7fffc4001fd8:	0x82	0x14	0x2b	0x2a	0xe3	0x38	0xfe	0x86
0x7fffc4001fe0:	0xe9	0x0b	0xb9	0x6f	0xac	0xb7	0x3b	0x2d
0x7fffc4001fe8:	0xb6	0x4e	0x3f	0x45	0x8f	0x7b	0x67	0x29
0x7fffc4001ff0:	0x4e	0xe1	0x66	0xdb	0x1c	0x27	0xac	0xa3
0x7fffc4001ff8:	0xfc	0x5e	0x19	0x07	0x04	0x09	0x22	0xa0
0x7fffc4002000:	0xb8	0xaa	0xde	0x3d	0x26	0x18	0x9e	0xc9
0x7fffc4002008:	0xa2	0xda	0x2e	0x70	0x28	0x86	0xad	0x66
0x7fffc4002010:	0x5a	0xf4	0x5c	0x94	0x4b	0x63	0x40	0x8a
0x7fffc4002018:	0x90	0xe9	0x5a	0x25	0x70	0xf2	0x45	0xee
0x7fffc4002020:	0x8f	0xbc	0x61	0x7d	0x15	0xb5	0x2d	0x4a
0x7fffc4002028:	0x11	0xfb	0xfe	0xce	0x36	0x71	0xee	0xf9
0x7fffc4002030:	0xdc	0x38	0x10	0xd4	0xd0	0xe4	0xb5	0x93
0x7fffc4002038:	0x07	0x17	0xfd	0x46	0xe0	0x0b	0xf1	0x36
0x7fffc4002040:	0xc5	0xe2	0xe1	0xf3	0x26	0x36	0x27	0x35
0x7fffc4002048:	0x37	0x95	0xfa	0x29	0x45	0xfb	0x31	0xa4


(gdb) x/16xb skeyseed.ptr
0x7fffc4002410:	0xb6	0x0e	0x1e	0x9b	0x9f	0x43	0xd5	0x32
0x7fffc4002418:	0xeb	0x5c	0x6c	0x5f	0x23	0xdd	0x94	0x58
 */
void SKEYSEED()
{
	int i = 0;

	/*  SKEYSEED = prf(Ni | Nr, g^ir) */
	/* gir */
	u8_t buf[128]={     0xa2,0xf9,0x0e,0xfd,0x32,0x83,0x1a,0x06,0x82,0x14,0x2b,0x2a,0xe3,0x38,0xfe,0x86,
		                0xe9,0x0b,0xb9,0x6f,0xac,0xb7,0x3b,0x2d,0xb6,0x4e,0x3f,0x45,0x8f,0x7b,0x67,0x29,
		                0x4e,0xe1,0x66,0xdb,0x1c,0x27,0xac,0xa3,0xfc,0x5e,0x19,0x07,0x04,0x09,0x22,0xa0,
		                0xb8,0xaa,0xde,0x3d,0x26,0x18,0x9e,0xc9,0xa2,0xda,0x2e,0x70,0x28,0x86,0xad,0x66,
		                0x5a,0xf4,0x5c,0x94,0x4b,0x63,0x40,0x8a,0x90,0xe9,0x5a,0x25,0x70,0xf2,0x45,0xee,
		                0x8f,0xbc,0x61,0x7d,0x15,0xb5,0x2d,0x4a,0x11,0xfb,0xfe,0xce,0x36,0x71,0xee,0xf9,
		                0xdc,0x38,0x10,0xd4,0xd0,0xe4,0xb5,0x93,0x07,0x17,0xfd,0x46,0xe0,0x0b,0xf1,0x36,
		                0xc5,0xe2,0xe1,0xf3,0x26,0x36,0x27,0x35,0x37,0x95,0xfa,0x29,0x45,0xfb,0x31,0xa4};
	/* NiNr.bin*/
	u8_t mac_key[64]={  0x09,0xff,0xcb,0x80,0xfe,0x33,0xf9,0x95,0xc5,0x32,0x6e,0x66,0x13,0xba,0xc7,0x86,
						0x4f,0x12,0x7c,0x4a,0x57,0x2a,0xdb,0x00,0xae,0xa1,0xa1,0x5a,0xbf,0x8c,0xbd,0x4d,
						0x8b,0xc2,0xed,0xd8,0x92,0x2f,0x04,0x1a,0xce,0x47,0x84,0xf3,0xfd,0x06,0x48,0x64,
						0x67,0xaa,0x1b,0x4d,0x1c,0x2a,0x4c,0x72,0x47,0x33,0x51,0x47,0x62,0x78,0xf5,0x66};
	chunk_t chunk;
	u8_t digist[80]= {0};
	
	HashHmac *hmac = new HashHmac();
	if (hmac == NULL)
	{
		printf("new hmac error\n");
	}
	hmac->setHashType(HASH_MD5);
	chunk_t key = {mac_key,64};
	hmac->setHashKey(key);
	chunk.ptr = buf;
	chunk.len = 128;
	u32_t size = hmac->getHmacSize();
	chunk_t out = {NULL,0};
	out.ptr = (u8_t*)malloc(size);
	
	hmac->getHashHmac(chunk, out);
	for(i = 0; i< out.len; i++)
	{
		printf("%02x ",out.ptr[i]);
	}
	printf("\n");

	delete(hmac);

}
/*   prf+ (K,S) = T1 | T2 | T3 | T4 | ...
	 where:
	 T1 = prf (K, S | 0x01)
	 T2 = prf (K, T1 | S | 0x02)
	 T3 = prf (K, T2 | S | 0x03)
	 T4 = prf (K, T3 | S | 0x04)
	 
	KEYMAT = prf+ (SKEYSEED, Ni | Nr | SPIi | SPIr)
	KEYMAT = SK_d | SK_ai | SK_ar | SK_ei | SK_er | SK_pi | SK_pr


(gdb) x/16xb skeyseed.ptr
0x7fffc4002410:	0xb6	0x0e	0x1e	0x9b	0x9f	0x43	0xd5	0x32
0x7fffc4002418:	0xeb	0x5c	0x6c	0x5f	0x23	0xdd	0x94	0x58

(gdb) x/80xb prf_plus_seed.ptr
0x7fffc40023b0:	0x09	0xff	0xcb	0x80	0xfe	0x33	0xf9	0x95
0x7fffc40023b8:	0xc5	0x32	0x6e	0x66	0x13	0xba	0xc7	0x86
0x7fffc40023c0:	0x4f	0x12	0x7c	0x4a	0x57	0x2a	0xdb	0x00
0x7fffc40023c8:	0xae	0xa1	0xa1	0x5a	0xbf	0x8c	0xbd	0x4d
0x7fffc40023d0:	0x8b	0xc2	0xed	0xd8	0x92	0x2f	0x04	0x1a
0x7fffc40023d8:	0xce	0x47	0x84	0xf3	0xfd	0x06	0x48	0x64
0x7fffc40023e0:	0x67	0xaa	0x1b	0x4d	0x1c	0x2a	0x4c	0x72
0x7fffc40023e8:	0x47	0x33	0x51	0x47	0x62	0x78	0xf5	0x66
0x7fffc40023f0:	0x4e	0x15	0xd3	0x2a	0xd9	0x8d	0x63	0xe4
0x7fffc40023f8:	0x2d	0x2d	0xd7	0x5d	0x01	0x7d	0x93	0x0f


(gdb) x/16xb this->skd.ptr
0x7fffc4002410:	0x58	0x1c	0x7a	0x92	0x5b	0x3a	0x62	0x19
0x7fffc4002418:	0xf0	0xf1	0x32	0xec	0xe3	0xa4	0xfc	0x9d
(gdb) x/16xb sk_ai.ptr
0x7fffc4002ae0:	0xb5	0xcd	0x1f	0x5a	0xcd	0xec	0x18	0x76
0x7fffc4002ae8:	0x26	0x85	0xc4	0xd3	0xa0	0x5e	0xa4	0x71
(gdb) x/16xb sk_ar.ptr
0x7fffc4002b00:	0xb5	0xf4	0x40	0x0c	0x6c	0x21	0xc4	0x7d
0x7fffc4002b08:	0x9c	0xed	0xbf	0x3a	0x85	0x26	0xd8	0x4e
(gdb) x/24xb sk_ei.ptr
0x7fffc4002b20:	0xf9	0x49	0x66	0xef	0xb3	0x53	0x2d	0xa2
0x7fffc4002b28:	0xb0	0x15	0x01	0xef	0x3e	0xd3	0x90	0xf5
0x7fffc4002b30:	0x52	0xf0	0x50	0xbf	0xe9	0xdc	0x10	0xaa
(gdb) x/24xb sk_er.ptr
0x7fffc4002b40:	0x92	0xf0	0x78	0xe8	0x8d	0xcf	0x99	0x69
0x7fffc4002b48:	0xdd	0x83	0xf7	0x96	0x89	0x35	0x4a	0x90
0x7fffc4002b50:	0x3a	0x89	0xda	0x86	0x33	0x8a	0x3e	0x6b
(gdb) x/16xb this->skp_build.ptr
0x7fffc4002b40:	0xaf	0xb7	0xf8	0xc4	0x69	0xf7	0xb4	0x2e
0x7fffc4002b48:	0x88	0xd3	0x6e	0x28	0x2b	0x48	0x0c	0x19

(gdb) x/16xb this->skp_verify.ptr
0x7fffc4002b20:	0xb7	0x8c	0xe1	0x2a	0x73	0xe4	0x58	0x13
0x7fffc4002b28:	0x5c	0x0f	0xb3	0xd2	0x4b	0x0f	0xc7	0xcf


*/

void KEYMAT()
{
	int i = 0;
	chunk_t chunk_empty2 = {NULL,0};

	u8_t mac_key[16] = {0xb6,0x0e,0x1e,0x9b,0x9f,0x43,0xd5,0x32,0xeb,0x5c,0x6c,0x5f,0x23,0xdd,0x94,0x58};

	u8_t prf_plus_seed[80]={  	0x09,0xff,0xcb,0x80,0xfe,0x33,0xf9,0x95,0xc5,0x32,0x6e,0x66,0x13,0xba,0xc7,0x86,
								0x4f,0x12,0x7c,0x4a,0x57,0x2a,0xdb,0x00,0xae,0xa1,0xa1,0x5a,0xbf,0x8c,0xbd,0x4d,
								0x8b,0xc2,0xed,0xd8,0x92,0x2f,0x04,0x1a,0xce,0x47,0x84,0xf3,0xfd,0x06,0x48,0x64,
								0x67,0xaa,0x1b,0x4d,0x1c,0x2a,0x4c,0x72,0x47,0x33,0x51,0x47,0x62,0x78,0xf5,0x66,
								0x4e,0x15,0xd3,0x2a,0xd9,0x8d,0x63,0xe4,0x2d,0x2d,0xd7,0x5d,0x01,0x7d,0x93,0x0f};

	
	
	chunk_t NiNrSPIiSPIr = {prf_plus_seed,80};

	/* KEYMAT = prf+ (SKEYSEED, Ni | Nr | SPIi | SPIr) */
	HashHmac *hmac = new HashHmac();
	if (hmac == NULL)
	{
		printf("new hmac error\n");
	}
	hmac->setHashType(HASH_MD5);
	chunk_t skeyseed = {mac_key,16};
	hmac->setHashKey(skeyseed);

	u32_t size = hmac->getHmacSize();
	chunk_t out = {NULL,0};
	out.ptr = (u8_t*)malloc(size);
	out.len = size;
	hmac->getHashHmac(NiNrSPIiSPIr, chunk_empty2);  //chunk_empty.ptr == NULL   append text

	/*T1 = prf (K, S | 0x01)*/
	u8_t count = 0x01;
	chunk_t counter = {&count,1};
	
	hmac->getHashHmac(counter, out);
	printf(" sk_d:");
	for(i = 0; i< out.len; i++)
	{
		printf("%02x ",out.ptr[i]);
	}
	printf("\n");

/*  	 T2 = prf (K, T1 | S | 0x02)  sk_ai*/

	hmac->getHashHmac(out, chunk_empty2);
	hmac->getHashHmac(NiNrSPIiSPIr, chunk_empty2);
	count++;
	hmac->getHashHmac(counter, out);

	printf("sk_ai:");
	for(i = 0; i< out.len; i++)
	{
		printf("%02x ",out.ptr[i]);
	}
	printf("\n");
	
/*		 T3 = prf (K, T2 | S | 0x03)  sk_ar*/

	hmac->getHashHmac(out, chunk_empty2);
	hmac->getHashHmac(NiNrSPIiSPIr, chunk_empty2);
	count++;
	hmac->getHashHmac(counter, out);

	printf("sk_ar:");
	for(i = 0; i< out.len; i++)
	{
		printf("%02x ",out.ptr[i]);
	}
	printf("\n");


	chunk_t des3 = {NULL,0};
	des3.ptr = (u8_t*)malloc(24);
	des3.len = 24;

/*		 T4 = prf (K, T3 | S | 0x04)  sk_ei 3des*/
	
	hmac->getHashHmac(out, chunk_empty2);
	hmac->getHashHmac(NiNrSPIiSPIr, chunk_empty2);
	count++;
	hmac->getHashHmac(counter, out);
	/* 3des ����24 ��ǰ16�ֽ� */
	memcpy(des3.ptr, out.ptr, out.len);
/*		 T5 = prf (K, T4 | S | 0x05)  sk_ei 3des*/

	hmac->getHashHmac(out, chunk_empty2);
	hmac->getHashHmac(NiNrSPIiSPIr, chunk_empty2);
	count++;
	hmac->getHashHmac(counter, out);
	memcpy(des3.ptr+16, out.ptr, 8);

	printf("sk_ei:");
	for(i = 0; i< 24; i++)
	{
		printf("%02x ",des3.ptr[i]);
	}
	printf("\n");
		
	/*		 T6 = prf (K, T5 | S | 0x06)  sk_er 3des*/
	memcpy(des3.ptr, out.ptr+8, 8);
	hmac->getHashHmac(out, chunk_empty2);
	hmac->getHashHmac(NiNrSPIiSPIr, chunk_empty2);
	count++;
	hmac->getHashHmac(counter, out);
	memcpy(des3.ptr+8, out.ptr, out.len);
	
	printf("sk_er:");
	for(i = 0; i< 24; i++)
	{
		printf("%02x ",des3.ptr[i]);
	}
	printf("\n");

/*		 T7 = prf (K, T6 | S | 0x07)  sk_pi */
	
	hmac->getHashHmac(out, chunk_empty2);
	hmac->getHashHmac(NiNrSPIiSPIr, chunk_empty2);
	count++;
	hmac->getHashHmac(counter, out);
	printf("sk_pi:");

	for(i = 0; i< out.len; i++)
	{
		printf("%02x ",out.ptr[i]);
	}
	printf("\n");

/*		 T8 = prf (K, T7 | S | 0x08)  sk_pr */
	
	hmac->getHashHmac(out, chunk_empty2);
	hmac->getHashHmac(NiNrSPIiSPIr, chunk_empty2);
	count++;
	hmac->getHashHmac(counter, out);
	printf("sk_pr:");

	for(i = 0; i< out.len; i++)
	{
		printf("%02x ",out.ptr[i]);
	}
	printf("\n");	

	free(out.ptr);
	free(des3.ptr);
	delete(hmac);

}

int main(int argc, char* argv[])
{
	//SKEYSEED();
	KEYMAT();
	return 0;
}
