#include "TSErrorHandle.h"
#include "../DialogTask.h"

namespace TrioTDM {

DEFINE_AGENCY( CErrorHandleAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    DEFINE_CONCEPTS(
        INT_SYSTEM_CONCEPT( last_error_turn )
        INT_SYSTEM_CONCEPT( consecutive_error_num )
    )

    DEFINE_SUBAGENTS(
        SUBAGENT( NoUnderstand, CNoUnderstandAgency, "none")
        SUBAGENT( Timeout, CTimeoutAgency, "none")
    )
)

DEFINE_REQUEST_AGENT( CErrorInform,
    IS_NOT_DTS_AGENT()
    public:
        virtual std::string Prompt() {
            if ( _owner_core_agent != NULL ) {
                CDialogAgent* da_focused = _owner_core_agent->GetCurrentMainTopicAgent();
                if ( da_focused != NULL ) {
                    return da_focused->CreateVersionedPrompt(_dialog_agent_name);
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
                    return da_focused->NLGConceptErrorParam();
                }
            }
            return NLGConceptErrorParam();
        }
)

    
DEFINE_INFORM_AGENT( CErrorNotReqInform,
    IS_NOT_DTS_AGENT()
    public:
        virtual std::string Prompt() {
            if ( _owner_core_agent != NULL ) {
                CDialogAgent* da_focused = _owner_core_agent->GetCurrentMainTopicAgent();
                if ( da_focused != NULL ) {
                    return da_focused->CreateVersionedPrompt(_dialog_agent_name);
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
                    return da_focused->NLGConceptErrorParam();
                }
            }
            return NLGConceptErrorParam();
        }
)
#if 0
DEFINE_AGENCY( CFirstErrorTrasferIVRGroundFloorAgency,
    PRECONDITION(
        _owner_core_agent != NULL && _owner_core_agent->GetCurrentMainTopicAgent() != NULL &&
        _owner_core_agent->GetCurrentMainTopicAgent()->GetErrorHandle() == "FirstErrorTrasferIVRGroundFloor"
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( RequestTrasferIVRFloor, CRequestTrasferIVRFloor, "none")
    )
)
#endif
DEFINE_AGENCY( CNoUnderstandAgency,
    SUCCESS_POLICY_IS_SELECT() 
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    DEFINE_SUBAGENTS(
        SUBAGENT( FirstNoUnderstandAgency, CFirstNoUnderstandAgency, "none")
        SUBAGENT( SecondNoUnderstandAgency, CSecondNoUnderstandAgency, "none")
        SUBAGENT( ThirdNoUnderstandAgency, CThirdNoUnderstandAgency, "none")
        SUBAGENT( FourthNoUnderstandAgency, CFourthNoUnderstandAgency, "none")
        SUBAGENT( FifthNoUnderstandAgency, CFifthErrorAgency, "none")
        //SUBAGENT( ThirdNoUnderstandAgency, CThirdErrorAgency, "none")
        SUBAGENT( RecoveryErrorCount, CRecoveryErrorCount, "none")
    )
    public:
        virtual bool ClaimsFocus() {
            if ( _owner_core_agent != NULL ) {
                CDialogAgent* current_main_topic_agent = _owner_core_agent->GetCurrentMainTopicAgent();
                if ( current_main_topic_agent != NULL ) {
                    VLOG(DEBUG) << "ERROR_HANDLE: " << current_main_topic_agent->GetErrorHandle();
                    if ( current_main_topic_agent->GetErrorHandle() == "owner") {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
            return CDialogAgent::ClaimsFocus();
        }
    ON_COMPLETION(
        if ( _owner_core_agent != NULL ) {
            if ( (int)C("last_error_turn") == _owner_core_agent->GetLastInputTurnNumber() - 1 ) {
                C("consecutive_error_num") = (int)C("consecutive_error_num") + 1;
            } else {
                C("consecutive_error_num") = 1;
            }
            C("last_error_turn") =  _owner_core_agent->GetLastInputTurnNumber();
        } else {
            VLOG(FATAL) << __FUNCTION__ << ", _owner_core_agent is NULL.";
        }
        Reset();
    )
)

DEFINE_AGENCY( CFirstNoUnderstandAgency,
    SUCCESS_POLICY_IS_SELECT() 
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && ( (int)C("consecutive_error_num") == 0 || (int)C("last_error_turn") < _owner_core_agent->GetLastInputTurnNumber() )
    )
    DEFINE_SUBAGENTS(
        //SUBAGENT( FirstTrasferIVRGroundFloor, CFirstErrorTrasferIVRGroundFloorAgency, "none")
        SUBAGENT( FirstNoUnderstandInform, CErrorInform, "none")
    )
)

DEFINE_AGENCY( CSecondNoUnderstandAgency,
    SUCCESS_POLICY_IS_SELECT() 
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && ( (int)C("consecutive_error_num") == 1 && (int)C("last_error_turn") == _owner_core_agent->GetLastInputTurnNumber() )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SecondErrorSkipNtmain_3Agency, CSecondErrorSkipNtmain_3Agency, "none")
        SUBAGENT( SecondNoUnderstandInform, CErrorInform, "none")
    )
)

DEFINE_AGENCY( CThirdNoUnderstandAgency,
    SUCCESS_POLICY_IS_SELECT() 
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && ( (int)C("consecutive_error_num") == 2 && (int)C("last_error_turn") == _owner_core_agent->GetLastInputTurnNumber() )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( ThirdErrorAgency, CThirdErrorAgency, "none")
        SUBAGENT( ThirdNoUnderstandInform, CErrorInform, "none")
    )
)

