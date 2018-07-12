// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-24 11:21
// Description: 

#ifndef _STATE_MANAGER_AGENT_H__
#define _STATE_MANAGER_AGENT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Utils.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Agent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DialogAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/CoreAgents/DMCoreAgent.h"

namespace TrioTDM {

typedef struct {
    TFloorStatus fsFloorStatus;
    std::string sFocusedAgentName;
    TExecutionStack esExecutionStack;
    TExpectationAgenda eaAgenda;
    TSystemAction saSystemAction;
    std::string sInputLineConfiguration;

    int iTurnNumber;
    int iEHIndex;
    std::string sStateName;
} TDialogState;

class CStateManagerAgent : public CAgent {

public:
    CStateManagerAgent(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CStateManagerAgent");
    virtual ~CStateManagerAgent();
    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);
    static CStateManagerAgent* Instance(); 
    virtual void Reset();

public:
    void LoadDialogStateNames(const std::string& file);
    void SetStateBroadcastAddress(const std::string address);
    void BroadcastState();
    void UpdateState(CDMCoreAgent* core_agent);
    std::string GetStateAsString(TDialogState state);
    std::string GetStateAsString();

    int GetStateHistoryLength();
    TDialogState& GetLastState();
    TDialogState& operator[](size_t i);

public:
    static CStateManagerAgent* s_instance;

    std::map<std::string, std::string> _dialog_state_names;
    std::vector<TDialogState> _state_history;
    std::string _state_broadcast_address;
};

}

#endif
