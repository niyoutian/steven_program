#include "mxHashTable.h"
#include "mxLog.h"


hashElement::hashElement(int key,void* data) :
	pPrev(this), pNext(this), nKey(key), nKey1(0), pData(data), nLen(0), pKey(0), ptrKey(NULL)//,pANode(NULL)
{
#if EANODE_SUPPORT
	pANode = NULL;
#endif	
}

hashElement::hashElement(char* key,int len,void* data) :
	pPrev(this), pNext(this), nKey(0), nKey1(0), pData(data), nLen(0), pKey(0), ptrKey(NULL)//,pANode(NULL)
{
	if ( len )
	{
		pKey = (char*)malloc(len);//new char[len];
		memcpy(pKey,key,len);
		nLen = len;
	}
	else
	{
		pKey = 0;
		nLen = 0;
	}
#if EANODE_SUPPORT
		pANode = NULL;
#endif
}

hashElement::hashElement(int key1,int key2,void* data) :
	pPrev(this), pNext(this), nKey(key1), nKey1(key2), pData(data), nLen(0), pKey(0), ptrKey(NULL)//,pANode(NULL)
{
#if EANODE_SUPPORT
	pANode = NULL;
#endif	
}

// ptr key support
hashElement::hashElement(void* key, void* data) :
	pPrev(this), pNext(this), nKey(0), nKey1(0), pData(data), nLen(0), pKey(0), ptrKey(key)
{
#if EANODE_SUPPORT
	pANode = NULL;
#endif	
}

hashElement::~hashElement()
{

	if ( pPrev!= this)
	{
		pPrev->pNext = pNext;
		pNext->pPrev = pPrev;
	}
	if ( pKey )
		delete[] pKey;
}

bool hashElement::isSameKey(s32_t key)
{
	if ( !pKey && (key == nKey))
		return true;
	return false;
}

bool hashElement::isSameKey(s32_t key,void* pValue)
{
	if ( !pKey && (key == nKey)&&(pValue==pData))
		return true;
  	return false;
}

bool hashElement::isSameKey(char* key,s32_t len)
{
	if( pKey )
	{
		if ( len != nLen )
			return false;
		return !memcmp((const void*)key,(const void*)pKey,len);
	}
	return false;
}

bool hashElement::isSameKey(s32_t key1,s32_t key2)
{
	if ( !pKey && (key1 == nKey)&&(key2==nKey1))
		return true;
	return false;
}

// ptr key
bool hashElement::isSameKey(void* key)
{
    return ( (ptrKey != NULL) && (ptrKey == key) );
}

mxHashTable::mxHashTable(u32_t numberOfBin,s32_t hashType,s32_t duplicateNum) : 
	mpBin(NULL), mBinNumber(numberOfBin), mDuplicateNumber(duplicateNum), 
	mHashType(hashType), mNumberOfElement(0), mBitMask(0)
{
	mpBin = (hashElement**)malloc(sizeof(hashElement*) * numberOfBin);
	mxLogFmt(LOG_DEBUG,"mpBin %p size %d",mpBin,sizeof(mpBin));
	for(int i = 0 ; i< numberOfBin; i++)
	{
		mpBin[i] = 0;
	} 
	// check if it is power of 2.
	if ( !(numberOfBin &(numberOfBin-1)))
	{ 
		mBitMask = numberOfBin-1;
	}
	else
		mBitMask = 0;
}

mxHashTable::~mxHashTable()
{
	clear();
	if ( mpBin )
	{    
		free(mpBin);
		mpBin = NULL;
	} 
}

bool mxHashTable::insertToBin(s32_t index,hashElement* pEle)
{
	if ( !pEle )
		return false;
	if ( mpBin[index] )
	{
		hashElement* pOld = mpBin[index];
		pEle->pNext = pOld;
		pEle->pPrev = pOld->pPrev;
		(pOld->pPrev)->pNext = pEle;
		pOld->pPrev = pEle;
	}
	else
	{
		mpBin[index] = pEle;
	}
#if EANODE_SUPPORT
	eANode<hashElement*>* tmpNode = new  eANode<hashElement*>;
	tmpNode->create(pEle);
	pEle->setNode(tmpNode);
	dataRoot.insert_before(tmpNode);
#endif
	mNumberOfElement++;
	return true;
}


bool mxHashTable::insert(s32_t key,void *pData)
{
	if( !mDuplicateNumber )
	{
		bool isFound;
		find(key,isFound);
		if ( isFound )
			return false;
	}	
	s32_t index = findBinIndex(key);
	mxLogFmt(LOG_DEBUG,"insert index %d",index);
	if ( index != -1 )
	{
		hashElement* pEle = new hashElement(key,pData);
	  	return insertToBin(index,pEle);
	}
	return false;
}

