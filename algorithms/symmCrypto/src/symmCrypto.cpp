#include <unistd.h>
#include <string.h>
#include "symmCrypto.h"


mxSymmEnc::mxSymmEnc()
{
	mpEnc = NULL;
}

mxSymmEnc::~mxSymmEnc()
{
	if(mpEnc)
	{
		delete(mpEnc);
		mpEnc = NULL;
	}
}


