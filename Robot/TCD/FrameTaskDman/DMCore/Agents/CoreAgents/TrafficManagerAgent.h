// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-23 10:33
// Description: 

#ifndef _TRAFFIC_MANAGER_AGENT_H__
#define _TRAFFIC_MANAGER_AGENT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Agent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DialogAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Concepts/Concept.h"

namespace TrioTDM {

typedef struct {
    std::string sSlotName;
    std::string sSlotValue;
} TSlotValuePair;

typedef struct {
    std::string sSlotValue;
    CConcept* pConcept;
} TSlotConceptPair;

typedef struct {
    std::string sModuleFunction;
    std::vector<TSlotValuePair> vToParams;
    std::vector<TSlotValuePair> vFromParams;
    bool bBlockingCall;
} TExternalCallSpec;

class CTrafficManagerAgent : public CAgent {

public:
    CTrafficManagerAgent(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CTrafficManagerAgent");
    virtual ~CTrafficManagerAgent();
    static CAgent* AgentFactory(const std::string&  name, const std::string& sAConfiguration);
    static CTrafficManagerAgent* Instance();

public:
    void Call(CDialogAgent* agent, const std::string& call);

private:
    void GalaxyCall(const TExternalCallSpec& call);
    void OaaCall(const TExternalCallSpec& call);

private:
    static CTrafficManagerAgent* s_instance;
};

}

#endif
