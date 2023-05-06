#ifndef __SD_DATABASE_H__
#define __SD_DATABASE_H__
#include "mxDef.h"
#include "service_interface.h"



class sdDatabase
{
public:
	sdDatabase();
	~sdDatabase();
    u32_t addOfferService(struct offerservice_entry *service);
    u32_t updateOfferService(struct offerservice_entry *service);
    u32_t delOfferService(struct offerservice_entry *service);

    u32_t addFindService(struct findservice_entry *service);
    u32_t updateFindService(struct findservice_entry *service);
    u32_t delFindService(struct findservice_entry *service);
private:
};
#endif
