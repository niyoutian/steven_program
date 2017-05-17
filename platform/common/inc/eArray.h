#ifndef __E_ARRAY_H__
#define __E_ARRAY_H__
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <typeinfo>
#include "mxDef.h"

#define NODES_PER_BLOCK             (0x1<<(SIZE&0xFFFF))
#define MAX_ALLOCATIONS				0x00FFFFFF
#define GROW_BLOCKS_LENGTH			64
#define TYPE_NAME_LEN 				32 

/* Size by which Array is Grown on Insertion */
#define BITARCH_32

#ifdef BITARCH_32
typedef enum eArraySize {
	 eArray_4				= 0x00030002
	,eArray_8				= 0x00070003
	,eArray_16				= 0x000F0004	
	,eArray_32				= 0x001F0005
	,eArray_64				= 0x003F0006
	,eArray_128				= 0x007F0007
	,eArray_256				= 0x00FF0008
	,eArray_512				= 0x01FF0009
	,eArray_1024			= 0x03FF000A
	,eArray_2048			= 0x07FF000B
	,eArray_4096			= 0x0FFF000C
} eArraySize_t;
#endif // BITARCH_32
#ifdef BITARCH_64
typedef enum eArraySize {
	 eArray_4				= 0x00030002
	,eArray_8				= 0x00070003
	,eArray_16				= 0x000F0004
	,eArray_32				= 0x001F0005
	,eArray_64				= 0x003F0006
	,eArray_128				= 0x007F0007
	,eArray_256				= 0x00FF0008
	,eArray_512				= 0x01FF0009
	,eArray_1024			= 0x03FF000A
	,eArray_2048			= 0x07FF000B
	,eArray_4096			= 0x0FFF000C
	,eArray_8192			= 0x1FFF000D
	,eArray_16384			= 0x3FFF000E
	,eArray_32768			= 0x7FFF000F
} eArraySize_t;
#endif // BITARCH_64

/////////////////////////////////////////////////////////////////////////////////
#define IS_FREE_INDICATOR	(0x80000000)
#define CLR_FREE_INDICATOR	(0x7FFFFFFF)
#define IS_FREE(id)			(id&IS_FREE_INDICATOR)
#define SET_FREE(id)		(id|=IS_FREE_INDICATOR)
#define RESET_FREE(id)		(id&=CLR_FREE_INDICATOR)
// eANode is not accessible from the outside world by design.
// Only access is through eAIterator.
template < class TYPE >
class eANode
{
public:
	eANode();
	~eANode();
	eANode< TYPE >* remove();					// Pop this from current
	void insert_after(eANode< TYPE >* loc);	// Insert loc after this
	void insert_before(eANode< TYPE >* loc);	// Insert loc before this

 	inline eANode< TYPE >* next() const { return mpNext; }
 	inline eANode< TYPE >* prev() const { return mpPrev; }
	inline TYPE&           data() const { return * (TYPE *) mData; }
	inline TYPE*           data_ptr() const { return (TYPE *) mData; }
	
	inline static eANode< TYPE >* nodeOf(TYPE* p) { 
		return ((eANode< TYPE >*) ((char*)p-offsetof(eANode< TYPE >,mData))); 
	}

	inline u32_t getId() const { return mId; } 
	inline void setId(u32_t id) { mId = (id | IS_FREE_INDICATOR); }
	inline bool getIsFree() { return (IS_FREE(mId) != 0); }
	inline void setIsFree(bool setFree) { if (setFree) SET_FREE(mId); else RESET_FREE(mId); }
	inline bool empty() const { return (mpNext==this); }
	inline void create();
	inline void create(const TYPE& _obj);
	inline void destroy();	
	
private:
	u32_t   			mId;	// to identify when removed from free list 
	eANode< TYPE >*		mpNext;	// for free list and allocated list
	eANode< TYPE >*		mpPrev;	// for free list and allocated list
	s8_t				mData[sizeof(TYPE)];

};

template < class TYPE >
eANode< TYPE >::eANode() : mId(0),mpNext(this),mpPrev(this)
{
	
}

template < class TYPE >
eANode< TYPE >::~eANode()
{
	
}

// Insert loc before this
template < class TYPE >
void eANode< TYPE >::insert_before(eANode< TYPE >* loc)
{
	loc->mpNext = this;
	loc->mpPrev = mpPrev;
	mpPrev->mpNext = loc;
	mpPrev = loc;
}

