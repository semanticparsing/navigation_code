#include "GAAskRepeat.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

DEFINE_EXECUTE_AGENT(_CAskRepeat, 
    IS_NOT_DTS_AGENT()
    private:
    CMARequest *_request_agent;
    public:
    void SetRequestAgent(CMARequest *request_agent) {
        _request_agent = request_agent;
    }
    CMARequest* GetRequestAgent() {
        return _request_agent;
    }
    virtual TDialogExecuteReturnCode Execute(COutput* output) {
        /*
        if ( CDMCoreAgent::Instance() == NULL || COutputManagerAgent::Instance() == NULL ) {
            VLOG(WARNING) << "CDMCoreAgent::Instance() == NULL  || COutputManagerAgent::Instance() == NULL";
            return dercTakeFloor;
        }
        if ( GetParameterValue("notify") == "true" ) {
            if ( (CDMCoreAgent::Instance()->GetBindingHistorySize() > 1) && 
                    (CDMCoreAgent::Instance()->GetBindingResult(-2).bNonUnderstanding)) {
                //COutputManagerAgent::Instance()->Output(this, "inform subsequest_nonunderstanding", fsSystem);
            } else {
                //COutputManagerAgent::Instance()->Output(this, "inform nonunderstanding", fsSystem);
            }
        }
        //COutputManagerAgent::Instance()->Output(this, "request nonunderstanding_askrepeat", fsUser);
        _request_agent->IncrementExecuteCounter();
        SetCompleted(ctSuccess);
        CDMCoreAgent::Instance()->PopAgentFromExecutionStack(this);
        */
        return dercTakeFloor;
    }
)

CGAAskRepeat::CGAAskRepeat(const std::string& new_configuration) : CGroundingAction(new_configuration) {
    ;
}

std::string CGAAskRepeat::GetName() {
    return "ASK_REPEAT";
}

void CGAAskRepeat::Run(void *params) {
    /*
    if ( CRegistryAgentType::Instance() == NULL || CDMCoreAgent::Instance() == NULL || CDMCoreAgent::Instance() == NULL ) {
        VLOG(WARNING) << "CRegistryAgentType::Instance() == NULL || CDMCoreAgent::Instance() == NULL || CDMCoreAgent::Instance() == NULL";
        return ;
    }
    _CAskRepeat *ask_repeat_agent = (_CAskRepeat *)(*CRegistry::Instance())["/_AskRepeat"];
    if ( !ask_repeat_agent ) {
        ask_repeat_agent = (_CAskRepeat*)CRegistry::Instance()->CreateAgent("_CAskRepeat", "/_AskRepeat");
        ask_repeat_agent->Initialize();
        ask_repeat_agent->Register();
    } else {
        ask_repeat_agent->Reset();
    }
    ask_repeat_agent->SetDynamicAgentID(((CMARequest*)params)->GetName());
    ask_repeat_agent->SetRequestAgent((CMARequest*)params);
    ask_repeat_agent->SetConfiguration(_configuration);
    CDMCoreAgent::Instance()->ContinueWith(CGroundingManagerAgent::Instance(), ask_repeat_agent);
    */
}

void CGAAskRepeat::RegisterDialogAgency() {
    /*
    if ( CRegistry::Instance() != NULL ) {
        if ( !CRegistry::Instance()->IsRegisteredAgentType("_CAskRepeat") ) {
            CRegistry::Instance()->RegisterAgentType("_CAskRepeat", _CAskRepeat::AgentFactory);
        }
    }
    */
}

}
