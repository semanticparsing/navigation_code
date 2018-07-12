#include "MAInform.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

namespace TrioTDM {

CMAInform::CMAInform(const std::string& name, const std::string& configuration, const std::string& type) :
    CDialogAgent(name, configuration, type) {
}

CMAInform::~CMAInform() {
    ;
}

CAgent* CMAInform::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CMAInform(name, configuration);
}

TDialogExecuteReturnCode CMAInform::Execute(COutput* output) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    IncrementExecuteCounter();
    if ( _owner_core_agent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _owner_core_agent is NULL.";
        return dercError;
    }
    if ( CNLG::Instance() == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", CNLG::Instance() is NULL.";
        return dercError;
    }
    if ( output == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", output is NULL.";
        return dercError;
    }
    std::string prompt = Prompt();

   OUTPUT_SET_INPUT;

    if ( prompt.empty() ) {
        VLOG(FATAL) << __FUNCTION__ << ", AgentName: " << _name << ", prompt.empty()";
        return dercError;
    }
    std::string msg = CNLG::Instance()->Generate(prompt, NLGConceptParam());
    if ( !msg.empty() ) {
        output->SetSendMsg(msg);
        SetCompleted(ctSuccess);
    } else {
        VLOG(WARNING) << __FUNCTION__ <<  ", Execute fail, prompt: " << prompt;
        SetCompleted(ctFailed);
        return dercError;
    }
    if ( _owner_core_agent->GetOutputManagerAgent() != NULL ) {
        _owner_core_agent->GetOutputManagerAgent()->AddOutputs(_owner_core_agent->GetSession(), *output);
    } else {
        VLOG(FATAL) << __FUNCTION__ << ", OutputManagerAgent is NULL.";
    }
    return dercTakeFloor;
}

void CMAInform::ReOpenTopic() {
    CDialogAgent::ReOpenTopic();
}

void CMAInform::Reset() {
    CDialogAgent::Reset();
}

bool CMAInform::SuccessCriteriaSatisfied() {
    return _execute_counter > 0;
}

TDialogExecuteReturnCode CMAInform::ReturnCode() {
    return dercTakeFloor;
}

int CMAInform::ModelTypeOperator() {
    return BROADCAST;
}

}
