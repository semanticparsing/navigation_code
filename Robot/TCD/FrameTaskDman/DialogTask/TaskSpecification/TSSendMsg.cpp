#include "TSCommonAgent.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

DEFINE_GAIN_CONCEPT_AGENT( CGainIsOutOfService,
    CONCEPT_NAME(is_out_of_service)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::IsOff)
)

DEFINE_AGENCY( COutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SkipOutOfServiceJudge, CSkipOutOfServiceJudge, "none")
    )
)

DEFINE_INFORM_AGENT( CSendMsgInform,
    RETURN_CODE( dercContinueExecution )
)

DEFINE_EXECUTE_AGENT( CExecuteSendSMS,
    DEFINE_CONCEPTS(
        STRING_SYSTEM_CONCEPT( sms_content)
    )
    EXECUTE_FUNC( &CExecuteApi::SendMessage )
    EXECUTE_PARAM(
        if ( CNLG::Instance() != NULL ) {
            C("sms_content") = CNLG::Instance()->Generate(Prompt(), C("result"));
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", CNLG::Instance() is NULL, AgentName: " << _name;
        }
        return C("sms_content");
    )
#ifdef TEST
    public:
        TDialogExecuteReturnCode Execute(COutput* output) {
            VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
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
            // TODO Not Output
            if ( output != NULL ) {
                output->SetIsOutput(true);
                
                output->SetOutaction( Outaction() );
                output->SetModelTypeOperator( BROADCAST );
                output->SetModelTypeDTMFLength( ModelTypeDTMFLength() );
                output->SetModelTypeEnd( ModelTypeEnd() );
                output->SetTelephone( _owner_core_agent->GetTelephone() );

                output->SetSendMsg(std::string("prompt_type=2&prompt_text=") + (std::string)param);
            }

            if ( !((_owner_core_agent->GetExecuteApi()->*func)( param )) ) {
                VLOG(WARNING) << __FUNCTION__ << ", system error, AgentName: " << _name;
                return dercError;
            }
            IncrementExecuteCounter();

            //return dercContinueExecution;
            return dercTakeFloor;
        }
#endif
)

DEFINE_EXECUTE_AGENT( CNotDTSAgentExecuteSendSMS,
    DEFINE_CONCEPTS(
        STRING_SYSTEM_CONCEPT( sms_content)
    )
    EXECUTE_FUNC( &CExecuteApi::SendMessage )
    EXECUTE_PARAM(
        if ( CNLG::Instance() != NULL ) {
            C("sms_content") = CNLG::Instance()->Generate(Prompt(), C("result"));
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", CNLG::Instance() is NULL, AgentName: " << _name;
        }
        return C("sms_content");
    )

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
#ifdef TEST
        TDialogExecuteReturnCode Execute(COutput* output) {
            VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
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
            // TODO Not Output
            if ( output != NULL ) {
                output->SetIsOutput(true);
                
                output->SetOutaction( Outaction() );
                output->SetModelTypeOperator( BROADCAST );
                output->SetModelTypeDTMFLength( ModelTypeDTMFLength() );
                output->SetModelTypeEnd( ModelTypeEnd() );
                output->SetTelephone( _owner_core_agent->GetTelephone() );

                output->SetSendMsg(std::string("prompt_type=2&prompt_text=") + (std::string)param);
            }

            if ( !((_owner_core_agent->GetExecuteApi()->*func)( param )) ) {
                VLOG(WARNING) << __FUNCTION__ << ", system error, AgentName: " << _name;
                return dercError;
            }
            IncrementExecuteCounter();

            return dercContinueExecution;
        }
#endif
)

DEFINE_EXECUTE_AGENT( CNotActivateBusinessExecuteSendSMS,
    DEFINE_CONCEPTS(
        STRING_SYSTEM_CONCEPT( sms_content)
    )
    EXECUTE_FUNC( &CExecuteApi::SendMessage )
    EXECUTE_PARAM(
        if ( CNLG::Instance() != NULL ) {
            C("sms_content") = CNLG::Instance()->Generate(Prompt(), C("result"));
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", CNLG::Instance() is NULL, AgentName: " << _name;
        }
        return C("sms_content");
    )

    public:
        virtual std::string Prompt() {
            return "template NotActivateBusiness.ExecuteSendSMS";
        }
#ifdef TEST
        TDialogExecuteReturnCode Execute(COutput* output) {
            VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
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
            // TODO Not Output
            if ( output != NULL ) {
                output->SetIsOutput(true);

                output->SetOutaction( Outaction() );
                output->SetModelTypeOperator( BROADCAST );
                output->SetModelTypeDTMFLength( ModelTypeDTMFLength() );
                output->SetModelTypeEnd( ModelTypeEnd() );
                output->SetTelephone( _owner_core_agent->GetTelephone() );

                output->SetSendMsg(std::string("prompt_type=2&prompt_text=") + (std::string)param);
            }

            if ( !((_owner_core_agent->GetExecuteApi()->*func)( param )) ) {
                VLOG(WARNING) << __FUNCTION__ << ", system error, AgentName: " << _name;
                return dercError;
            }
            IncrementExecuteCounter();

            return dercContinueExecution;
        }
#endif
)
void RegisterSendMsg() {
    DECLARE_AGENT( CGainIsOutOfService)
    DECLARE_AGENT( COutOfServiceAgency)
    DECLARE_AGENT( CSendMsgInform)
    DECLARE_AGENT( CExecuteSendSMS)
    DECLARE_AGENT( CNotDTSAgentExecuteSendSMS)
    DECLARE_AGENT( CNotActivateBusinessExecuteSendSMS)
}

}