DEFINE_AGENCY( CFourthNoUnderstandAgency,
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && ( (int)C("consecutive_error_num") == 3 && (int)C("last_error_turn") == _owner_core_agent->GetLastInputTurnNumber() )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( FourthNoUnderstandInform, CErrorInform, "none")
    )
)

DEFINE_AGENCY( CTimeoutAgency,
    SUCCESS_POLICY_IS_SELECT() 
    PRECONDITION( false )
    // TRIGGERED_BY_COMMANDS("*[timeout]", "none")
    IS_NOT_DTS_AGENT()
    DEFINE_SUBAGENTS(
        SUBAGENT( FirstTimeoutAgency, CFirstTimeoutAgency, "none")
        SUBAGENT( SecondTimeoutAgency, CSecondTimeoutAgency, "none")
        SUBAGENT( ThirdTimeoutAgency, CThirdTimeoutAgency, "none")
        SUBAGENT( FourthTimeoutAgency, CFourthTimeoutAgency, "none")
        SUBAGENT( FifthTimeoutAgency, CFifthErrorAgency, "none")
        //SUBAGENT( ThirdTimeoutAgency, CThirdErrorAgency, "none")
        SUBAGENT( RecoveryErrorCount, CRecoveryErrorCount, "none")
    )
    public:
        virtual bool ClaimsFocus() {
            if ( _owner_core_agent != NULL ) {
                CDialogAgent* current_main_topic_agent = _owner_core_agent->GetCurrentMainTopicAgent();
                if ( current_main_topic_agent != NULL ) {
                    if (current_main_topic_agent->GetErrorHandle() == "owner") {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
            return CDialogAgent::ClaimsFocus();
        }
    ON_COMPLETION(
        if ( _owner_core_agent != NULL ) {
            if ( (int)C("last_error_turn") == _owner_core_agent->GetLastInputTurnNumber() - 1 ) {
                C("consecutive_error_num") = (int)C("consecutive_error_num") + 1;
            } else {
                C("consecutive_error_num") = 1;
            }
            C("last_error_turn") =  _owner_core_agent->GetLastInputTurnNumber();
        } else {
            VLOG(FATAL) << __FUNCTION__ << ", _owner_core_agent is NULL.";
        }
        Reset();
    )
)

DEFINE_AGENCY( CFirstTimeoutAgency,
    SUCCESS_POLICY_IS_SELECT()
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && ( (int)C("consecutive_error_num") == 0 || (int)C("last_error_turn") < _owner_core_agent->GetLastInputTurnNumber() )
    )
    DEFINE_SUBAGENTS(
        //SUBAGENT( FirstTrasferIVRGroundFloorAgency, CFirstErrorTrasferIVRGroundFloorAgency, "none")
        SUBAGENT( FirstTimeoutInform, CErrorInform, "none")
    )
)

DEFINE_AGENCY( CSecondTimeoutAgency,
    SUCCESS_POLICY_IS_SELECT() 
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && ( (int)C("consecutive_error_num") == 1 && (int)C("last_error_turn") == _owner_core_agent->GetLastInputTurnNumber() )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SecondErrorSkipNtmain_3Agency, CSecondErrorSkipNtmain_3Agency, "none")
        SUBAGENT( SecondTimeoutInform, CErrorInform, "none")
    )
)

