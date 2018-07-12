#include "MAExecute.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

namespace TrioTDM {

CMAExecute::CMAExecute(const std::string& name, const std::string& configuration, const std::string& type) :
    CDialogAgent(name, configuration, type) {
        ;
    }

CMAExecute::~CMAExecute() {
    ;
}

CAgent* CMAExecute::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CMAExecute(name, configuration);
}

TDialogExecuteReturnCode CMAExecute::Execute(COutput* output) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    IncrementExecuteCounter();
    if ( _owner_core_agent == NULL || _owner_core_agent->GetExecuteApi() == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", AgentName: " << _name << 
            "\t_owner_core_agent == NULL: " << (_owner_core_agent == NULL);
        return dercError;
    }
    ExecuteFunc func = GetExecuteFunc();
    if ( func == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", ExecuteFunc is NULL, AgentName: " << _name;
        return dercError;
    }
    CConcept& param = GetExecuteParam();

    if ( !((_owner_core_agent->GetExecuteApi()->*func)( param )) ) {
        VLOG(WARNING) << __FUNCTION__ << ", system error, AgentName: " << _name;
        return dercError;
    }

    return dercContinueExecution;
}

bool CMAExecute::SuccessCriteriaSatisfied() {
    return ( _execute_counter > 0 );
}

ExecuteFunc CMAExecute::GetExecuteFunc() {
    return NULL;
}

CConcept& CMAExecute::GetExecuteParam() {
    return C("param");
}

}
