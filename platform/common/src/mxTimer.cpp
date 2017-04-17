#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/times.h>
#include "mxTimer.h"





mxTimerMgr::mxTimerMgr()
{  
	int i;
	tv1.num = 0;
	tv2.num = 0;
	tv3.num = 0;
	tv4.num = 0;
	tv5.num = 0;
	tvecs[0] = (struct timerVec *)&tv1;
	tvecs[1] = &tv2;
	tvecs[2] = &tv3;
	tvecs[3] = &tv4;
	tvecs[4] = &tv5;
	mActiveTimerCount = 0;
	mBaseTick = 0 ;  
	mRevise = false;
	mStatus = false;
	for ( i = 0; i < TVN_SIZE; i++ ) 
	{
		INIT_LIST_HEAD(tv5.vec + i);
		INIT_LIST_HEAD(tv4.vec + i);
		INIT_LIST_HEAD(tv3.vec + i);
		INIT_LIST_HEAD(tv2.vec + i);
	}
	for ( i = 0; i < TVR_SIZE; i++ )
	{
		INIT_LIST_HEAD(tv1.vec + i);
	}

#ifdef TIMER_RETRY
	reorderTimerCount = 0;
	dropTimerCount = 0;
#endif // TIMER_RETRY
	mKilledTimerCount = 0;
	mThreadId = 0;
	mTickBase = 0;
	mTickLast = 0;
    	discardNum = 0;
	mClkRate = sysconf( _SC_CLK_TCK );
}


mxTimerMgr::~mxTimerMgr()
{
	Stop();
}




mxTimerMgr* mxTimerMgr::mpInstance = NULL;

mxTimerMgr* mxTimerMgr::getInstance(void)
{
	if(mpInstance == NULL)
		mpInstance = new mxTimerMgr();
	return mpInstance;
}

void mxTimerMgr::StartTimer(void)
{
	mMutex.lock();
    	if ( mThreadId )
	{
		mxLogFmt(LOG_ERR, "the timer thread has been started!\n");
        	mMutex.unlock();
		return;
	}
	mStatus = true;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create( &mThreadId, &attr, mainThreadEntry, this );
	mMutex.wait();
	mMutex.unlock();
}

void* mxTimerMgr::mainThreadEntry(void* param)
{
	mxTimerMgr *pEngine = (mxTimerMgr*) param;
	mxLogFmt(LOG_DEBUG,"enter \n");
	pEngine->timerTask();
	return NULL;
}

void mxTimerMgr::timerTask(void)
{
	struct sched_param sched;
	/*The number of ticks per second*/
	mxLogFmt(LOG_DEBUG,"start timer task with clock rate %ld\n",mClkRate);
	sched.sched_priority = 0x70;
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &sched);
	mMutex.lock();
    	reviseBaseTick();
	mMutex.signal();
	mMutex.unlock();
	
	while(mStatus)
	{
		//mxLogFmt(_CRT_DEBUG,LOG_MX,"hello\n");
		runTimerList();
		taskDelay(1);
	}
}

void mxTimerMgr::taskDelay( u32_t ms )
{
	usleep( ms*1000 );
}

void mxTimerMgr::Stop(void)
{
	void *pRet = NULL;
	mStatus= false;
	if(mThreadId) 
	{
		if(pthread_join(mThreadId, &pRet ) != 0) 
		{
			int err = errno;
			mxLogFmt(LOG_ERR, "pthread_join fail. reason %s \n",strerror(err));
		}
		mThreadId = 0;
	}
}

void mxTimerMgr::listAdd( timerBlock_t *newTimer, timerBlock_t *prev, timerBlock_t * next )
{
	next->prev = newTimer;
	newTimer->next = next;
	newTimer->prev = prev;
	prev->next = newTimer;
}

void mxTimerMgr::listDel( timerBlock_t *entry )
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;

	entry->next = NULL;
	entry->prev = NULL;
    if ( entry->whichPan == &tv1 )
    {
        tv1.num--;
    }
    else if ( entry->whichPan == &tv2 )
    {
        tv2.num--;
    }
    else if ( entry->whichPan == &tv3 )
    {
        tv3.num--;
    }
    else if ( entry->whichPan == &tv4 )
    {
        tv4.num--;
    }
    else if ( entry->whichPan == &tv5 )
    {
        tv5.num--;
    }
}