DEFINE_AGENCY( CThirdTimeoutAgency,
    SUCCESS_POLICY_IS_SELECT() 
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && ( (int)C("consecutive_error_num") == 2 && (int)C("last_error_turn") == _owner_core_agent->GetLastInputTurnNumber() )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( ThirdErrorAgency, CThirdErrorAgency, "none")
        SUBAGENT( ThirdTimeoutInform, CErrorInform, "none")
    )
)

DEFINE_AGENCY( CFourthTimeoutAgency,
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && ( (int)C("consecutive_error_num") == 3 && (int)C("last_error_turn") == _owner_core_agent->GetLastInputTurnNumber() )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( FourthTimeoutInform, CErrorInform, "none")
    )
)

DEFINE_AGENCY( CSecondErrorSkipNtmain_3Agency,
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && _owner_core_agent->GetCurrentMainTopicAgent() != NULL &&
        _owner_core_agent->GetCurrentMainTopicAgent()->GetErrorHandle() == SECOND_SKIP_NT_MAIN_3
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

DEFINE_AGENCY( CThirdErrorAgency,
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && _owner_core_agent->GetCurrentMainTopicAgent() != NULL &&
        _owner_core_agent->GetCurrentMainTopicAgent()->GetErrorHandle() != FIFTH_SKIP_NT_MAIN_2
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( ThirdErrorPromptInform, CErrorNotReqInform, "none")
        SUBAGENT( ThirdErrorTransferAgency, CThirdErrorTransferAgency, "none")
    )
)

DEFINE_AGENCY( CFifthErrorAgency,
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && ( (int)C("consecutive_error_num") == 4 && (int)C("last_error_turn") == _owner_core_agent->GetLastInputTurnNumber() )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( FifthErrorPromptInform, CErrorNotReqInform, "none")
        SUBAGENT( SkipNtMain_2, CSkipNtMain_2, "none")
    )
)

DEFINE_AGENCY( CThirdErrorTransferAgency,
    IS_NOT_DTS_AGENT()
    SUCCESS_POLICY_IS_SELECT()

    DEFINE_SUBAGENTS(
        SUBAGENT( ErrorTrasferIVRAgency, CErrorTrasferIVRAgency, "none")
        //SUBAGENT( ErrorSMSPasswordChangeAgency, CErrorSMSPasswordChangeAgency, "none")
        SUBAGENT( ErrorSkipNtmain_2Agency, CErrorSkipNtmain_2Agency, "none")
        //SUBAGENT( ErrorSkipNtmain_12Agency, CErrorSkipNtmain_12Agency, "none")
        SUBAGENT( ErrorSkipNtmain_3Agency, CErrorSkipNtmain_3Agency, "none")
    )
)

DEFINE_AGENCY( CErrorTrasferIVRAgency,
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && _owner_core_agent->GetCurrentMainTopicAgent() != NULL &&
        _owner_core_agent->GetCurrentMainTopicAgent()->GetErrorHandle() == "TrasferIVR"
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SkipTrasferIVR, CSkipTrasferIVR, "none")
    )
)

