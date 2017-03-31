#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "mxEngineEpoll.h"
#include "mxHandlePacket.h"

mxEngineEpoll::mxEngineEpoll() : mEpollFd(0)
{
	mEpollFd = epoll_create(MAX_EPOLLENGINE);
}

mxEngineEpoll::~mxEngineEpoll()
{
	close(mEpollFd);
}

s32_t mxEngineEpoll::addEvent(s32_t fd, u8_t event)
{
	s32_t ret = 0;
	struct epoll_event ev = {0};
	ev.events =  EPOLLERR;
	if (event & EVENT_READ)
		ev.events |= EPOLLIN;
	if (event & EVENT_WRITE)
		ev.events |= EPOLLOUT;
	if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &ev) < 0)
	{
		engineLog(LOG_WARNING,"epoll_ctl ADD failed");
		ret = -1;
	}
	return ret;
}

s32_t mxEngineEpoll::modEvent(s32_t fd, u8_t event)
{
	s32_t ret = 0;
	struct epoll_event ev = {0};
	ev.events =  EPOLLERR;
	if (event & EVENT_READ)
		ev.events |= EPOLLIN;
	if (event & EVENT_WRITE)
		ev.events |= EPOLLOUT;
	if (epoll_ctl(mEpollFd, EPOLL_CTL_MOD, fd, &ev) < 0)
	{
		engineLog(LOG_WARNING,"epoll_ctl MOD failed");
		ret = -1;
	}
	return ret;	
}

s32_t mxEngineEpoll::remEvent(s32_t fd)
{
	s32_t ret = 0;
	if (epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
	{
		engineLog(LOG_WARNING,"epoll_ctl DEL failed");
		ret = -1;
	}
	return ret;	
}


s32_t mxEngineEpoll::service(s32_t timeout)
{
	struct epoll_event events[MAX_EVENTS_PER_TRIGGERING];
	s32_t nfds = 0;

	timeout = (timeout == 0) ? -1 : timeout;
	
	while (isOnService())
	{
		if ((nfds = epoll_wait(mEpollFd, events, MAX_EVENTS_PER_TRIGGERING, timeout)) < 0)
		{
			continue;
		}
		for (u32_t n = 0; n < nfds; n++)
		{
			if (events[n].events & EPOLLIN)
			{
				mpHandle->eventRead(events[n].data.fd);
			}
			if (events[n].events & EPOLLOUT)
			{
				mpHandle->eventWrite(events[n].data.fd);
			}
			if (events[n].events & EPOLLERR)
			{
				mpHandle->eventError(events[n].data.fd);
				struct sockaddr_in local_addr;
				u32_t len_addr = sizeof(local_addr);
				getsockname( events[n].data.fd, (struct sockaddr*)&local_addr, &len_addr );
				engineLog(LOG_WARNING,"event error fd %d, port %d",events[n].data.fd,ntohs( local_addr.sin_port ));
			}
		}
	}
	return 0;
}
