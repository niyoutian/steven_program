#ifndef __MXIPCSHM_H__
#define __MXIPCSHM_H__
#include <sys/types.h>
#include "mxDef.h"










class mxIpcShM
{
public:
	mxIpcShM();
	~mxIpcShM();
	s32_t  create(key_t key, s32_t size, s32_t flags=0666);
	s32_t  attach(s32_t key, s32_t flags=0666);
	s32_t  detach(void);
	s32_t  remove(void);
	void*  getAddr(void){return mpShMAddr;}
	s32_t  getSize(void);
	s32_t  getAttachNum(void);
private:
	s32_t  mShMId;
	void*  mpShMAddr;
	s32_t  mSize;
};














#endif

