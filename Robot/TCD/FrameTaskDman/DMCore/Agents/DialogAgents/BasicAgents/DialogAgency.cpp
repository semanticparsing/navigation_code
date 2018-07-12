#include "DialogAgency.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

namespace TrioTDM {

#define AGENCY_MAX_EXECUTE_NUM 300

CDialogAgency::CDialogAgency(const std::string& name, const std::string& configuration, const std::string& type) 
: CDialogAgent(name, configuration, type) {
    ;
}

CDialogAgency::~CDialogAgency() {
    ;
}

CAgent* CDialogAgency::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CDialogAgency(name, configuration);
}

void CDialogAgency::Create() {
    CreateConcepts();

    if ( IsCreateBuildTree() ) {
        CreateSubAgents();
    }
    OnCreation();
}

void CDialogAgency::Initialize() {
    OnInitialization();
}

int CDialogAgency::GetMaxExecuteCounter() {
    if ( Config::Instance() != NULL ) {
        int n = Config::Instance()->GetIntValue("task_dman/agency_max_execute_num_limit");
        if ( n > 0 ) {
            return n;
        }
    }
    return AGENCY_MAX_EXECUTE_NUM;
}

void CDialogAgency::CreateSubAgents() {
}

TDialogExecuteReturnCode CDialogAgency::Execute(COutput* output) {
    if ( _owner_core_agent == NULL ) {
        VLOG(WARNING) <<__FUNCTION__ << ", _owner_core_agent is NULL, AgentName: " << _name;
        return dercError;
    }
    CDialogAgent *dialog_agent = NextAgentToExecute();
    if ( dialog_agent ) {
        _owner_core_agent->ContinueWith(this, dialog_agent);
        IncrementExecuteCounter();
        return dercContinueExecution;
    } else {
        VLOG(WARNING) << __FUNCTION__ << "agency " << GetName() << " failed to complete.";
        return dercError;
    }
}

CDialogAgent* CDialogAgency::NextAgentToExecute() {

    std::string current_execution_policy = GetExecutionPolicy();

    VLOG(DEBUG) << __FUNCTION__ << ", ExecutePolicy: " << current_execution_policy;

    if ( current_execution_policy == START_AGAIN_LEFT_TO_RIGHT ) {
        // while( HasCompleted() ) { contine; }
        if ( IsStartAgain() ) {
            VLOG(DATA) << __FUNCTION__ << ", SubAgent Continue, Start Again, AgentName: " << _dialog_agent_name;
            Reset();
        } else {
            bool is_over = true;
            for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
                if ( !_sub_agents[i]->HasCompleted() ) {
                    is_over = false;
                    break;
                }
            }
            // while( HasCompleted() ) { exeecute over; }
            if ( is_over ) {
                VLOG(DATA) << __FUNCTION__ << ", All SubAgent is Completed, Start Again, AgentName: " << _dialog_agent_name;
                Reset();
            }
        }
    }

    if ( current_execution_policy == LEFT_TO_RIGHT_OPEN || 
            current_execution_policy == START_AGAIN_LEFT_TO_RIGHT ) {
        for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
            if ( !_sub_agents[i]->HasCompleted() && 
                    _sub_agents[i]->PreconditionsSatisfied() && 
                    !_sub_agents[i]->IsBlocked() ) {
                return _sub_agents[i];
            }
        }
        VLOG(WARNING) << __FUNCTION__ << ", Failure information for agency " << _dialog_agent_name;
        for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
            VLOG(WARNING) << __FUNCTION__ << ", Agent: " << _sub_agents[i]->GetDialogAgentName() 
                << "\tHasCompleted: " << _sub_agents[i]->HasCompleted() 
                << "\tPreconditionsSatisfied: " << _sub_agents[i]->PreconditionsSatisfied(); 
        }
        return NULL;
    } else if ( current_execution_policy == LEFT_TO_RIGHT_ENFORCED ) {
        size_t i = 0;
        while ( (i < _sub_agents.size()) && (_sub_agents[i]->HasCompleted()) ) {
            _sub_agents[i]->Block();
            ++i;
        }
        if ( i == _sub_agents.size() ) {
            VLOG(WARNING) << __FUNCTION__ << ", All agents are completed, AgentName: " << _name;
            return NULL;
        }
        _sub_agents[i]->UnBlock();
        int index_next_agent = i;
        for ( i++; i < _sub_agents.size(); ++i ) {
            _sub_agents[i]->Block();
        }
        return _sub_agents[index_next_agent];
    }

    return NULL;
}

std::string CDialogAgency::GetExecutionPolicy() {
    return LEFT_TO_RIGHT_OPEN;
}

bool CDialogAgency::SuccessCriteriaSatisfied() {
    std::string success_policy = GetSuccessPolicy();

    // VLOG(DEBUG) << __FUNCTION__ << ", Policy: " << success_policy << ", AgentName: " << _name;

    if ( success_policy == SUCCESS_POLICY_NORMAL ) {
        return CDialogAgent::SuccessCriteriaSatisfied();
    } else if ( success_policy == SUCCESS_POLICY_SELECT ) {
        for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
            if ( _sub_agents[i]->HasCompleted() ) {
                return true;
            }
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", can not deal this success_policy, " << success_policy;
    }
    return false;
}

bool CDialogAgency::IsCreateBuildTree() {
    return true;
}

void CDialogAgency::OnFocusclaims() {
    VLOG(DEBUG) << __FUNCTION__ << ", IsCreateSubAgent: " << _is_create_sub_agent << "\tAgentName: " << _name; 
    if ( !_is_create_sub_agent ) {
        DO_CREATE_SUB_AGENTS;
    }
}

bool CDialogAgency::IsNeedSaveForTaskSkip() {
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        if ( _sub_agents[i] != NULL && (!_sub_agents[i]->GetIsInitialStatus() ||  _sub_agents[i]->IsNeedSaveForTaskSkip()) ) {
            return true;
        }
    }
    return false;
}

std::string CDialogAgency::GetSuccessPolicy() {
    return SUCCESS_POLICY_NORMAL;
}

}
