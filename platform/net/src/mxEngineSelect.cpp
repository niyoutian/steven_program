#include <memory.h>
#include "mxEngineSelect.h"
#include "mxHandlePacket.h"



mxEngineSelect::mxEngineSelect():mMaxFd(-1)
{
	FD_ZERO(&mRdFds);
	FD_ZERO(&mWrFds);
	FD_ZERO(&mExFds);	
}

mxEngineSelect::~mxEngineSelect()
{
	
}

s32_t mxEngineSelect::addEvent(s32_t fd, u8_t event)
{
	FD_CLR(fd, &mRdFds);
	FD_CLR(fd, &mWrFds);
	FD_CLR(fd, &mExFds);
	if (event & EVENT_READ)
	{
		FD_SET(fd, &mRdFds);
	}
	if (event & EVENT_WRITE)
	{
		FD_SET(fd, &mWrFds);
	}
	if (event)
	{
		FD_SET(fd, &mExFds);
		mMaxFd= (fd > mMaxFd) ? fd : mMaxFd;
	}
	return 0;
}

s32_t mxEngineSelect::modEvent(s32_t fd, u8_t event)
{
	FD_CLR(fd, &mRdFds);
	FD_CLR(fd, &mWrFds);
	FD_CLR(fd, &mExFds);
	if (event & EVENT_READ)
	{
		FD_SET(fd, &mRdFds);
	}
	if (event & EVENT_WRITE)
	{
		FD_SET(fd, &mWrFds);
	}
	if (event)
	{
		FD_SET(fd, &mExFds);
	}
	return 0;
}

s32_t mxEngineSelect::remEvent(s32_t fd)
{
	FD_CLR(fd, &mRdFds);
	FD_CLR(fd, &mWrFds);
	FD_CLR(fd, &mExFds);
	if (fd == mMaxFd)
	{
		while (--mMaxFd> 0)
		{
			if (FD_ISSET(mMaxFd, &mExFds))
				break;
		}
	}
	return 0;
}


s32_t mxEngineSelect::service(s32_t timeout)
{
	fd_set rd_fds;
	fd_set wr_fds;
	fd_set ex_fds;

	FD_ZERO(&rd_fds);
	FD_ZERO(&wr_fds);
	FD_ZERO(&ex_fds);
	s32_t nfds = 0;

	struct timeval tv, *ptv=NULL;
	memset(&tv,0,sizeof(tv));
	
	while (isOnService())
	{
		rd_fds = mRdFds;
		wr_fds = mWrFds;
		ex_fds = mExFds;
		if(timeout>=0)
		{
			tv.tv_sec = timeout/1000;
			tv.tv_usec = (timeout%1000)*1000;
			ptv = &tv;
		}

		if ((nfds = select(mMaxFd + 1, &rd_fds, &wr_fds, &ex_fds, ptv)) < 0)
		{
			engineLog(LOG_WARNING,"engine select error");
			continue;
		}
		for (u32_t n = 0; n <= mMaxFd && (nfds > 0); n++)
		{
			bool isset = false;
			if (FD_ISSET(n, &rd_fds))
			{	
				isset = true;
				mpHandle->eventRead(n);
			}
			if (FD_ISSET(n, &wr_fds))
			{
				isset = true;
				mpHandle->eventWrite(n);
			}
			if (FD_ISSET(n, &ex_fds))
			{
				isset = true;
				mpHandle->eventError(n);
			}
			if (isset)
				nfds--;
		}
	}
	return 0;
}


