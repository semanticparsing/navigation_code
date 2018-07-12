#include "MATaskSkip.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

namespace TrioTDM {

CMATaskSkip::CMATaskSkip(const std::string& name, const std::string& configuration, const std::string& type) :
    CDialogAgent(name, configuration, type) {
    _skip_task = NULL;
}

CMATaskSkip::~CMATaskSkip() {
    ;
}

void CMATaskSkip::OnDestruction() {
    if ( _skip_task != NULL ) {
        _skip_task->OnDestruction();
        delete _skip_task;
        _skip_task = NULL;
    }
}

bool CMATaskSkip::IsDTSAgent() {
    return false;
}

void CMATaskSkip::SetOwnerCoreAgent(CDMCoreAgent* core_agent) {
    // VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name << ", core_agent == NULL: " << (core_agent == NULL);
    _owner_core_agent = core_agent;
    if ( _skip_task != NULL ) {
        _skip_task->SetOwnerCoreAgent(core_agent);
    } else {
        VLOG(DATA) << __FUNCTION__ << ", _skip_task is NULL.";
    }
}

TDialogExecuteReturnCode CMATaskSkip::Execute(COutput* output) {
    IncrementExecuteCounter();
    SetCompleted();
    if ( _skip_task == NULL ) {
        if ( !CreateSkipTask() ) {
            return dercError;
        }
    }
    if ( _owner_core_agent != NULL ) {
        if ( _skip_task != NULL ) {
            // TODO 眺之前做一次 Reset 
            _skip_task->Reset();
            // 跳转清空执行栈
            _owner_core_agent->ResetExecutionStack();
            _skip_task->OnFocusclaims();
            _owner_core_agent->ContinueWith(_owner_core_agent, _skip_task);
            return dercContinueExecution;
        } else {
            VLOG(FATAL) << __FUNCTION__ << ", _skip_task is NULL.";
        }
    } else {
        VLOG(FATAL) << __FUNCTION__ << ", _owner_core_agent is NULL.";
    }
    return dercError;
}

bool CMATaskSkip::SuccessCriteriaSatisfied() {
    return false;
}

void CMATaskSkip::Register() {
    CDialogAgent::Register();
    if ( _skip_task != NULL ) {
        std::string agent_name = _name + "/" + DialogName();
        _skip_task->SetName( agent_name );
        _skip_task->Register();
    }
}

bool CMATaskSkip::Load(const std::map<std::string, std::string>& agents, const std::map<std::string, std::string>& concepts) {
    if ( !CDialogAgent::Load(agents, concepts) ) {
        VLOG(WARNING) << __FUNCTION__ << ", CDialogAgent::Load Fail, AgentName: " << _name;
        return false;
    }

    if ( _is_create_skip_task && _skip_task == NULL ) {
        if ( !CreateSkipTask() ) {
            VLOG(WARNING) << __FUNCTION__ << ", CreateSkipTask Fail, AgentName: " << _name;
            return false;
        }
    }

    if ( _skip_task != NULL ) {
        if ( !_skip_task->Load(agents, concepts) ) {
            VLOG(WARNING) << __FUNCTION__ << ", _skip_task Load Fail, AgentName: " << _name;
            return false;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _skip_task is NULL, AgentName: " << _name;
    }

    return true;
}

bool CMATaskSkip::DeclareSaveData(std::map<std::string, std::string>& agents, std::map<std::string, std::string>& concepts) {
    if ( !CDialogAgent::DeclareSaveData(agents, concepts) ) {
        VLOG(WARNING) << __FUNCTION__ << ", CDialogAgent::DeclareSaveData Fail, AgentName: " << _name;
    }

    if ( _skip_task != NULL ) {
        if ( !_skip_task->DeclareSaveData(agents, concepts) ) {
            VLOG(WARNING) << __FUNCTION__ << ", _skip_task DeclareSaveData Fail, AgentName: " << _name;
            return false;
        }
    } else {
        VLOG(DATA) << __FUNCTION__ << ", _skip_task is NULL, AgentName: " << _name;
    }

    return true;
}

bool CMATaskSkip::IsNeedSaveForTaskSkip() {
    if ( _skip_task != NULL ) {
        if ( !_skip_task->GetIsInitialStatus() ) {
            return true;
        }
        return _skip_task->IsNeedSaveForTaskSkip();
    }
    return false;
}

bool CMATaskSkip::CreateSkipTask() {
    if ( CRegistryAgentType::Instance() == NULL ) {
         VLOG(FATAL) << __FUNCTION__ << ", CRegistryAgentType::Instance() is NULL.";
         return false;
    }
    _skip_task = (CDialogAgent*)CRegistryAgentType::Instance()->CreateAgent(DialogType(), DialogName(), DialogConfiguration());
    if ( _skip_task == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", _skip_task is NULL.";
        return false;
    }
    _skip_task->Initialize();
    _skip_task->SetContextAgent(this);
    _skip_task->SetOwnerCoreAgent( _owner_core_agent );
 
    std::string agent_name = _name + "/" + DialogName();
    _skip_task->SetName( agent_name );
    _skip_task->Register();
    _is_create_skip_task = true;
    return true;

}

std::string CMATaskSkip::DialogConfiguration() {
    return "none";
}

}