// Pop self from array
template < class TYPE >
eANode< TYPE >* eANode< TYPE >::remove()
{
	mpNext->_prev = mpPrev;
	mpPrev->mpNext = mpNext;	
	mpNext = mpPrev = this;
	return this;
}


// Insert _loc after this
template < class TYPE >
void eANode< TYPE >::insert_after(eANode< TYPE >* loc)
{
	loc->mpNext = mpNext;
	loc->mpPrev = this;
	mpNext->mpPrev = loc;
	mpNext = loc;
}

template < class TYPE >
void eANode< TYPE >::create()
{
	new (mData) TYPE();
}

template < class TYPE >
void eANode< TYPE >::create(const TYPE& obj )
{
	new (mData) TYPE( obj );
}

template < class TYPE >
void eANode< TYPE >::destroy()
{
	((TYPE*)(mData))->TYPE::~TYPE();
}



///////////////////////////////////////////////////////////////////////////////

template < class TYPE, u32_t SIZE >
class eABlock
{
public:
	eABlock(u32_t id);
	inline ~eABlock();

	u32_t initialize(eABlock< TYPE, SIZE >*& blocks, eANode< TYPE >& freeRoot);

public:
	inline eANode< TYPE >& operator[](u32_t i) { return mNodes[i]; }
private:
	eANode< TYPE > 		mNodes[ NODES_PER_BLOCK ];
};

template < class TYPE, u32_t SIZE >
eABlock< TYPE, SIZE >::eABlock(u32_t id)
{
	u32_t max = NODES_PER_BLOCK;
	for(u32_t i=0; i< max; i++)
		mNodes[i].setId(id++);
}

template < class TYPE, u32_t SIZE >
eABlock< TYPE, SIZE >::~eABlock()
{
	
}

template < class TYPE, u32_t SIZE >
u32_t eABlock< TYPE, SIZE >::initialize(eABlock< TYPE, SIZE >*& blocks, eANode< TYPE >& freeRoot)
{
	// Add the new nodes to the free array
	u32_t max = NODES_PER_BLOCK;
	for(u32_t idx=0; idx < max; idx++)
		freeRoot.insert_before(&mNodes[idx]);
	return max;
}



///////////////////////////////////////////////////////////////////////////////

#define IF_IS_VALID_INDEX(i, b, n)			\
	register unsigned int b;				\
	register unsigned int n;				\
	if (i<mCapacity) {						\
		b=(i>>(mAllocSize&0xFFFF));		\
		n=(i&(mAllocSize>>16));			\
		if (b<mBlocksCnt) { 				\


#define ELSE_ON_INVALID_INDEX()				\
		}									\
	}	


template < class TYPE, unsigned int SIZE >
class eArray
{
public:
	typedef eABlock< TYPE, SIZE >*		block_type_ptr;
	typedef eANode< TYPE >				node_type;

	eArray();
	~eArray();
	void gen_free_nodes(void);
	eANode< TYPE >* get_free_node();
	eANode< TYPE >* alloc_node(unsigned int idx);
	TYPE* alloc(unsigned int* idx = NULL);
	TYPE* alloc(const TYPE& object, unsigned int* idx = NULL);
	TYPE* alloc(unsigned int idx);
	TYPE* alloc(const TYPE& object, unsigned int idx);
	void dealloc(unsigned int idx);
	void dealloc(TYPE* object);

private:
    s8_t            		mClassName[TYPE_NAME_LEN];
	u32_t					mAllocSize;
    u32_t    				mTypeSize;
    u32_t    				mNodesPerBlock;
	u32_t					mSize;
	u32_t					mCapacity;
	u32_t    				mBlocksCnt;
	u32_t					mBlocksMax;
	eABlock< TYPE, SIZE >**	mpBlocks;	// array of allocated blocks nodes ...
	eANode< TYPE >			mFreeRoot;	// Root node of double link-array of free nodes
	eANode< TYPE >			mDataRoot;	// Root node of double link-array of data nodes

};

template < class TYPE, unsigned int SIZE >
eArray< TYPE, SIZE >::eArray() : 
	mAllocSize(SIZE), mTypeSize(sizeof(TYPE)),mNodesPerBlock(SIZE), mSize(0), mCapacity(0), mBlocksCnt(0), 
	mBlocksMax(0)
{
	gen_free_nodes();
    memset(mClassName, 0, TYPE_NAME_LEN);
    strncpy(mClassName, typeid(TYPE).name(), TYPE_NAME_LEN-1);
}