timerBlock_t* mxTimerMgr::listSearch(timerBlock_t * head ,u16_t timerId)
{
	timerBlock_t * pos = NULL;
	for(pos=head->next;pos && pos != head; pos=pos->next)
	{
		if(pos->timerId == timerId)
		{
			mxLogFmt(LOG_DEBUG, "find timerBlock %p\n",pos);
			return pos;
		}
	}
	return NULL;
}


timerBlock_t* mxTimerMgr::allocTmBlk(u32_t &timerId)
{
	timerId = mActiveTimerCount+1;
	timerBlock_t* pTmBlk = (timerBlock_t*)malloc(sizeof(timerBlock_t));
	mxLogFmt(LOG_DEBUG, "allocTmBlk timerBlock %p\n",pTmBlk);
	return pTmBlk;
}

timerBlock_t* mxTimerMgr::getTmBlkByTimerId(u32_t timerId)
{
	u8_t vec_index = (u8_t)((timerId >>24) & 0xff);
	u8_t list_index = (u8_t)((timerId >>16) & 0xff);
	u16_t timer_index = (u16_t)(timerId & 0xffff);
	if(vec_index>4)
		return NULL;
	return listSearch(&(tvecs[vec_index]->vec[list_index]), timer_index);
}

void mxTimerMgr::returnTmBlk( timerBlock_t* pTmBlk )
{
	/*arrTimerBlock.dealloc( pTmBlk->timerId );*/
	mxLogFmt(LOG_DEBUG, "returnTmBlk timerBlock %p\n",pTmBlk);
	if(pTmBlk)
	{
		free(pTmBlk);
   		mActiveTimerCount-- ;
	}
}

inline u32_t mxTimerMgr::getClkRate(void)
{
    return (u32_t) mClkRate;
}

u32_t mxTimerMgr::msToTick( u32_t msCount )
{
	u64_t temp = msCount;

	if( 0 ==  msCount )
	{ 
		return 0;  
	}
	u32_t  Ret = (u32_t)( (temp*getClkRate())/1000 );      
	return Ret>0 ? Ret : 1;
}
u32_t mxTimerMgr::tickToMs( u32_t tick )
{    
	return ( tick*1000 )/( getClkRate() );
}

u32_t mxTimerMgr::setTimer( timerPara_t *param )
{
	u32_t       timerId = INVALID_TIMER_ID;
	mxLogFmt(LOG_DEBUG, "enter\n");
   	mMutex.lock();
	if ( mThreadId == 0 )
	{
		mMutex.unlock();
		StartTimer();
		mMutex.lock();
	}

	if ( !mRevise )
	{
		mMutex.wait();
	}
	u64_t tickCurrent = getCurrentTickNoSema();

	//timerBlock_t *pTmBlk = arrTimerBlock.alloc(&timerId);
	//timerBlock_t *pTmBlk = NULL;
	timerBlock_t *pTmBlk = allocTmBlk(timerId);
	mxLogFmt(LOG_DEBUG, "allocTmBlk %p\n",pTmBlk);
	if ( NULL == pTmBlk )
	{	
		mMutex.unlock();
		mxLogFmt(LOG_ERR, "alloc timer block failure!\n");
		return INVALID_TIMER_ID;
	}

   	if( timerId >= MAX_TIMERID )
	{ 
		returnTmBlk(pTmBlk);
		mMutex.unlock();
		mxLogFmt(LOG_ERR,"maximum timers started!\n");
		return INVALID_TIMER_ID;
	}
		
	INIT_TIMER_BLOCK( pTmBlk );

	pTmBlk->timerId = timerId;

	if (PARAMETER_MSG_POINTER == param->paraType )
	{
		/*pTmBlk->pMsg = param->pMsg;*/
	} 
	else if (PARAMETER_MSG_OID == param->paraType )
	{
		/*memcpy( &pTmBlk->dstOid, &param->dstOid, sizeof(oid_t) );
		pTmBlk->data = param->data;
		pTmBlk->msgId = param->msgId;
		pTmBlk->priorityLevel = param->priorityLevel;*/
	} 
	else 
	{
		pTmBlk->Func= param->Func;
    		pTmBlk->para = param->para;
	}

	pTmBlk->paraType = param->paraType;

	pTmBlk->settedTick = msToTick( param->interval );
#ifdef TIMER_RETRY
	pTmBlk->timeToLeft = 0;
#endif // TIMER_RETRY
   	pTmBlk->tick = msToTick( param->interval ) + tickCurrent;
	
	//pTmBlk->traceId = param->traceId;
    	pTmBlk->file = param->file;
    	pTmBlk->line = param->line;
    	pTmBlk->caller = param->caller;
	mActiveTimerCount++;
   	internalAddTimer( pTmBlk );
	mMutex.unlock();
	timerId |= (pTmBlk->vec<<24 | pTmBlk->list<<16);
	mxLogFmt(LOG_DEBUG,"TimerId 0x%08x interval:%d started file:%s line:%d caller:%s\n", timerId, param->interval, param->file, param->line, param->caller);
	return  timerId;
}

