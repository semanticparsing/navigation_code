#include "TSHelp.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

DEFINE_AGENCY( CHelpAgency,
    PRECONDITION(false)
    RESET_ON_COMPLETION
    IS_NOT_DTS_AGENT()

    TRIGGERED_BY_COMMANDS("[help]", "none")
    DEFINE_SUBAGENTS(
        SUBAGENT( HelpInform, CHelpInform, "none")
    )
)

DEFINE_REQUEST_AGENT( CHelpInform,
    IS_NOT_DTS_AGENT()
    
    ON_EXECUTE_FAIL(
       TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT 
    )
    public:
        virtual std::string Prompt() {
            if ( _owner_core_agent != NULL ) {
                CDialogAgent* da_focused = _owner_core_agent->GetCurrentMainTopicAgent();
                if ( da_focused != NULL ) {
                    return da_focused->CreateVersionedPrompt("Help");
                }
            } else {
                VLOG(FATAL) << "CErrorHandleAgency::Execute, _owner_core_agent is NULL.";
            }
            return "";
        }

        virtual  CConcept& NLGConceptParam() {
            if ( _owner_core_agent != NULL ) {
                CDialogAgent* da_focused = _owner_core_agent->GetCurrentMainTopicAgent();
                if ( da_focused != NULL ) {
                    return da_focused->NLGConceptHelpParam();
                }
            }
            return NLGConceptHelpParam();
        }
)

void RegisterHelpAgent() {
    DECLARE_AGENT( CHelpAgency)
    DECLARE_AGENT( CHelpInform)
}

}
