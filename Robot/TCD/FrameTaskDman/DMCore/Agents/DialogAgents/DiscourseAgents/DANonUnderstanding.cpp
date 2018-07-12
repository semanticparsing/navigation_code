#include "DANonUnderstanding.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

DEFINE_AGENCY( CNonUnderstanding,
    PRECONDITION(false);
    ON_COMPLETION(C("last_nonunderstood_turn") = -2)
    DEFINE_CONCEPTS(
        INT_SYSTEM_CONCEPT(last_nonunderstood_turn)
        STRING_SYSTEM_CONCEPT(call_agent)
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(FirstNonUnderstanding, CFirstNonUnderstanding, "none")
        SUBAGENT(SubsequentNonUnderstanding, CSubsequentNonUnderstanding, "none")
    )    
)

DEFINE_INFORM_AGENT( CFirstNonUnderstanding,
    TRIGGERED_BY( (_owner_core_agent->LastTurnNonUnderstanding()) && ((int)C("last_nonunderstood_turn") < _owner_core_agent->GetLastInputTurnNumber() - 1) );
    virtual TDialogExecuteReturnCode Execute(COutput* output) {
        CDialogAgent *focued = _owner_core_agent->GetAgentPreviouslyInFocus(this);
        C("calling_agent") = focued->GetName();

        if ( focued->GetExecuteCounter() >= focued->GetMaxExecuteCounter() ) {
            focued->SetCompleted(ctFailed);
            return dercContinueExecution;
        }

        std::string explain_more_prompt = focued->CreateVersionedPrompt("explain_more");
        //COutputManagerAgent::Instance()->Output(focued, explain_more_prompt, fsUser);
        SetCompleted(ctSuccess);
        _owner_core_agent->PopAgentFromExecutionStack(this);

        return dercTakeFloor;
    }

    ON_COMPLETION(
        C("last_nonunderstood_turn") = _owner_core_agent->GetLastInputTurnNumber();
        Reset();
    )   
)

DEFINE_INFORM_AGENT( CSubsequentNonUnderstanding,
        /*
    TRIGGERED_BY(_owner_core_agent->LastTurnNonUnderstanding() && 
        ((int)C("last_nonunderstood_turn") == _owner_core_agent->GetLastInputTurnNumber() - 1) &&
        (!((CDialogAgent*)CRegistry::Instance()->operator[]((std::string)C("calling_agent")))->HasCompleted()));
    virtual TDialogExecuteReturnCode Execute(COutput* output) {
        CDialogAgent* focused = (CDialogAgent*)CRegistry::Instance()->operator[]((std::string)C("calling_agent"));

        if ( focused->GetExecuteCounter() >= focused->GetMaxExecuteCounter() ) {
            focused->SetCompleted(ctFailed);
            return dercContinueExecution;
        }
        std::string what_can_i_sya_prompt = focused->CreateVersionedPrompt("what_can_i_say");
        //COutputManagerAgent::Instance()->Output(focused, what_can_i_sya_prompt, fsUser);
        SetCompleted(ctSuccess);
        _owner_core_agent->PopAgentFromExecutionStack(this);
        return dercTakeFloor;
    }
    */
    ON_COMPLETION(
        C("last_nonunderstood_turn") = _owner_core_agent->GetLastInputTurnNumber();
        Reset();
    )
)

DEFINE_AGENCY( CDialogNonUnderstanding,
    IS_MAIN_TOPIC();
)

DEFINE_AGENCY( CChannelNonUnderstanding,
    IS_MAIN_TOPIC()
)

void RegisterNonUnderstandingAgents() {
    DECLARE_AGENT(CDialogNonUnderstanding)
    DECLARE_AGENT(CChannelNonUnderstanding)
    DECLARE_AGENT(CNonUnderstanding)
    DECLARE_AGENT(CFirstNonUnderstanding)
    DECLARE_AGENT(CSubsequentNonUnderstanding)
}

}
