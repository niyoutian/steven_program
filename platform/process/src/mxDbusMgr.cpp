#include <unistd.h>
#include "mxLog.h"
#include "mxDbusMgr.h"
#include "mxDbusMgr.h"


mxDbusMgr::mxDbusMgr()
{
	mpConnect = NULL;
}

mxDbusMgr::~mxDbusMgr()
{
	if (mpConnect) {
        dbus_connection_close(mpConnect);
        mpConnect = NULL;
    }
}

 /**
 * @brief		init dbus session
 * @details	    This is the detail description. 
 * @param[in]	req_name well known bus name.
 * @retval		STATUS_SUCCESS		成功
 * @retval		STATUS_FAILED	错误 
 * @par 标识符
 * 		保留
 * @par 其它
 * 		无
 * @par 修改日志
 * @note 
 */
u32_t mxDbusMgr::initDbus(char *req_name)
{
    DBusError err;
    DBusConnection *connection = NULL;

    dbus_error_init(&err);
    /*  创建 session D-Bus */
    connection = dbus_bus_get_private(DBUS_BUS_SESSION, &err);

	if (NULL == connection) {
        mxLogFmt(LOG_ERR,"create dbus session failed\n");
        return STATUS_FAILED;
	}
	if (dbus_error_is_set(&err)) {
        mxLogFmt(LOG_ERR,"create dbus session failed:%s\n",err.message);
		dbus_error_free(&err);
        return STATUS_FAILED;
	}

    mNniqueName = dbus_bus_get_unique_name(connection);

    if (req_name != NULL) {
        int ret = 0;
        dbus_error_init(&err);
        ret = dbus_bus_request_name(connection, req_name, DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &err);
        if (dbus_error_is_set(&err)) {
            mxLogFmt(LOG_ERR,"dbus_bus_request_name failed:%s\n",err.message);
            dbus_error_free(&err);
            dbus_connection_close(connection);
            return STATUS_FAILED;
        }

        if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
            mxLogFmt(LOG_ERR,"Not Primary Owner(%d:%s)\n",ret, req_name);
            dbus_connection_close(connection);
            return STATUS_FAILED;
        }

        mWellKnownName = req_name;
    }

    mpConnect = connection;

    return STATUS_SUCCESS;
}

void  mxDbusMgr::deInitDbus(void)
{

}

u32_t mxDbusMgr::startDbusService(void)
{
	mMutex.lock();
    if ( mThreadId ) {
		mxLogFmt(LOG_ERR, "the DBUS thread has been started!\n");
        mMutex.unlock();
		return STATUS_FAILED;
	}
	mStatus = true;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create( &mThreadId, &attr, mainThreadEntry, this );
    mxLogFmt(LOG_ERR, "before wait!\n");
	mMutex.wait();
     mxLogFmt(LOG_ERR, "after wait!\n");
	mMutex.unlock();

    return STATUS_SUCCESS;
}

void* mxDbusMgr::mainThreadEntry(void* param)
{
	mxDbusMgr *pEngine = (mxDbusMgr*) param;
    
    pEngine->runDbusTask();
	return NULL;
}

void mxDbusMgr::runDbusTask(void)
{
    DBusMessage* msg = NULL;
    const char * sender = NULL;
    const char * dst = NULL;
    u_int32_t serial = 0;
    const char * path = NULL;
    const char * intf = NULL;
    const char * name = NULL;

    mxLogFmt(LOG_DEBUG,"start dbus task with pthread id=%d\n", mThreadId);

	mMutex.lock();
	mMutex.signal();
	mMutex.unlock();

    while (mStatus) {
        dbus_connection_read_write(mpConnect, 0);
        msg = dbus_connection_pop_message(mpConnect);
        if (NULL == msg) {
            usleep(1000*10);
            continue;
        }
        sender = dbus_message_get_sender(msg);
        dst = dbus_message_get_destination(msg);
        serial = dbus_message_get_serial(msg);
        path = dbus_message_get_path(msg);
        intf = dbus_message_get_interface(msg);
        name = dbus_message_get_member(msg);
        mxLogFmt(LOG_DEBUG,"sender:%s dst:%s serial:%d path:%s intf:%s name:%s\n", sender,dst,serial,path,intf,name);
        dbus_message_unref(msg);
    }

}

u32_t mxDbusMgr::registerDbusMethod(dbus_method_t *method)
{
    vector<dbus_method_t *>::iterator iter;
    for(iter=mMethodArray.begin(); iter!=mMethodArray.end(); iter++){

    }
    return 0;
}

 /**
 * @brief		add dbus watch event
 * @param[in]	rule watch rule.
 * @retval		STATUS_SUCCESS		成功
 * @retval		STATUS_FAILED	错误 
 * @par 标识符
 * 		保留
 * @par 其它
 * 		无
 * @par 修改日志
 * @note 
 */
u32_t mxDbusMgr::addDbusWatch(const char *rule)
{
    DBusError dbus_error;	
    dbus_error_init(&dbus_error); 

    dbus_bus_add_match(mpConnect, rule, &dbus_error);

    return STATUS_SUCCESS;
}

u32_t mxDbusMgr::sendDbusData(char *name, char *path, char *inter, char *method, u8_t data, u32_t len)
{
    return 0;
}