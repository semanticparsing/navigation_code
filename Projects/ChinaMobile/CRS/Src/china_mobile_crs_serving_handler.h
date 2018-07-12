// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-30 13:53
// Description: 

#ifndef _CHINA_MOBILE_CRS_SERVING_HANDLER_H__
#define _CHINA_MOBILE_CRS_SERVING_HANDLER_H__

#include <boost/thread.hpp>
//#include "Robot/Public/serviceHead.h"
#include "Projects/ChinaMobile/Proto/ChinaMobileCRSServing.h"
//#include "Common/Util/Thread/threadPool.h"
//#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/Utils/RedisWrapper.h"
//#include "Robot/TCD/FrameTaskDman/DMCore/Agents/CoreAgents/AllCoreAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace chinaMobile {

typedef void (OutParams::*SetOutParamsValue)(const std::string& value);

typedef struct CoreAgentInfo {
    CoreAgentInfo() : 
        nExecuteCount(0),
        nCreateTime(0),
        pCoreAgent(NULL) {}

    int nExecuteCount;
    int64 nCreateTime;
    std::string sSession;
    TrioTDM::CDMCoreAgent* pCoreAgent;
} TCoreAgentInfo;

typedef std::map<std::string, TCoreAgentInfo> TSession2CoreAgentInfo;

class CChinaMobileCRSServingHandler : public ChinaMobileCRSServingIf {

public:
    CChinaMobileCRSServingHandler();
    ~CChinaMobileCRSServingHandler();

public:
    bool Init();
    bool InitParam2SetFunc();
    bool InitParamFromConfig();
    virtual void ChinaMobileCRSProcess(ChinaMobileBundle& response, const ChinaMobileBundle& request);

private:
    bool Process(const ChinaMobileBundle& request, ChinaMobileBundle& response);
    bool DialogTaskRun(const std::string& session, const ChinaMobileBundle& request, TCoreAgentInfo& core_agent_info, ChinaMobileBundle& response);
    bool Parser(const std::string& query, TrioTDM::CInteractionEvent& event);
    TCoreAgentInfo GetCoreAgent(const std::string& session, bool& is_insert_session);

    bool LoadCRSFromRedis(TCoreAgentInfo& core_agent_info, const std::string& session);
    bool SaveCRSToRedis(TCoreAgentInfo core_agent_info, const std::string& session);
    bool DeleCRSFromRedis(const std::string& session);
    void DeleteCoreAgent(TCoreAgentInfo& core_agent_info);
    void ReleaseCoreAgent(TCoreAgentInfo& core_agent_info);
    void ReleaseSession(const std::string& session);
    void CheckReleaseCoreAgent();
    bool IsSessionExistInRedis(const std::string& session);
    bool FillResponse(ChinaMobileBundle& response, const TrioTDM::COutput& output, const ChinaMobileBundle& request);

private:
    std::string GetRedisExecuteCountKey(const std::string& session);
    std::string GetRedisAgentKey(const std::string& session);
    std::string GetRedisConceptKey(const std::string& session);

private:
    // 交互
    int _outaction_interactive;
    // 结束
    int _outaction_end;
    // 跳转
    int _outaction_skip;

    // session 初始化
    int _inaction_begin;
    // 返回语音播报及其他结果
    int _inaction_call_ret;
    // 呼叫转移结果
    int _inaction_transfer_ret;
    // 挂机
    std::string _input_hangup;
    // 系统异常
    std::string _input_sys_err;
    // 播放完成
    std::string _input_playover;
    // 转移类型对应的转移路由
    std::map<std::string, std::string> _trans_type2route;

private:
    boost::recursive_mutex _mutex;
    std::map<std::string, SetOutParamsValue>        _param_2_set_func;
    std::set<std::string>                           _sessions;
    // session 对应的 DMCoreAgent 和 执行计数
    TSession2CoreAgentInfo                          _session_2_core;
    // CheckReleaseCoreAgent 的限定条件
    int         _session_num_limit;
    int64       _can_del_time_delta;
    int64       _must_del_time_delta;
};

}

#endif
