#ifndef __MXSTATEMACHINEBASE_H__
#define __MXSTATEMACHINEBASE_H__
#include "mxDef.h"
#include "mxLog.h"



template<class O, class S, int M>
class mxStateMachineBase
{
public:
	mxStateMachineBase(O* _owner, S** _states) : owner(_owner), states(_states), current(states[0]) 
	{
		current->enter(_owner, this);
	}
	virtual ~mxStateMachineBase() 
	{
		current->exit(owner, this);
	}

	virtual s32_t OnEvent(tMsg* msg){return 0;};
	virtual void setState(S* _new)
	{
		if (_new==NULL)
		{
			mxLogFmt(_CRT_ERR,LOG_MX, "state is NULL!\n");
			return;
		}
		if (current==_new)
			return;
		current->exit(owner, this);
		current = _new;
		current->enter(owner, this);
	}

	O* getOwner() { return owner; }
	S* CurrentState() { return current; }
	virtual S*  getState(u32_t _id) { return (_id<M)?states[_id]:NULL; }
	virtual bool isCurrentState(u32_t _id) { return (current==states[_id]); }
	virtual void setState(u32_t id) { setState(getState(id)); }
	mxStateMachineBase &operator=(mxStateMachineBase &newFsm){return *this;}

protected:
	O*			owner;
	S** 		states; 	// need to be 
	S*			current;	// current state
};









#endif