template < class TYPE, u32_t SIZE >
void eArray< TYPE, SIZE >::gen_free_nodes(void) 
{
	assert((SIZE & 0xFFFF0000) != 0);
	if (mBlocksCnt==mBlocksMax)
	{
		if ((mCapacity+NODES_PER_BLOCK)>MAX_ALLOCATIONS)
			return;
		block_type_ptr*	curr_blocks = mpBlocks;
		block_type_ptr*	new_blocks = NULL;
		unsigned int curr_max = mBlocksMax;
		unsigned int new_max = mBlocksMax + GROW_BLOCKS_LENGTH;
		new_blocks = (block_type_ptr*) malloc(new_max*sizeof(block_type_ptr));
		if (new_blocks==NULL)
			return;
		if (curr_blocks)
			memcpy(new_blocks, curr_blocks, curr_max*sizeof(block_type_ptr));

		memset(new_blocks+curr_max, 0, (sizeof(block_type_ptr)*GROW_BLOCKS_LENGTH));

		mpBlocks = new_blocks;
		if (curr_blocks)
			free(curr_blocks);
		mBlocksMax = new_max;
	}
	mpBlocks[mBlocksCnt] = new eABlock< TYPE, SIZE >(mCapacity);
	mCapacity += mpBlocks[mBlocksCnt]->initialize(mpBlocks[mBlocksCnt], mFreeRoot);
	mBlocksCnt++;
}

template < class TYPE, u32_t SIZE >
eANode< TYPE >* eArray< TYPE, SIZE >::get_free_node()
{
	if (mFreeRoot.empty())  {
		gen_free_nodes();
		if (mFreeRoot.empty())
			return NULL;
	}

	eANode< TYPE >* tmp = mFreeRoot.next()->remove();
	tmp->setIsFree(false);
	return tmp;
}

template < class TYPE, unsigned int SIZE >
eANode< TYPE >*  eArray< TYPE, SIZE >::alloc_node(unsigned int idx)
{
	
}


template < class TYPE, u32_t SIZE >
TYPE* eArray< TYPE, SIZE >::alloc(u32_t* idx)
{
	eANode< TYPE >* tmp = get_free_node();

	if (tmp==NULL)
		return NULL;

	try {
		tmp->create();
	} catch(...) {
		mFreeRoot.insert_before(tmp);
		throw;
	}
	mSize++;

	mDataRoot.insert_before(tmp);

	if (idx)
		*idx = tmp->getId();

	return tmp->data_ptr();
}

//IMPORTANT!!
//This API is ONLY for replication !!
template < class TYPE, u32_t SIZE >
TYPE* eArray< TYPE, SIZE >::alloc(u32_t idx)
{
	eANode< TYPE >* tmp = alloc_node(idx);
	
	if(tmp != NULL) {
		tmp->create();
		return tmp->data_ptr();
	} else {
		return NULL;
	}
}

template < class TYPE, u32_t SIZE >
TYPE* eArray< TYPE, SIZE >::alloc(const TYPE& object, u32_t idx)
{
	eANode< TYPE >* tmp = alloc_node(idx);
	
	if(tmp != NULL) {
		tmp->create(object);
		return tmp->data_ptr();
	} else {
		return NULL;
	}
}

template < class TYPE, u32_t SIZE >
TYPE* eArray< TYPE, SIZE >::alloc(const TYPE& object, u32_t* idx)
{
	eANode< TYPE >* tmp = get_free_node();

	if (tmp==NULL)
		return NULL;

	try {
		tmp->create(object);
	} catch(...) {
		mFreeRoot.insert_before(tmp);
		throw;
	}
	mSize++;

	mDataRoot.insert_before(tmp);

	if (idx)
		*idx = tmp->getId();

	return tmp->data_ptr();
}

template < class TYPE, unsigned int SIZE >
void eArray< TYPE, SIZE >::dealloc(unsigned int idx)
{
	IF_IS_VALID_INDEX(idx, block, node)
		set_free_node(&(*(mpBlocks[block]))[node]);
		return;
	ELSE_ON_INVALID_INDEX()
}

template < class TYPE, unsigned int SIZE >
void eArray< TYPE, SIZE >::dealloc(TYPE* object)
{
	set_free_node(node_type::nodeOf(object));
}


#endif
	
