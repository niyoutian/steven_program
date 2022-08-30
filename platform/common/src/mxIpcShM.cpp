#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include "mxIpcShM.h"
#include "mxLog.h"




mxIpcShM::mxIpcShM():mShMId(-1),mpShMAddr(NULL),mSize(0)
{
	//mxLogFmt(LOG_DEBUG,  "in mxIpcShM\n");
}

mxIpcShM::~mxIpcShM()
{
	if(mpShMAddr)
	{
		detach();
		mpShMAddr = NULL;
	}
	mShMId = -1;
	mSize = 0;
	//mxLogFmt(LOG_DEBUG,  "out mxIpcShM\n");
}





s32_t  mxIpcShM::create(key_t key, s32_t size, s32_t flags)
{
	mShMId = shmget(key, size, flags | IPC_CREAT );
	if(mShMId == -1)
	{
		mxLogFmt(LOG_ERR,"creat share memory filed %s\n",strerror(errno));
		return -1;
	}
	mpShMAddr = shmat(mShMId, NULL, 0);
	if(mpShMAddr == (void*)-1) 
	{
		mxLogFmt(LOG_ERR,  "attach share memory error %s\n", strerror(errno));
		return -1;
	}
	mxLogFmt(LOG_DEBUG,  "attach share memory address %p\n", mpShMAddr);
	struct shmid_ds  shmbuf;
	if( shmctl(mShMId, IPC_STAT, &shmbuf) == -1) 
	{
		mxLogFmt(LOG_ERR,  "get share memory status error %s\n",strerror(errno));
		return -1;
	} 
	mxLogFmt(LOG_DEBUG,  "share memory real size %d\n", shmbuf.shm_segsz);
	if(shmdt(mpShMAddr) == -1)
	{
		mxLogFmt(LOG_ERR,  "detaching share memory error  %s\n", strerror(errno));
	}
	mpShMAddr = NULL;
	return 0;
}

s32_t  mxIpcShM::attach(s32_t key, s32_t flags)
{
	mShMId = shmget(key, 0,  flags);
	if(mShMId == -1) 
	{
		mxLogFmt(LOG_ERR,   "get share memory with key %d error %s\n", key, strerror(errno));
		return -1;
	}
	mpShMAddr = shmat(mShMId, NULL, 0);
	if(mpShMAddr == (void*)-1) {
		mxLogFmt(LOG_ERR,   "attach share memory with key %d error %s\n", key, strerror(errno));
		return -1;
	}

	return 0;	

}


s32_t   mxIpcShM::detach(void)
{

	if(!mpShMAddr)
	{
		mxLogFmt(LOG_ERR,  "share memory is not attached. addr is NULL.\n");
		return -1;
	}
	
	s32_t  dm = shmdt(mpShMAddr);
	if(dm == -1) 
		mxLogFmt(LOG_ERR,  "detaching share memory error  %s\n", strerror(errno));

	mpShMAddr = NULL;
	
	return dm;
}

s32_t  mxIpcShM::remove(void)
{
	s32_t r = -1;

	r = shmctl(mShMId, IPC_RMID, NULL);

	if(r == -1) 
		mxLogFmt(LOG_ERR,  "removing error %s \n",strerror(errno));
	
	mpShMAddr = NULL;

	return r;
}

s32_t  mxIpcShM::getSize(void)
{
	s32_t cv;
	struct shmid_ds buff;

	if(mSize > 0)
		return mSize;
	
	cv = shmctl(mShMId, IPC_STAT, &buff);
	if( !cv )
		mSize = buff.shm_segsz;

	return !cv ? mSize : 0;
}

s32_t mxIpcShM::getAttachNum(void)
{
	struct shmid_ds	numbuf;

	if(shmctl(mShMId,IPC_STAT, &numbuf)<0) 
		return -1;

	return numbuf.shm_nattch;
}