DEFINE_AGENCY( CErrorSMSPasswordChangeAgency,
    IS_NOT_DTS_AGENT()
    PRECONDITION(
        _owner_core_agent != NULL && _owner_core_agent->GetCurrentMainTopicAgent() != NULL &&
        _owner_core_agent->GetCurrentMainTopicAgent()->GetErrorHandle() == THIRD_SEND_SMS_TO_NT_MAIN_3
    )

    DEFINE_SUBAGENTS(
        SUBAGENT( ExecuteSendSMS, CNotDTSAgentExecuteSendSMS, "none")
        SUBAGENT( SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

DEFINE_AGENCY( CErrorSkipNtmain_2Agency,
    PRECONDITION(
        _owner_core_agent != NULL && _owner_core_agent->GetCurrentMainTopicAgent() != NULL &&
        _owner_core_agent->GetCurrentMainTopicAgent()->GetErrorHandle() == THIRD_TO_NT_MAIN_2
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SkipNtMain_2, CSkipNtMain_2, "none")
    )
)

DEFINE_AGENCY( CErrorSkipNtmain_12Agency,
    PRECONDITION(
        _owner_core_agent != NULL && _owner_core_agent->GetCurrentMainTopicAgent() != NULL &&
        _owner_core_agent->GetCurrentMainTopicAgent()->GetErrorHandle() == THIRD_TO_NT_MAIN_12
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SkipNtMain_12, CSkipNtMain_12, "none")
    )
)

DEFINE_AGENCY( CErrorSkipNtmain_3Agency,
    DEFINE_SUBAGENTS(
        SUBAGENT( SkipNtMain_3, CSkipNtMain_3, "none")
    )
)


DEFINE_OPERATE_AGENT( CRecoveryErrorCount,
    RESET_ON_COMPLETION
    OPERATE(
        C("consecutive_error_num") = 0;
    )
)

DEFINE_TASK_SKIP_AGENT( CSkipNoUnderstand,
    DIALOG_TYPE( CNoUnderstandAgency )
    DIALOG_NAME( NoUnderstandAgency )
)

DEFINE_TASK_SKIP_AGENT( CSkipTimeout,
    DIALOG_TYPE( CTimeoutAgency )
    DIALOG_NAME( TimeoutAgency )
)

DEFINE_OPERATE_AGENT( CTriggerNoUnderstand,
    OPERATE(
        C("action") = "拒识";
        C("target") = "1";
        if ( _owner_core_agent != NULL ) {
            _owner_core_agent->SignalFocusClaimsPhase();
        } else {
            VLOG(FATAL) << __FUNCTION__ << ", _owner_core_agent is NULL, AgentName: " << _name;
        }
    )
)

void RegisterErrorHandle() {
    DECLARE_AGENT( CErrorHandleAgency)
    DECLARE_AGENT( CErrorInform)
    DECLARE_AGENT( CErrorNotReqInform)
    DECLARE_AGENT( CNoUnderstandAgency)
    DECLARE_AGENT( CFirstNoUnderstandAgency)
    DECLARE_AGENT( CErrorSkipNtmain_2Agency)
    DECLARE_AGENT( CSecondNoUnderstandAgency)
    DECLARE_AGENT( CThirdNoUnderstandAgency)
    DECLARE_AGENT( CFourthNoUnderstandAgency)
    DECLARE_AGENT( CTimeoutAgency)
    DECLARE_AGENT( CFirstTimeoutAgency)
    DECLARE_AGENT( CSecondTimeoutAgency)
    DECLARE_AGENT( CThirdTimeoutAgency)
    DECLARE_AGENT( CFourthTimeoutAgency)
    DECLARE_AGENT( CSecondErrorSkipNtmain_3Agency)
    DECLARE_AGENT( CThirdErrorAgency)
    DECLARE_AGENT( CFifthErrorAgency)
    DECLARE_AGENT( CErrorSkipNtmain_3Agency)
    DECLARE_AGENT( CThirdErrorTransferAgency)
    DECLARE_AGENT( CErrorTrasferIVRAgency)
    DECLARE_AGENT( CErrorSMSPasswordChangeAgency)
    DECLARE_AGENT( CRecoveryErrorCount)
    DECLARE_AGENT( CSkipNoUnderstand)
    DECLARE_AGENT( CSkipTimeout)
    DECLARE_AGENT( CTriggerNoUnderstand)
}

}
