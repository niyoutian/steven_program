#ifndef __MX_HASH_TABLE_H__
#define __MX_HASH_TABLE_H__
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <typeinfo>
#include "mxDef.h"
#include "eArray.h"

#define EANODE_SUPPORT	1
#define HASH_STR_CONSTANT 31

class hashElement
{
public:
	hashElement(s32_t key,void* data);
	hashElement(char* key,s32_t len, void* data);
	hashElement(s32_t key1,s32_t key2,void* data);
	// void*(long) as key
	hashElement(void* key, void* data); 
	 ~hashElement();
	bool isSameKey(s32_t key);
	bool isSameKey(s32_t key,void*pValue);
	bool isSameKey(char* key,s32_t len);
	bool isSameKey(s32_t key1,s32_t key2);
	// void*(long) as key
	bool isSameKey(void* key);
	
	void* getData(){ return pData;}
	s32_t getKey(){return nKey;}
	s32_t getKey1(){return nKey1;}
	s32_t getKeyLen(){return nLen;}
	void* getPtrKey(){return ptrKey;}
	char* getStrKey(){return pKey;}
	bool isOnlyOne(){ return (pPrev==this);}
#if EANODE_SUPPORT
	void setNode( eANode<hashElement*> *pNode) {pANode = pNode;}
	void destroyNode() 
	{ 
		pANode->remove();
		pANode->destroy();
		delete pANode ; 
	}
#endif
	hashElement* pPrev;
	hashElement* pNext;

private:
	s32_t	nKey;
	s32_t	nKey1;
	void* 	pData;
	s32_t	nLen;
	char* 	pKey;
	void* 	ptrKey;
#if EANODE_SUPPORT
	eANode<hashElement*> *pANode;
#endif
};

typedef enum{
	INT32Type,
	INT64Type,
	StringType,
	MainCCType,
	PointerType
}HashType_e;

class mxHashTable
{
public:
	mxHashTable(u32_t numberOfBin,s32_t hashType,s32_t duplicateNum=0);
	~mxHashTable();
	bool insert(s32_t key,void* pData);
	bool insert(s32_t key1,s32_t key2,void* pData);
	bool insert(char* pKey,int len,void* pData);
	// void*(long) as key
	bool insert(void* key, void* data);

	void * remove(s32_t key);
	void * remove(s32_t key1,s32_t key2);
	void * remove(char* pKey,s32_t len);
	void * remove(void* key);

	void * find(s32_t key,bool& isFound);
	void * find(s32_t key1,s32_t key2,bool& isFound);
	void * find(char* pKey, s32_t len,bool& isFound);
	void * find(void* key, bool& isFound);
	
	void * find(s32_t key);
	void * find(s32_t key1,s32_t key2);
	void * find(char* pKey, s32_t len);
	void * find(void* key);
	
	bool  insertToBin(s32_t index,hashElement* pElm);
	u32_t size() { return mNumberOfElement; }
	void clear(void);
	void showHashTable(void);
protected:
	s32_t  findBinIndex(s32_t key);
	s32_t  findBinIndex(char*pKey,int len);
	s32_t  findBinIndex(int key1,int key2);
	// ptr(long)
	s32_t  findBinIndex(void* key);

  	hashElement** mpBin;	
	int   	mBinNumber;
	int   	mDuplicateNumber;
	int   	mHashType;
	u32_t   mNumberOfElement;
	int   	mBitMask;
private:
#if EANODE_SUPPORT
	eANode<hashElement*> dataRoot;
#endif
};




#endif

