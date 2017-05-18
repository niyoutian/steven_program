#ifndef __MX_ENGINE_CURL_H__
#define __MX_ENGINE_CURL_H__
#include "mxDef.h"
#include "mxEngineBase.h"




class mxEngineCurl : public mxEngineBase
{
public:
	mxEngineCurl();
	~mxEngineCurl();
	s32_t addHandle(CURL *easy_handle);
	s32_t remHandle(CURL *easy_handle);
	s32_t service(s32_t timeout);
private:
	CURLM*		mpMutliHandle;
};











#endif

