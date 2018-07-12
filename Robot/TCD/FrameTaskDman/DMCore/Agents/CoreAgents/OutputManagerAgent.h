// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-23 10:49
// Description: 输出管理

#ifndef _OUTPUT_MANAGER_AGENT_H__
#define _OUTPUT_MANAGER_AGENT_H__

#include <boost/thread.hpp>
#include "Robot/TCD/FrameTaskDman/Utils/Utils.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Agent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DialogAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Outputs/Output.h"

#define OD_NOTIFIES 0x1

namespace TrioTDM {
/**
 * @brief 输出管理类
 */
class COutputManagerAgent : public CAgent {

public:
    COutputManagerAgent(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:COutputManagerAgent");
    virtual ~COutputManagerAgent();

    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);
    virtual void Reset();
    /**
     * @brief 序列化
     */
    bool DeclareSaveData(std::map<std::string, std::string>& agents);
    /**
     * @brief 反序列化
     */
    bool Load(const std::map<std::string, std::string>& agents);

public:
    /**
     * @brief 返回最后一次的输出
     */
    bool GetLastOutputs(const std::string& session, COutput& output);
    /**
     * @brief 返回最后一次的有用输出
     */
    bool GetLastHelpfulOutputs(const std::string& session, COutput& output);
    /**
     * @brief 添加相应 session 的输出
     */
    void AddOutputs(const std::string& session, const COutput& output);

private:
    std::vector<COutput> _session2outpus; ///< 输出集合

};

}

#endif
