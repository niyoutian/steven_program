#include <unistd.h>
#include <string.h>
#if 0
#include "mxLog.h"
#include "mxEngineCurl.h"


static const char* strreplace(char *str, const char *search, const char *replace)
{
	size_t len, slen, rlen, count = 0;
	char *res, *pos, *found = NULL, *dst;

	if (!str || !*str || !search || !*search || !replace)
	{
		return (char*)str;
	}
	slen = strlen(search);
	rlen = strlen(replace);
	if (slen != rlen)
	{
		for (pos = (char*)str; (pos = strstr(pos, search)); pos += slen)
		{
			found = pos;
			count++;
		}
		if (!count)
		{
			return (char*)str;
		}
		len = (found - str) + strlen(found) + count * (rlen - slen);
	}
	else
	{
		len = strlen(str);
	}
	found = strstr(str, search);
	if (!found)
	{
		return (char*)str;
	}
	dst = res = (char*)malloc(len + 1);
	pos = (char*)str;
	do
	{
		len = found - pos;
		memcpy(dst, pos, len);
		dst += len;
		memcpy(dst, replace, rlen);
		dst += rlen;
		pos = found + slen;
	}
	while ((found = strstr(pos, search)));
	strcpy(dst, pos);
	return res;
}

size_t save_sina_page(void *buffer, size_t size, size_t count, void *user_p)
{
	mxLogFmt(LOG_DEBUG,"write file sina page, size: %d",size);
    return fwrite(buffer, size, count, (FILE *)user_p);
}

size_t save_sohu_page(void *buffer, size_t size, size_t count, void *user_p)
{
	mxLogFmt(LOG_DEBUG,"write file sohu page, size: %d",size);
    return fwrite(buffer, size, count, (FILE *)user_p);
}

int main(int argc, char* argv[])
{
	mxLogInit("libcurl");
	mxLogFmt(LOG_DEBUG,"libcurl test");
	mxEngineCurl * engine = new mxEngineCurl();
	mxLogFmt(LOG_DEBUG,"engine %p",engine);
	engine->setEngineTimeout(3000);
	engine->startEngine();
    CURL *easy_handle1 = NULL;
    CURL *easy_handle2 = NULL;
    extern size_t save_sina_page(void *buffer, size_t size, size_t count, void *user_p);
    extern size_t save_sohu_page(void *buffer, size_t size, size_t count, void *user_p);
    FILE *fp_sina = fopen("./sina.txt", "ab+");
    FILE *fp_sohu = fopen("./sohu.txt", "ab+");


    easy_handle1 = curl_easy_init();
    char uri[256] = {0};
    strncpy(uri, "http://10.2.2.112:8080/ejbca/publicweb/webdist/certdist?cmd=crl&issuer=C=CH, L=Default City, O=Mavenir.com, CN=mavenir Root CA1", 256);
    const char* encUri = strreplace(uri, " ", "%20");
    curl_easy_setopt(easy_handle1, CURLOPT_URL, encUri);
    curl_easy_setopt(easy_handle1, CURLOPT_WRITEFUNCTION, save_sina_page);
    curl_easy_setopt(easy_handle1, CURLOPT_WRITEDATA, fp_sina);

    easy_handle2 = curl_easy_init();
    strncpy(uri,"http://10.2.2.112:8080/ejbca/publicweb/webdist/certdist?cmd=crl&issuer=C=CH, O=Linux strongSwan, CN=strongSwan Root CA", 256); 
    const char* encUri2 = strreplace(uri, " ", "%20"); 
    curl_easy_setopt(easy_handle2, CURLOPT_URL, encUri2);
    curl_easy_setopt(easy_handle2, CURLOPT_WRITEFUNCTION, save_sohu_page);
    curl_easy_setopt(easy_handle2, CURLOPT_WRITEDATA, fp_sohu);
	engine->addHandle(easy_handle1);
	sleep(1);
	engine->addHandle(easy_handle2);
	sleep(10);
	return 0;
}
#else
int main(int argc, char* argv[])
{
    return 0;
}
#endif
