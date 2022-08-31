#ifndef __MX_SEM_H__
#define __MX_SEM_H__
#include <semaphore.h>
#include "mxDef.h"


class mxSem
{
public:
	mxSem(u32_t value = 1);
	~mxSem();

	void post(void);
	void wait(void);
private:
	sem_t  mSem;
};


#endif
