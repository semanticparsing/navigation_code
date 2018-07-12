/***************************************************************************
 *
 * Copyright (c) 2016 Trio.com, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file serviceHead.h
 * @author liwei(liwei@trio.ai)
 * @date 2016/12/14 20:52:02
 * @brief
 *
 **/

#ifndef ROBOT_PUBLIC_SERVICEHEAD_H_
#define ROBOT_PUBLIC_SERVICEHEAD_H_

#include <string>
#include "Common/Util/ThriftClientWrapper/thriftClientManager.h"
#include "Common/Util/ZKThriftManager/zkthriftManager.h"


#include "Robot/Public/Proto/BrainServing.h"
#include "Robot/Public/Proto/TrioBrainServing.h"
#include "Robot/Public/Proto/ChatServing.h"
#include "Robot/Public/Proto/TrioSysControlServing.h" 
#include "Robot/Public/Proto/PornFilterServing.h"
#include "Robot/Public/Proto/VerticalServing.h"
#include "Robot/Public/Proto/SysProfileServing.h"
#include "Robot/Public/Proto/QaServing.h"
#include "Robot/Public/Proto/WeatherServing.h"
#include "Robot/Public/Proto/TrioWeatherServing.h"
#include "Robot/Public/Proto/NewsServing.h"
#include "Robot/Public/Proto/JokeServing.h"
#include "Robot/Public/Proto/SingleRoundServing.h"
#include "Robot/Public/Proto/QueryPostServing.h"
#include "Robot/Public/Proto/QueryReplyServing.h"
#include "Robot/Public/Proto/TopicServing.h"
#include "Robot/Public/Proto/FaqServing.h"
#include "Robot/Public/Proto/SimilarityServing.h"
#include "Robot/Public/Proto/KnowledgeServing.h"
#include "Robot/Public/Proto/CalculatorServing.h"
#include "Robot/Public/Proto/AskDatetimeServing.h"
#include "Robot/Public/Proto/FaqApiServing.h"
#include "Robot/Public/Proto/TrioXFaqApiServing.h"
#include "Robot/Public/Proto/GenerativeChatServing.h"
#include "Robot/Public/Proto/SNewsServing.h"
#include "Robot/Public/Proto/GfNewsServing.h"
#include "Robot/Public/Proto/GfFuturesServing.h"
#include "Robot/Public/Proto/WorkflowServing.h"
#include "Robot/Public/Proto/MiParserServing.h"
#include "Robot/Public/Proto/VideoServing.h"
#include "Robot/Public/Proto/InfoQueryServing.h"
#include "Robot/Public/Proto/TrioChatServing.h"
#include "Robot/Public/Proto/TrioQueryPostServing.h"
#include "Robot/Public/Proto/TrioQueryReplyServing.h"
#include "Robot/Public/Proto/TrioBrainServing.h"
#include "Robot/Public/Proto/TrioSysProfileServing.h"
#include "Robot/Public/Proto/TrioNewsServing.h"
#include "Robot/Public/Proto/TrioTopicServing.h"
#include "Robot/Public/Proto/ContextQueryPostServing.h"
#include "Robot/Public/Proto/ContextQueryReplyServing.h"
#include "Robot/Public/Proto/TrioGenerativeChatServing.h"
#include "Robot/Public/Proto/ChinaMobileCRSServing.h"
#include "Robot/Public/Proto/TrioWordFilterServing.h"
#include "Robot/Public/Proto/QueryParserServing.h"
#include "Robot/Public/Proto/QueryUnderstandingServing.h"
#include "Robot/Public/Proto/XuhuiH5WavWorkflowServing.h"
#include "Robot/Public/Proto/FacadeServing.h"
#include "Robot/Public/Proto/MusicServing.h"
#include "Robot/Public/Proto/TrioJokeServing.h"
#include "Robot/Public/Proto/GuangfaBrainServing.h"
#include "Robot/Public/Proto/TrioAudioServing.h"
#include "Robot/Public/Proto/TrioAudioChildServing.h"
#include "Robot/Public/Proto/VideoControllerServing.h"
#include "Robot/Public/Proto/TrioIotDeviceControlServing.h"
#include "Robot/Public/Proto/TimeDateServing.h"
#include "Robot/Public/Proto/TrioSkillServing.h"
#include "Robot/Public/Proto/SkillIntentServing.h"
#include "Robot/Public/Proto/TrioRemindServing.h"
#include "Robot/Public/Proto/SkillTestBrainServing.h"
#include "Robot/Public/Proto/TrioTakeOutFoodServing.h"
#include "Robot/Public/Proto/TrioMovieTicketServing.h"
#include "Robot/Public/Proto/ShoppingServing.h"
#include "Robot/Public/Proto/NerServing.h"
#include "Robot/Public/Proto/ContentAggregationServing.h"


using namespace skill_intent;

#define REGISTER_CALL_RPC_SERVICE(ClassName, ServicePrefix, m_client_manager, ServiceName) \
void ClassName::Call##ServicePrefix(const robot::DataBundle* request, robot::DataBundle* response, BlockingCounter* p_block_counter) {\
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
    if (!response->__isset.return_status) {\
        response->__set_return_status(robot::ReturnStatus::TIMEOUT);\
    }\
    if (p_block_counter != NULL) { \
        p_block_counter->DecrementCount();\
    } \
}

