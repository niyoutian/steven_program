#ifndef __MX_ENGINE_EPOLL_H__
#define __MX_ENGINE_EPOLL_H__
#include "mxDef.h"
#include "mxEngineBase.h"

#define MAX_EPOLLENGINE                         		50
#define MAX_EVENTS_PER_TRIGGERING		10
class mxEngineEpoll : public mxEngineBase
{
public:
	mxEngineEpoll();
	~mxEngineEpoll();
	s32_t service(s32_t timeout);
private:
	s32_t     mEpollFd;
};


#endif