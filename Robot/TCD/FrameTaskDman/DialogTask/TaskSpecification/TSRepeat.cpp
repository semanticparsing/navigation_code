#include "TSRepeat.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

DEFINE_AGENCY( CRepeatAgency,
    PRECONDITION( false )
    RESET_ON_COMPLETION
    IS_NOT_DTS_AGENT()

    TRIGGERED_BY_COMMANDS("[repeat]", "none")
    DEFINE_SUBAGENTS(
        SUBAGENT( ExecuteRepeat, CExecuteRepeat, "none")
    )
)

DEFINE_EXECUTE_AGENT( CExecuteRepeat,
    IS_NOT_DTS_AGENT()

    ON_EXECUTE_FAIL(
       TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT 
    )

    virtual TDialogExecuteReturnCode Execute(COutput* output) {
        if ( output == NULL ) {
            VLOG(WARNING) << __FUNCTION__ << ",output is NULL.";
            return dercError;
        }

        if (  _owner_core_agent != NULL && _owner_core_agent->GetOutputManagerAgent() != NULL ) {
            COutput last_output;
            if ( _owner_core_agent->GetOutputManagerAgent()->GetLastOutputs(_owner_core_agent->GetSession(), last_output) ) {
                if ( _owner_core_agent->GetOutputManagerAgent()->GetLastHelpfulOutputs(_owner_core_agent->GetSession(), *output) ) {
                    output->SetModelType( last_output.ModelType() );
                    SetCompleted();
                    return dercWaitForEvent;
                } else {
                    VLOG(WARNING) << __FUNCTION__ << ", Can not GetLastHelpfulOutputs, session: " << _owner_core_agent->GetSession();
                    return dercError;
                }
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", Can not GetLastOutputs, session: " 
                    << _owner_core_agent->GetSession();
                return dercError;
            }
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", _owner_core_agent != NULL: " << ( _owner_core_agent != NULL );
            return dercError;
        }
    }
)

void RegisterRepeatAgent() {
    DECLARE_AGENT( CRepeatAgency)
    DECLARE_AGENT( CExecuteRepeat)
}

}
