#include <memory.h>
#include <curl/curl.h>
#include "mxEngineCurl.h"



mxEngineCurl::mxEngineCurl():mpMutliHandle(NULL)
{
	CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);
    if (ret != CURLE_OK)
		engineLog(LOG_WARNING,"global libcurl initializing failed");
	mpMutliHandle = curl_multi_init();
}

mxEngineCurl::~mxEngineCurl()
{
	if(mpMutliHandle)
 		curl_multi_cleanup(mpMutliHandle);
    curl_global_cleanup();	
}

s32_t mxEngineCurl::addHandle(CURL *easy_handle)
{
	if(!easy_handle)
		return -1;
	curl_multi_add_handle(mpMutliHandle, easy_handle);
	return 0;
}

s32_t mxEngineCurl::remHandle(CURL *easy_handle)
{
	if(!easy_handle)
		return -1;
	curl_multi_remove_handle(mpMutliHandle, easy_handle);
	curl_easy_cleanup(easy_handle);
	return 0;
}


s32_t mxEngineCurl::service(s32_t timeout)
{
	fd_set rd_fds;
	fd_set wr_fds;
	fd_set ex_fds;
	s32_t  max_fd;
	s32_t nfds = 0;
	
	struct timeval tv;
	memset(&tv,0,sizeof(tv));
	tv.tv_sec = timeout/1000;
	tv.tv_usec = (timeout%1000)*1000;
	
	s32_t running_handle_count = 0;
	
	struct CURLMsg *m = NULL;
	while (isOnService())
	{
		FD_ZERO(&rd_fds);
		FD_ZERO(&wr_fds);
		FD_ZERO(&ex_fds);
		curl_multi_fdset(mpMutliHandle, &rd_fds, &wr_fds, &ex_fds, &max_fd);
		if ((nfds = select(max_fd + 1, &rd_fds, &wr_fds, &ex_fds, &tv)) < 0)
		{
			engineLog(LOG_WARNING,"curl select error");
			continue;
		}
		else
		{
			while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(mpMutliHandle, &running_handle_count));
			do {
				s32_t msgq = 0;
				m = curl_multi_info_read(mpMutliHandle, &msgq);
				if(m && (m->msg == CURLMSG_DONE)) 
				{
					CURL *e = m->easy_handle;
					curl_multi_remove_handle(mpMutliHandle, e);
					curl_easy_cleanup(e);
				}
			} while(m);
		}
	}
	return 0;
}