void mxTimerMgr::reviseBaseTick(void)
{
	s64_t  curTick = getCurrentTickNoSema();
	/*TVR_MASK = 0xff*/
   	u16_t  index1 = (u16_t)(curTick & TVR_MASK);
	u16_t  index2 = (u16_t)((curTick >> TVR_BITS) & TVN_MASK);
	u16_t  index3 = (u16_t)((curTick >> (TVR_BITS + TVN_BITS)) & TVN_MASK);
	u16_t  index4 = (u16_t)((curTick >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK);
	u16_t  index5 = (u16_t)((curTick >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK);
	/*every tick is 10ms*/
	mBaseTick = curTick;
	tv1.index = index1;          
	
	/* add this to avoid infinite loop later */
	if ( tv1.index == 0 )
	{
		tv1.index++;
		mBaseTick++;
	} 
	tv2.index = index2+1;
	tv3.index = index3+1;
	tv4.index = index4+1;
	tv5.index = index5+1;
	
	mRevise = true;
	mxLogFmt(LOG_DEBUG, "revise baseTick:0x%08x\n", mBaseTick);
}

bool mxTimerMgr::cancelTimer( u32_t TimerId, const s8_t *file, u32_t line, const s8_t *caller )
{
	mxLogFmt(LOG_DEBUG, "TimerId 0x%x is cancelled file:%s line:%d function:%s\n", TimerId, file, line,caller);
#if 1
	timerBlock_t *pTmBlk = NULL;
	//tMsg *pToBeFreeMsg = NULL;
    	u8_t *pToBeFreePara = NULL;

	if ( INVALID_TIMER_ID == TimerId )
	{
		mxLogFmt(LOG_WARNING, "TimerId == INVALID_TIME_ID!!! file:%s line:%d function:%s\n", file, line, caller);
		return false;
	}
	mMutex.lock();	
	pTmBlk = getTmBlkByTimerId(TimerId);
	if ( NULL == pTmBlk )
	{
		mMutex.unlock();
		mxLogFmt(LOG_DEBUG,"cannot find timer block of TimerId 0x%08x!!! file:%s line:%d function:%s\n", TimerId, file, line, caller);
		return false;
	}
	
	if ( (pTmBlk->prev != NULL) && (pTmBlk->next != NULL) ) 
	{
		listDel( pTmBlk );
		/*if ( (PARAMETER_MSG_POINTER == pTmBlk->paraType)&& ( NULL != pTmBlk->pMsg ))
		{
			pToBeFreeMsg = pTmBlk->pMsg; 
		}*/
		if ( (PARAMETER_CALLBACK == pTmBlk->paraType)&& ( NULL != pTmBlk->para ))
		{
			pToBeFreePara = (u8_t *)pTmBlk->para;
		}
		returnTmBlk( pTmBlk );
		mKilledTimerCount++;
	}
	mMutex.unlock();
                     		
   /* if( pToBeFreeMsg )
        mxFree( pToBeFreeMsg );*/

    if( pToBeFreePara )
        free( pToBeFreePara );
#endif
	return true;
}


u64_t  mxTimerMgr::getCurrentTick()
{   
	u64_t result;
	mMutex.lock();
	result = getCurrentTickNoSema();
	mMutex.unlock();
	return result;   
}

u64_t  mxTimerMgr::getCurrentTickNoSema()
{
	unsigned long tickCurr;
	u64_t result;

    	struct tms buf;
    	clock_t clock = times(&buf);

	//_ASSERT(clock >= 0); 
	if(clock != -1)  
		tickCurr = clock;	
	else 
	{
		tickCurr = (unsigned long) (-errno);
        	mxLogFmt(LOG_ERR,"times() error: %s; convert the errno to ticks 0x%lx\n", strerror(errno), tickCurr);
	}

	if ( (tickCurr < mTickLast) && (sizeof(unsigned long)==4) ){
		mTickBase += (u64_t)0xffffffff;//the 32 bit tick may return to 0 after some time,so add 
		mTickBase += (u64_t)0x1;
	}
	mTickLast = tickCurr;
	
	result = mTickBase + tickCurr;

	return result;   
}

void mxTimerMgr::internalAddTimer( timerBlock_t *timer )
{	
	s64_t     nExpires = timer->tick;     
	u64_t uIdx = nExpires - mBaseTick;
	timerBlock_t *ptVec = NULL;
	u16_t uLoc;
	if ( (s64_t)uIdx < 0 )
	{
		ptVec = ( tv1.vec + tv1.index );
		tv1.num++;
		timer->whichPan = &tv1;
		timer->whichList = tv1.index;
		timer->vec = 0;
		timer->list = tv1.index;
		//TimerLog(_CRT_DEBUG, "add timer 0x%x interval:%d nExpires:%llu baseTick:%llu uIdx:%llu in pan 1 list:%d\n", (timer->timerId + (timer->counter<<MAX_TIMERID_BITS)), tickToMs(timer->settedTick), nExpires, baseTick, uIdx, timer->whichList);
	} 
	else if ( uIdx < TVR_SIZE )
	{
		uLoc = (u16_t)( nExpires & TVR_MASK );
		ptVec = ( tv1.vec + uLoc ) ;
        	tv1.num++;
        	timer->whichPan = &tv1; 
        	timer->whichList = uLoc;
		timer->vec = 0;
		timer->list = uLoc;
		mxLogFmt(LOG_DEBUG, "add timer 0x%x interval:%d nExpires:%llu baseTick:%llu  in pan %x list:%x\n", timer->timerId, tickToMs(timer->settedTick), nExpires, mBaseTick, timer->vec, timer->list);
	} 
	else if ( uIdx < (1 << (TVR_BITS + TVN_BITS)) )
	{
		uLoc = (u16_t)( (nExpires >> TVR_BITS) & TVN_MASK );
		ptVec = (tv2.vec + uLoc);
       	 	tv2.num++;
        	timer->whichPan = &tv2;
        	timer->whichList = uLoc;
		timer->vec = 1;
		timer->list = uLoc;
		//TimerLog(_CRT_DEBUG, "add timer 0x%x interval:%d nExpires:%llu baseTick:%llu uIdx:%llu in pan 2 list:%d\n", (timer->timerId + (timer->counter<<MAX_TIMERID_BITS)), tickToMs(timer->settedTick), nExpires, baseTick, uIdx, timer->whichList);
	} 
	else if ( uIdx < (1 << (TVR_BITS + 2 * TVN_BITS)) )
	{
		uLoc = (u16_t)( (nExpires >> (TVR_BITS + TVN_BITS)) & TVN_MASK );
		ptVec =  ( tv3.vec + uLoc );
        	tv3.num++;
        	timer->whichPan = &tv3;
		timer->whichList = uLoc;
		timer->vec = 2;
		timer->list = uLoc;
		//TimerLog(_CRT_DEBUG, "add timer 0x%x interval:%d nExpires:%llu baseTick:%llu uIdx:%llu in pan 3 list:%d\n", (timer->timerId + (timer->counter<<MAX_TIMERID_BITS)), tickToMs(timer->settedTick), nExpires, baseTick, uIdx, timer->whichList);
	} 
	else if ( uIdx < (1 << (TVR_BITS + 3 * TVN_BITS)) ) 
	{
		uLoc = (u16_t)((nExpires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK);
		ptVec = ( tv4.vec + uLoc );
        	tv4.num++;
       	 	timer->whichPan = &tv4;
        	timer->whichList = uLoc;
		timer->vec = 3;
		timer->list = uLoc;
		//TimerLog(_CRT_DEBUG, "add timer 0x%x interval:%d nExpires:%llu baseTick:%llu uIdx:%llu in pan 4 list:%d\n", (timer->timerId + (timer->counter<<MAX_TIMERID_BITS)), tickToMs(timer->settedTick), nExpires, baseTick, uIdx, timer->whichList);
	} else if ( uIdx <= 0xffffffffUL )
	{
		uLoc = (u16_t)((nExpires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK);
		ptVec = ( tv5.vec + uLoc );
        	tv5.num++;
        	timer->whichPan = &tv5;
        	timer->whichList = uLoc;
		timer->vec = 4;
		timer->list = uLoc;
		//TimerLog(_CRT_DEBUG, "add timer 0x%x interval:%d nExpires:%llu baseTick:%llu uIdx:%llu in pan 5 list:%d\n", (timer->timerId + (timer->counter<<MAX_TIMERID_BITS)), tickToMs(timer->settedTick), nExpires, baseTick, uIdx, timer->whichList);
	} 
	else 
	{
		INIT_LIST_HEAD( timer );
		//TimerLog(_CRT_ERR, "no pan for this timer 0x%x interval:%d  nExpires:%llu baseTick:%llu uIdx:%llu,this timer will be discarded\n", (timer->timerId + (timer->counter<<MAX_TIMERID_BITS)),tickToMs(timer->settedTick), nExpires, baseTick, uIdx);
        	discardNum++;
        	mMutex.lock();
        	returnTmBlk( timer );
        	mMutex.unlock();
		return;
	}
	/*  Timers are FIFO! */
	listAdd( timer, ptVec->prev, ptVec->prev->next );
}

u32_t mxTimerMgr::getTmBlkNum(void)
{       
	u32_t temp;
	mMutex.lock();
	temp = mActiveTimerCount ;
	mMutex.unlock();
	return temp;
} 

void mxTimerMgr::cascadeTimers( timerVec *tv )
{
	timerBlock_t *head = NULL;
	timerBlock_t *curr = NULL;
	timerBlock_t *next = NULL;

	if ( tv->index > TVN_MASK ) 
	{
		tv->index = 0;
	}

	head = tv->vec + tv->index;
	curr = head->next;
	while ( curr != head  && curr ) {
		next = curr->next;
		listDel(curr);
		internalAddTimer(curr);
		curr = next;
	}
	
	INIT_LIST_HEAD(head);
	tv->index = ((tv->index + 1) & TVN_MASK);

	if (tv->index > TVN_MASK) {
		tv->index = 0;
	}
}


void mxTimerMgr::runTimerList(void)
{
   	u32_t timerId; 
	s32_t    ret = -1;
	s64_t    interTick;

	
	interTick = getCurrentTickNoSema();
//	interTick /= TIMER_PRICISION;
	interTick -=  mBaseTick; 
	
    	mMutex.lock();
	
	timerBlock_t *head = NULL;
	timerBlock_t *curr = NULL;
	bool  runFlag = true;

	while( interTick >= 0 )
	{
		if ( !tv1.index ) 
		{
			u32_t n = 1;
			
			do 
			{
                		mxLogFmt(LOG_DEBUG, "cascade n:%d\n", n);
				cascadeTimers( tvecs[n] );
			} while ((tvecs[n]->index == 1) && ((++n) < NOOF_TVECS));
		}

		runFlag = true;
		while( runFlag )
		{
			if( tv1.index > 255 ) 
			{
				tv1.index = 0;
			}

			head = tv1.vec + tv1.index;
			curr = head->next;
			if ( curr != head  && curr ) 
			{
				timerId = curr->timerId;

				listDel(curr);
				returnTmBlk( curr );
				mMutex.unlock();

                		//TimerLog(_CRT_DEBUG, "timer 0x%x expired\n", timerId + (curr->counter << MAX_TIMERID_BITS));
				if (PARAMETER_MSG_POINTER == curr->paraType )
				{
					/*
					mxCallTrace.setTraceIdToTLS((CallTraceId)(curr->traceId));
					ret = mxSend(curr->pMsg);*/
				}
				else if (PARAMETER_MSG_OID == curr->paraType )
				{
				/*	oid_t src;
					INIT_OID_T(src);
					src.ab = mxGetLocalSide();
					src.nodeId = mxGetLocalNode();

					curr->dstOid.routeMode = MX_MODE_DIRECT;
					curr->dstOid.nodeId = mxGetLocalNode();
					curr->dstOid.ab = mxGetLocalSide();


					tMsg *pNewMsg = mxInitMsg5(MX_MODE_ACTIVE,src,curr->msgId,curr->dstOid,curr->priorityLevel);

					pNewMsg->data1 = curr->data;
					pNewMsg->msgRoute.src.subOid = timerId + (curr->counter << MAX_TIMERID_BITS);

					// TODO: set uid, and then send
					mxCallTrace.setTraceIdToTLS((CallTraceId)(curr->traceId));
					ret = mxSend(pNewMsg);
					//ret = mxSendUId(pNewMsg, launchTimer->traceId);
					*/
				}
				else if ( PARAMETER_CALLBACK == curr->paraType )
				{
					curr->Func(curr->para);
					ret = 0;
				}
				mMutex.lock();
#ifdef TIMER_RETRY
				if ( ret != 0 )
				{
					curr->timeToLeft++;

					if (curr->timeToLeft > TIMER_TTL) 
					{
						returnTmBlk(curr);
						dropTimerCount++;
					} 
					else 
					{
						curr->tick += curr->settedTick + TIMER_PRICISION + 1;
						internalAddTimer(curr);
						reorderTimerCount++;
					}
				}
#endif // TIMER_RETRY
			}
			else 
			{
				runFlag = false;
			}
		}
		++mBaseTick; 
		tv1.index = ((tv1.index + 1) & TVR_MASK);
		if ( tv1.index > TVR_MASK ) 
		{
			tv1.index = 0;
		}
	    interTick --; 
	}
	mMutex.unlock();

}


u32_t mxSetTimer(timerPara_t *param )
{
    return mxTimerMgr::getInstance()->setTimer( param );		
}

bool mxCancelTimer(u32_t id, const s8_t *file, u32_t line, const s8_t *caller)
{
	return mxTimerMgr::getInstance()->cancelTimer( id, file, line, caller);
}

void mxStartTimer(void)
{
	mxTimerMgr::getInstance()->StartTimer();
	return ;	
}

void mxStopTimer(void)
{
	mxTimerMgr::getInstance()->Stop();
	return ;
}
#if 0
u32_t mxStartMsTimer1( const s8_t *file, u32_t line, const s8_t *caller, Interval_t millsec, tMsg *pMsg, bool bLoop )
{
/*	timerPara_t param;
	INIT_TIMER_PARA( param );
    
    pMsg->msgRoute.dest.nodeId = mxGetLocalNode();
    pMsg->msgRoute.dest.ab = mxGetLocalSide();
    pMsg->msgRoute.dest.routeMode = MX_MODE_DIRECT;
	param.pMsg = pMsg;
	param.paraType = ::PARAMETER_MSG_POINTER;
	param.interval = millsec;
    param.file = (s8_t *)file;
    param.line = line;
    param.caller = (s8_t *)caller;
	return mavSetTimer( &param );*/
	return 0;
}

TimerId_t mavStartMsTimer2( const s8_t *file, u32_t line, const s8_t *caller, Interval_t millsec, oid_t dstOid, u32_t  data1, u32_t msgId, bool bLoop )
{
	timerPara_t param;
	INIT_TIMER_PARA( param );

	param.msgId = msgId;
	param.data = data1;
	memcpy( &param.dstOid, &dstOid, sizeof(oid_t));
	param.paraType = ::PARAMETER_MSG_OID;
	param.interval = millsec;
    param.file = (s8_t *)file;
    param.line = line;
    param.caller = (s8_t *)caller;
	
	return mavSetTimer( &param );
}
#endif

u32_t mxStartMsTimer3( const s8_t *file, u32_t line, const s8_t *caller, u32_t millsec, timerCallBack *Func, void* para, bool bLoop )
{
	timerPara_t param;
	INIT_TIMER_PARA( param );

	param.para = para;
	param.Func = Func;
	param.paraType = PARAMETER_CALLBACK;
	param.interval = millsec;
    	param.file = (s8_t *)file;
    	param.line = line;
    	param.caller = (s8_t *)caller;
	
	return mxSetTimer( &param );
}



bool cancelMsTimer( u32_t timerId, const s8_t *file, u32_t line, const s8_t *caller )
{
	return mxCancelTimer( timerId, file, line, caller );
}