bool mxHashTable::insert(s32_t key1, s32_t key2,void *pData)
{
	if( !mDuplicateNumber )
	{
		bool isFound;
		find(key1,key2,isFound);
		if ( isFound )
			return false;
	}	
	s32_t index = findBinIndex(key1,key2);
	if ( index != -1 )
	{
		hashElement* pEle = new hashElement(key1,key2,pData);
		//hashElement* pEle = (hashElement*)malloc(sizeof(hashElement));
	  	//new (pEle) hashElement(key1,key2, pData);
	  	return insertToBin(index,pEle);
	}
	return false;
}

bool mxHashTable::insert(char* pKey,int len,void* pData)
{
	if( !mDuplicateNumber )
	{
		bool isFound;
		find(pKey,len,isFound);
		if ( isFound )
			return false;
	}
  	s32_t index = findBinIndex(pKey,len);
	if ( index != -1)
	{
		hashElement* pEle = new hashElement(pKey,len,pData);
	//hashElement* pEle = (hashElement*)mxAlloc(sizeof(hashElement));
	//new (pEle) hashElement(pKey,len,pData);
		return insertToBin(index,pEle);
	}
	return false;
}

// void*(long) key, insert
bool mxHashTable::insert(void* key, void* data)
{
	if ( !mDuplicateNumber ) // no duplicate
	{
		bool isFound;
		find(key, isFound);
		if ( isFound )
			return false;
	}

	s32_t index = findBinIndex(key);
	if ( index != -1 )
	{
		hashElement* pEle = new hashElement(key, data);
		//hashElement* pEle = (hashElement*)malloc(sizeof(hashElement));
		//new (pEle) hashElement(key, data);
		return insertToBin(index, pEle);
	}
	return false;
}

void * mxHashTable::remove(s32_t key)
{
	s32_t index = findBinIndex(key);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(key) )
				{
					void* pData = pNext->getData();
					if ( pNext->isOnlyOne())
					{
						mpBin[index] = NULL;
					}
					else
					{
						if ( pNext == pOrg )
						{
							mpBin[index]= pNext->pNext;
						}
					}
					//pNext->destroyNode(); 
					delete pNext;
					mNumberOfElement--;
					return pData;
				}
				pNext = pNext->pNext;
			}
		}
	}
	return NULL;	
}

void * mxHashTable::remove(s32_t key1,s32_t key2)
{
	s32_t index = findBinIndex(key1,key2);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(key1,key2) )
				{
					void* pData = pNext->getData();
					if ( pNext->isOnlyOne())
					{
						mpBin[index] = NULL;
					}
					else
					{
						if ( pNext == pOrg )
						{
							mpBin[index]= pNext->pNext;
						}
					}
					//pNext->destroyNode();
					delete pNext;
					mNumberOfElement--;
					return pData;
				}
				pNext = pNext->pNext;
			}
		}
	}
	return NULL;	
}

void * mxHashTable::remove(char* pKey,s32_t len)
{
	s32_t index = findBinIndex(pKey,len);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(pKey,len) )
				{
					void *pData = pNext->getData();
					if ( pNext->isOnlyOne())
					{
						mpBin[index] = NULL;
					}
					else
					{
						if ( pNext == pOrg )
						{
							mpBin[index]= pNext->pNext;
						}
					}
					//pNext->destroyNode();
					delete pNext;
					mNumberOfElement--;
					return pData;
				}
				pNext = pNext->pNext;
			}
		}
	}
	return NULL;	
}

void * mxHashTable::remove(void* key)
{
	s32_t index = findBinIndex(key);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(key) )
				{
					void* pData = pNext->getData();
					if ( pNext->isOnlyOne())
					{
						mpBin[index] = NULL;
					}
					else
					{
						if ( pNext == pOrg )
						{
							mpBin[index]= pNext->pNext;
						}
					}
					//pNext->destroyNode();
					delete pNext;
					mNumberOfElement--;
					return pData;
				}
				pNext = pNext->pNext;
			}
		}
	}
	return NULL;	
}

void * mxHashTable::find(s32_t key,bool& isFound)
{
	s32_t index = findBinIndex(key);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(key) )
				{
					isFound = true;
					return pNext->getData();
				}
				pNext = pNext->pNext;
			}
		}
	}
	isFound = false;
	return NULL;
}

void * mxHashTable::find(s32_t key1,s32_t key2,bool& isFound)
{
	s32_t index = findBinIndex(key1,key2);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(key1,key2) )
				{
					isFound = true;
					return pNext->getData();
				}
				pNext = pNext->pNext;
			}	
		}
	}
	isFound = false;
	return NULL;
}

void * mxHashTable::find(char* pKey,s32_t len,bool& isFound)
{
	s32_t index = findBinIndex(pKey,len);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(pKey,len) )
				{
					isFound = true;
					return pNext->getData();
				}
				pNext = pNext->pNext;
			}
		}
	}
	isFound = false;
	return NULL;
}

