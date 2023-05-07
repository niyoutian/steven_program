#include "mxStatus.h"
#include "serviceDatabase.h"

serviceDatabase::serviceDatabase()
{
}

serviceDatabase::~serviceDatabase()
{
}


 /**
 * @brief		add entry
 * @details	    add service entry to database 
 * @param[in]	service input service config.
 * @retval		STATUS_SUCCESS	 success
 * @retval		STATUS_EXIST	 already exist
 * @retval		STATUS_ERR_MALLOC	 malloc failed
 * @note 
 */
u32_t serviceDatabase::addEntry(someip_service_cfg_t *service)
{
    someip_service_entry_t *entry = NULL;

    mServiceListMutex.lock();
    if (!mServiceList.empty()) {
        list<someip_service_entry_t *>::iterator it;
        for (it = mServiceList.begin(); it != mServiceList.end(); it++) {
            entry = *it;
            if ((service->s_key.service_id == entry->cfg.s_key.service_id) &&
                (service->s_key.instance_id == entry->cfg.s_key.instance_id) ) {
                mServiceListMutex.unlock();
                return STATUS_EXIST;
            }
        }
    }

    entry =(someip_service_entry_t *)malloc(sizeof(someip_service_entry_t));
    if (entry==NULL) {
        mServiceListMutex.unlock();
        return STATUS_ERR_MALLOC;
    }
    memset(entry, 0, sizeof(*entry));
    memcpy(&entry->cfg, service, sizeof(*service));

    mServiceList.push_back(entry);
    mServiceListMutex.unlock();

    return STATUS_SUCCESS;
}

u32_t serviceDatabase::addMethod(u16_t service_id, u16_t instance_id, someip_method_t *method)
{
    mServiceListMutex.lock();
    if (mServiceList.empty()) {
        mServiceListMutex.unlock();
        return STATUS_FAILED;
    }

    list<someip_service_entry_t *>::iterator it;
    someip_service_entry_t *entry = NULL;
    someip_service_entry_t *find_service = NULL;

    for (it = mServiceList.begin(); it != mServiceList.end(); it++) {
        entry = *it;
        if ((service_id == entry->cfg.s_key.service_id) &&
            (instance_id == entry->cfg.s_key.instance_id) ) {
            find_service = entry;
            break;
        }
    }
    if (find_service == NULL) {
        mServiceListMutex.unlock();
        return STATUS_FAILED;
    }

    someip_method_t *temp_method = NULL;
    if (!find_service->mMethodList.empty()) {
        list<someip_method_t *>::iterator it2;
        for (it2 = find_service->mMethodList.begin(); it2 != find_service->mMethodList.end(); it2++) {
            temp_method = *it2;
            if (temp_method->method_id == method->method_id ) {
                mServiceListMutex.unlock();
                return STATUS_EXIST;
            }
        }
    }

    temp_method =(someip_method_t *)malloc(sizeof(someip_method_t));
    if (temp_method==NULL) {
        mServiceListMutex.unlock();
        return STATUS_ERR_MALLOC;
    }
    memset(temp_method, 0, sizeof(*temp_method));
    memcpy(temp_method, method, sizeof(*method));

    find_service->mMethodList.push_back(temp_method);
    
    mServiceListMutex.unlock();
    return STATUS_SUCCESS;
}

u32_t serviceDatabase::addEvent(u16_t service_id, u16_t instance_id, someip_event_t *event)
{
    mServiceListMutex.lock();
    if (mServiceList.empty()) {
        mServiceListMutex.unlock();
        return STATUS_FAILED;
    }

    list<someip_service_entry_t *>::iterator it;
    someip_service_entry_t *entry = NULL;
    someip_service_entry_t *find_service = NULL;

    for (it = mServiceList.begin(); it != mServiceList.end(); it++) {
        entry = *it;
        if ((service_id == entry->cfg.s_key.service_id) &&
            (instance_id == entry->cfg.s_key.instance_id) ) {
            find_service = entry;
            break;
        }
    }
    if (find_service == NULL) {
        mServiceListMutex.unlock();
        return STATUS_FAILED;
    }

    someip_event_t *temp_event = NULL;
    if (!find_service->mEventList.empty()) {
        list<someip_event_t *>::iterator it2;
        for (it2 = find_service->mEventList.begin(); it2 != find_service->mEventList.end(); it2++) {
            temp_event = *it2;
            if (temp_event->event_id == event->event_id ) {
                mServiceListMutex.unlock();
                return STATUS_EXIST;
            }
        }
    }

    temp_event =(someip_event_t *)malloc(sizeof(someip_event_t));
    if (temp_event==NULL) {
        mServiceListMutex.unlock();
        return STATUS_ERR_MALLOC;
    }
    memset(temp_event, 0, sizeof(*temp_event));
    memcpy(temp_event, event, sizeof(*event));

    find_service->mEventList.push_back(temp_event);
    
    mServiceListMutex.unlock();
    return STATUS_SUCCESS;
}


u32_t serviceDatabase::addField(u16_t service_id, u16_t instance_id, someip_field_t *field)
{
    mServiceListMutex.lock();
    if (mServiceList.empty()) {
        mServiceListMutex.unlock();
        return STATUS_FAILED;
    }

    list<someip_service_entry_t *>::iterator it;
    someip_service_entry_t *entry = NULL;
    someip_service_entry_t *find_service = NULL;

    for (it = mServiceList.begin(); it != mServiceList.end(); it++) {
        entry = *it;
        if ((service_id == entry->cfg.s_key.service_id) &&
            (instance_id == entry->cfg.s_key.instance_id) ) {
            find_service = entry;
            break;
        }
    }
    if (find_service == NULL) {
        mServiceListMutex.unlock();
        return STATUS_FAILED;
    }

    someip_field_t *temp_field = NULL;
    if (!find_service->mFieldList.empty()) {
        list<someip_field_t *>::iterator it2;
        for (it2 = find_service->mFieldList.begin(); it2 != find_service->mFieldList.end(); it2++) {
            temp_field = *it2;
            if (temp_field->field_id == field->field_id ) {
                mServiceListMutex.unlock();
                return STATUS_EXIST;
            }
        }
    }

    temp_field =(someip_field_t *)malloc(sizeof(someip_field_t));
    if (temp_field==NULL) {
        mServiceListMutex.unlock();
        return STATUS_ERR_MALLOC;
    }
    memset(temp_field, 0, sizeof(*temp_field));
    memcpy(temp_field, field, sizeof(*field));

    find_service->mFieldList.push_back(temp_field);
    
    mServiceListMutex.unlock();
    return STATUS_SUCCESS;
}
