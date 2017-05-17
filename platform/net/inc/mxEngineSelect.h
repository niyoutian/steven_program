#ifndef __MX_ENGINE_SELECT_H__
#define __MX_ENGINE_SELECT_H__
#include "mxDef.h"
#include "mxEngineBase.h"

#define MAX_SELECTENGINE                50
#define MAX_EVENTS_PER_TRIGGERING		10
class mxEngineSelect : public mxEngineBase
{
public:
	mxEngineSelect();
	~mxEngineSelect();
	s32_t addEvent(s32_t fd, u8_t event);
	s32_t modEvent(s32_t fd, u8_t event);
	s32_t remEvent(s32_t fd);
	s32_t service(s32_t timeout);
private:
	fd_set		mRdFds;
	fd_set		mWrFds;
	fd_set		mExFds;
	s32_t		mMaxFd;

};


#endif