// ptr/long
void * mxHashTable::find(void* key, bool& isFound)
{
    s32_t index = findBinIndex(key);
    if ( index != -1 )
    {
        hashElement* pOrg = mpBin[index];
        if ( pOrg )
        {
            hashElement* pNext = NULL;
            while ( pOrg != pNext )
            {
	            if ( !pNext )
	            {
	                pNext = pOrg;
	            }
	            if ( pNext->isSameKey(key) )
	            {
                    isFound = true;
	                return pNext->getData();
	            }
	            pNext = pNext->pNext;
            }
        }
    }
  
    isFound = false;
    return NULL;
}

void * mxHashTable::find(s32_t key)
{
	s32_t index = findBinIndex(key);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(key) )
				{
					return pNext->getData();
				}
				pNext = pNext->pNext;
			}
		}
	}
	return NULL;	
}

void * mxHashTable::find(s32_t key1,s32_t key2)
{
	s32_t index = findBinIndex(key1,key2);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(key1,key2) )
				{
					return pNext->getData();
				}
				pNext = pNext->pNext;
			}
		}
	}
	return NULL;
}

void * mxHashTable::find(char* pKey, s32_t len)
{
	s32_t index = findBinIndex(pKey,len);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(pKey,len) )
				{
					return pNext->getData();
				}
				pNext = pNext->pNext;
			}
		}
	}
	return NULL;
}

void * mxHashTable::find(void* key)
{
	s32_t index = findBinIndex(key);
	if ( index != -1 )
	{
		hashElement* pOrg = mpBin[index];
		if ( pOrg )
		{
			hashElement* pNext = NULL;
			while ( pOrg != pNext )
			{
				if ( !pNext )
				{
					pNext = pOrg;
				}
				if ( pNext->isSameKey(key) )
				{
					return pNext->getData();
				}
				pNext = pNext->pNext;
			}
		}
	}
	return NULL;	
}

s32_t mxHashTable::findBinIndex(s32_t key)
{
	s32_t val = 0;
	switch ( mHashType )
	{ 
		case INT32Type:
		{
			if ( mBitMask )
				val = key&mBitMask;
			else
				val = key%mBinNumber;

			if(val < 0)
				val += mBinNumber;
			return val;
		}	
		case MainCCType:
		{
			int newKey = (key<<2)+((key>>24)&0x3);
			if ( mBitMask )
				val = (newKey&mBitMask);
			else
				val= ( newKey%mBinNumber);

			if(val < 0)
				val += mBinNumber;
			return val;
		}
	}      
	return -1;
}

s32_t mxHashTable::findBinIndex(char* pKey,int len)
{
	if ( mHashType == StringType)
	{
		u32_t sum = 0;
		for( u32_t i = 0 ; i< len ; i++)
		{
			sum = (HASH_STR_CONSTANT*sum)+pKey[i];
		}
		if ( mBitMask)
		{
			return (sum&mBitMask);
		}
		else
		{
			return (sum%mBinNumber);
		}
	}
	return -1;
}

s32_t mxHashTable::findBinIndex(s32_t key1,s32_t key2)
{
  	int val =0;
  	if ( mHashType == INT64Type)
	{
		if ( mBitMask )
		{
			val = ((key1+key2)&mBitMask);
		}
		else
		{
			val = ( (key1+key2)%mBinNumber);
		}

		if(val<0)
		{
			val += mBinNumber;
		}
		return val;
	}
  	return -1;
}

// ptr/long
s32_t mxHashTable::findBinIndex(void* key)
{
    if ( PointerType != mHashType )
        return -1;

    int val;
    int newKey = ((long)key<<2)+(((long)key>>24)&0x3);
    if ( mBitMask )
	    val = (newKey&mBitMask);
    else
	    val = ( newKey%mBinNumber);
	if(val < 0)
	{
		val += mBinNumber;
	}
    return val;
}

void mxHashTable::clear(void)
{
	hashElement* pCur=NULL;
	hashElement* pTmp=NULL;
 	if( mpBin )
    {
		for(int i = 0 ; i< mBinNumber ; i++)
		{
			if ( mpBin[i] )
			{
				pCur=mpBin[i]->pPrev;
				while(pCur != mpBin[i])
				{
					pTmp = pCur->pPrev;
					//pCur->destroyNode();
					pCur->hashElement::~hashElement();
					//mxDealloc((u8_t*)pCur);
					delete pCur;
					mNumberOfElement--;
					pCur= pTmp;
				}
				//mpBin[i]->destroyNode();
				mpBin[i]->hashElement::~hashElement();
				//mxDealloc((u8_t*)pBin[i]);
				delete mpBin[i];
				mNumberOfElement--;
				mpBin[i] = NULL;	
			}
		}
    }
}

void mxHashTable::showHashTable(void)
{
	for(u32_t i = 0; i < mBinNumber; i++)
	{
		mxLogFmt(LOG_DEBUG,"i %d addr %p value %08x",i,&mpBin[i],mpBin[i]);
		hashElement* pOld = mpBin[i];
		while(pOld)
		{
			mxLogFmt(LOG_DEBUG,"list %p %d",pOld->getData(),*(int *)(pOld->getData()));
			pOld = pOld->pNext;
			if(pOld == mpBin[i])
				break;
		}
	}
}

