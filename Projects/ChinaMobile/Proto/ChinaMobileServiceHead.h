/***************************************************************************
 *
 * Copyright (c) 2016 Trio.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __PROJECTS_CHINAMOBILE_SERVICE_HEAD_H__
#define __PROJECTS_CHINAMOBILE_SERVICE_HEAD_H__

#include "Common/Util/ThriftClientWrapper/thriftClientManager.h"
#include "Common/Util/ZKThriftManager/zkthriftManager.h"


#include "Projects/ChinaMobile/Proto/ChinaMobileCRSServing.h"

#define REGISTER_CALL_CHINAMOBILE_SERVICE(ClassName, ServicePrefix, m_client_manager, ServiceName) \
void ClassName::Call##ServicePrefix(const chinaMobile::ChinaMobileBundle* request, chinaMobile::ChinaMobileBundle* response, BlockingCounter* p_block_counter) {\
    CHECK(NULL != response) << "response is NULL"; \
    string serviceKey = "";\
    string allocationKey = Int64ToString(common::GetTimeInMs());\
    CHECK(NULL != m_client_manager.get()) << ServiceName << "client is NULL!"; \
    robot::ThriftClientProxy<ServicePrefix##ServingClient>* client = m_client_manager->GetThriftClient(allocationKey, &serviceKey);\
    if (client == NULL) {\
        VLOG(1) << "failed to create " << ServiceName << " client";\
        return;\
    }\
    client->CallClosureService(&ServicePrefix##ServingClient::ServicePrefix##Process, *response, *request);\
    m_client_manager->ReleaseClient(serviceKey, client);\
    if (!response->__isset.ret) {\
        response->__set_ret(-1);\
    }\
    if (p_block_counter != NULL) { \
        p_block_counter->DecrementCount();\
    } \
}


#define REGISTER_CALL_CHINAMOBILE_FAQ_SERVICE(ClassName, ServicePrefix, m_client_manager, ServiceName) \
void ClassName::Call##ServicePrefix(const robot::TrioDataBundle* request, robot::TrioDataBundle* response, BlockingCounter* p_block_counter) {\
    CHECK(NULL != response) << "response is NULL"; \
    string serviceKey = "";\
    string allocationKey = Int64ToString(common::GetTimeInMs());\
    CHECK(NULL != m_client_manager.get()) << ServiceName << "client is NULL!"; \
    robot::ThriftClientProxy<ServicePrefix##ServingClient>* client = m_client_manager->GetThriftClient(allocationKey, &serviceKey);\
    if (client == NULL) {\
        VLOG(1) << "failed to create " << ServiceName << " client";\
        return;\
    }\
    client->CallClosureService(&ServicePrefix##ServingClient::ServicePrefix##Process, *response, *request);\
    m_client_manager->ReleaseClient(serviceKey, client);\
    if (p_block_counter != NULL) { \
        p_block_counter->DecrementCount();\
    } \
}


#define REGISTER_CALL_CHINAMOBILE_SERVICE_ZK(ClassName, ServicePrefix, m_client_manager, ServiceName) \
void ClassName::Call##ServicePrefix(const chinaMobile::ChinaMobileBundle* request, chinaMobile::ChinaMobileBundle* response, BlockingCounter* p_block_counter) {\
    CHECK(NULL != response) << "response is NULL"; \
    int32_t backend_idx = -1; \
    int32_t update_id = -1; \
    CHECK(NULL != m_client_manager.get()) << "zk client manager is NULL!" << m_client_manager->GetMonitorName(); \
    robot::ThriftClientProxy<ServicePrefix##ServingClient>* client = m_client_manager->GetThriftClient(&backend_idx, &update_id); \
    if (client == NULL) {\
        VLOG(1) << "failed to create " << ServiceName << " client";\
        return;\
    }\
    client->CallClosureService(&ServicePrefix##ServingClient::ServicePrefix##Process, *response, *request);\
    m_client_manager->ReleaseClient(backend_idx, update_id, client);\
    if (!response->__isset.ret) {\
        response->__set_ret(-1);\
    }\
    if (p_block_counter != NULL) { \
        p_block_counter->DecrementCount();\
    }\
}

#define DEFINE_CALL_CHINAMOBILE_SERVICE(ServicePrefix) \
void Call##ServicePrefix(const chinaMobile::ChinaMobileBundle* request, chinaMobile::ChinaMobileBundle* response, BlockingCounter* p_block_counter);

#define DEFINE_CALL_CHINAMOBILE_FAQ_SERVICE(ServicePrefix) \
void Call##ServicePrefix(const robot::TrioDataBundle* request, robot::TrioDataBundle* response, BlockingCounter* p_block_counter);

#endif

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
