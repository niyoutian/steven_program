#ifndef __MXTIMER_H__
#define __MXTIMER_H__
#include "mxMutex.h"
#include "mxLog.h"


#define INVALID_TIMER_ID                        0xffffffff


#define TIMER_PRICISION     6        // 6tick 
#define TIMER_TTL           100 // timer ttl time
#define TVN_BITS 			6
#define TVR_BITS 			8
#define TVN_SIZE 			(1 << TVN_BITS)
#define TVR_SIZE 			(1 << TVR_BITS)
#define TVN_MASK 			(TVN_SIZE - 1)
#define TVR_MASK 			(TVR_SIZE - 1)
#define TVN_NUM             5

// TIMERID passed to application consists of [ counter | timerId ]
#define MAX_TIMERID_BITS	26
#define MAX_TIMERID			((0x1<<MAX_TIMERID_BITS)-1)
#define GET_TIMERID(x)		((x) & MAX_TIMERID)
#define GET_COUNTER(x)		(((x)>>MAX_TIMERID_BITS)&0xf)

#define NOOF_TVECS ( sizeof(tvecs)/sizeof(tvecs[0]) )

#define INIT_LIST_HEAD(ptr) do{ (ptr)->next = (ptr); (ptr)->prev = (ptr); }while(0)

								

//declare paramter enum type for which function call set timer
typedef enum {
	PARAMETER_MSG_POINTER=1,
	PARAMETER_MSG_OID,
	PARAMETER_CALLBACK,
}timerPara_e;


//declare callback function prototype
typedef void timerCallBack( void *param );

//declare the input prototype of parameter
typedef struct timerPara_s{
	u8_t   	paraType;		//which function call set timer interface

	u32_t 	interval;			//setted timer interval
	//tMsg* 	pMsg;			//the parameter is pointer

	u32_t 	data;			//the paramter is dstOid msgid and data1 of tmsg structure
	u32_t  	msgId;
	//oid_t   	dstOid;

	void * 	para;			//the parameter is callback funtion adddress and para pointer

	//CallTraceId traceId; // can not be packed
	u32_t traceId;

	timerCallBack* Func;
	//MsgQueuePriorityLevel_e priorityLevel;
	s8_t *file;
	s32_t line;
	s8_t *caller;
}__attribute__((packed)) timerPara_t;

//declare time block structure
typedef struct timerBlock
{
	struct timerBlock *prev;  //the pointer that pointed previou timerblock
	struct timerBlock *next; //the pointer that pointed next timerblock
	u32_t vec:8;		//which timerVec
	u32_t list:8;		// which list
	u32_t timerId:16;	
	u32_t settedTick;
	u64_t tick;
	u8_t   paraType;
#ifdef TIMER_RETRY
	u16_t timeToLeft;
#endif // TIMER_RETRY
	//tMsg  *pMsg;			//when the timer expire,send this message
	
	u32_t data;
	u32_t msgId;
	//oid_t  dstOid;

	// it has to be CallTraceId here, as I have to init it
	//CallTraceId traceId; // can not be packed
	u32_t traceId;
	void  *para;			//timer parameter
	timerCallBack *Func;
	//MsgQueuePriorityLevel_e priorityLevel;
	void *whichPan;
	u16_t whichList;
	s8_t *file;
	s32_t line;
	s8_t *caller;
}__attribute__ ((packed))  timerBlock_t;

#define INIT_TIMER_BLOCK(_block) do {\
								memset(_block, 0, sizeof(timerBlock_t));\
                               			 		_block->traceId = 0; \
								} while(0)

#define INIT_TIMER_PARA( _para )  do { \
                                    memset( &_para, 0, sizeof(timerPara_t) ); \
                                    } while(0)

struct timerVec 
{
	u16_t index;
	u32_t num;
	struct timerBlock vec[TVN_SIZE];
};

struct timerVecRoot 
{
	u16_t index;
	u32_t num;
	struct timerBlock vec[TVR_SIZE];
};

//typedef eArray<timerBlock_t , eArray_REG_SYSTEM> arrTimerBlock_t;

class mxTimerMgr
{
public:
	mxTimerMgr();
	~mxTimerMgr();	
	static mxTimerMgr* getInstance(void);
	void StartTimer(void);
	void Stop(void);
	u32_t getClkRate(void);
	u32_t setTimer( timerPara_t *param );
	bool cancelTimer( u32_t TimerId, const s8_t *file, u32_t line, const s8_t *caller );
	u64_t getCurrentTick(void);  
	u64_t getCurrentTickNoSema(void);
	u32_t getTmBlkNum(void);
	void runTimerList(void);
private:
	static void* mainThreadEntry(void* param);
	void timerTask(void);
	void taskDelay( u32_t ms );
	void  listAdd( timerBlock_t *newTimer, timerBlock_t *prev, timerBlock_t *next ) ;
	void  listDel( timerBlock_t *pEntry ) ;
	timerBlock_t* listSearch(timerBlock_t * head ,u16_t timerId);
	timerBlock_t* allocTmBlk(u32_t &timerId);
	timerBlock_t* getTmBlkByTimerId(u32_t timerId);
   	void  returnTmBlk( timerBlock_t *pTmBlk ); 
	void internalAddTimer( timerBlock_t *timer );
	void reviseBaseTick(void);
	u32_t msToTick( u32_t msCount );
	u32_t tickToMs( u32_t tick );
	void cascadeTimers( timerVec *tv );

	
	static mxTimerMgr*	 	mpInstance;
	mxMonitoredMutex  		mMutex;
	pthread_t          		mThreadId;
	u64_t    				mTickBase;
	u64_t   				mBaseTick;
	unsigned long   			mTickLast;
	bool              	 		mStatus;
	bool                			mRevise;
	u32_t               		discardNum;
	u32_t 			    	mActiveTimerCount ;
#ifdef TIMER_RETRY
	u32_t  				reorderTimerCount;
	u32_t  				dropTimerCount;
#endif // TIMER_RETRY
	u32_t  				mKilledTimerCount;
	struct timerVec tv5;
	struct timerVec tv4;
	struct timerVec tv3;
	struct timerVec tv2;
	struct timerVecRoot tv1;
	struct timerVec * tvecs[TVN_NUM];

	 long  				mClkRate;
};




extern "C" u32_t mxSetTimer(timerPara_t *param );
extern "C" bool mxCancelTimer( u32_t id, const s8_t *file, u32_t line, const s8_t *caller );
extern "C" void mxStartTimer(void);
extern "C" void mxStopTimer(void);



extern "C" u32_t mxStartMsTimer3( const s8_t *file, u32_t line, const s8_t *caller, u32_t millsec, timerCallBack *pFun, void* Para, bool bLoop = false );
#define mavStartMsTimer3( args... ) \
    mxStartMsTimer3(__FILE__, __LINE__, __FUNCTION__, args)

extern "C" bool cancelMsTimer( u32_t timerId,const s8_t *file, u32_t line, const s8_t *caller );
#define mavCancelMsTimer( id ) \
	cancelMsTimer(id, __FILE__, __LINE__, __FUNCTION__)

#endif