#define REGISTER_CALL_RPC_SERVICE_ZK(ClassName, ServicePrefix, m_client_manager, ServiceName) \
void ClassName::Call##ServicePrefix(const robot::DataBundle* request, robot::DataBundle* response, BlockingCounter* p_block_counter) {\
    CHECK(NULL != response) << "response is NULL"; \
    int32_t backend_idx = -1; \
    int32_t update_id = -1; \
    CHECK(NULL != m_client_manager.get()) << "zk client manager is NULL!" << m_client_manager->GetMonitorName(); \
    robot::ThriftClientProxy<ServicePrefix##ServingClient>* client = m_client_manager->GetThriftClient(&backend_idx, &update_id); \
    if (client == NULL || backend_idx < 0 || update_id < 0) {\
        VLOG(1) << "failed to create " << ServiceName << " client";\
        if (p_block_counter != NULL) { \
            p_block_counter->DecrementCount();\
        }\
        return;\
    }\
    client->CallClosureService(&ServicePrefix##ServingClient::ServicePrefix##Process, *response, *request);\
    m_client_manager->ReleaseClient(backend_idx, update_id, client);\
    if (!response->__isset.return_status) {\
        response->__set_return_status(robot::ReturnStatus::TIMEOUT);\
    }\
    if (p_block_counter != NULL) { \
        p_block_counter->DecrementCount();\
    }\
}

#define DEFINE_CALL_RPC_SERVICE(ServicePrefix) \
void Call##ServicePrefix(const robot::DataBundle* request, robot::DataBundle* response, BlockingCounter* p_block_counter);


#define TRIO_REGISTER_CALL_RPC_SERVICE(ClassName, ServicePrefix, ServiceInterface, m_client_manager, ServiceName) \
    void ClassName::Call##ServicePrefix(const robot::TrioDataBundle* request, robot::TrioDataBundle* response, BlockingCounter* p_block_counter) {\
    CHECK(NULL != response) << "response is NULL"; \
    int32_t backend_idx = -1; \
    int32_t update_id = -1; \
    CHECK(NULL != m_client_manager.get()) << "zk client manager is NULL!" << m_client_manager->GetMonitorName(); \
    robot::ThriftClientProxy<ServicePrefix##ServingClient>* client = m_client_manager->GetThriftClient(&backend_idx, &update_id); \
    if (client == NULL || backend_idx < 0 || update_id < 0) {\
        VLOG(1) << "failed to get " << ServiceName << " client" << " endidx " << backend_idx << " updateid " << update_id;\
        if (p_block_counter != NULL) {\
            p_block_counter->DecrementCount();\
        }\
        return;\
    }\
    client->CallClosureService(&ServicePrefix##ServingClient::ServiceInterface, *response, *request);\
    m_client_manager->ReleaseClient(backend_idx, update_id, client);\
    if (!response->__isset.resp) {\
        robot::ResponseParams params; \
        params.__set_return_status(robot::ReturnStatus::TIMEOUT); \
        response->__set_resp(params);\
    }\
    if (response->ctx.__isset.reply_from_service) { \
        response->resp.__set_reply_from_service(response->ctx.reply_from_service); \
    }\
    if (p_block_counter != NULL) { \
        p_block_counter->DecrementCount();\
    }\
}

#define TRIO_DEFINE_CALL_RPC_SERVICE(ServicePrefix) \
    void Call##ServicePrefix(const robot::TrioDataBundle* request, robot::TrioDataBundle* response, BlockingCounter* p_block_counter);

#define TRIO_REGISTER_CALL_RPC_SERVICE2(ClassName, ServicePrefix2, ServicePrefix, ServiceInterface, m_client_manager, ServiceName) \
    void ClassName::Call##ServicePrefix2(const robot::TrioDataBundle* request, robot::TrioDataBundle* response, BlockingCounter* p_block_counter) {\
    CHECK(NULL != response) << "response is NULL"; \
    int32_t backend_idx = -1; \
    int32_t update_id = -1; \
    CHECK(NULL != m_client_manager.get()) << "zk client manager is NULL!" << m_client_manager->GetMonitorName(); \
    robot::ThriftClientProxy<ServicePrefix##ServingClient>* client = m_client_manager->GetThriftClient(&backend_idx, &update_id); \
    if (client == NULL || backend_idx < 0 || update_id < 0) {\
        VLOG(1) << "failed to get " << ServiceName << " client";\
        if (p_block_counter != NULL) {\
            p_block_counter->DecrementCount();\
        }\
        return;\
    }\
    client->CallClosureService(&ServicePrefix##ServingClient::ServiceInterface, *response, *request);\
    m_client_manager->ReleaseClient(backend_idx, update_id, client);\
    if (!response->__isset.resp) {\
        robot::ResponseParams params; \
        params.__set_return_status(robot::ReturnStatus::TIMEOUT); \
        response->__set_resp(params);\
    }\
    if (p_block_counter != NULL) { \
        p_block_counter->DecrementCount();\
    }\
}


#define TRIO_DEFINE_CALL_RPC_SERVICE2(ServicePrefix2) \
    void Call##ServicePrefix2(const robot::TrioDataBundle* request, robot::TrioDataBundle* response, BlockingCounter* p_block_counter);

#endif  // ROBOT_PUBLIC_SERVICEHEAD_H_



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
