#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "mxEngineEpoll.h"

mxEngineEpoll::mxEngineEpoll() : mEpollFd(0)
{
	mEpollFd = epoll_create(MAX_EPOLLENGINE);
}

mxEngineEpoll::~mxEngineEpoll()
{
	close(mEpollFd);
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
				
			}
			if (events[n].events & EPOLLOUT)
			{
				
			}
			if (events[n].events & EPOLLERR)
			{
				
			}
		}
	}
	return 0;
}
