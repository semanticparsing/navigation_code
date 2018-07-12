#include "TSPrimaryMenu.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

DEFINE_AGENCY( CPrimaryMenuAgency,
    IS_MAIN_TOPIC()
    ERROR_HANDLE( "TrasferIVR" )
    DEFINE_SUBAGENTS(
        SUBAGENT( SkipPrimaryMenuAgency, CSkipPrimaryMenuAgency, "none")
    )
)

DEFINE_AGENCY( CReturnPrimaryMenuAgency,
    PRECONDITION( false )
    IS_MAIN_TOPIC()
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        SUBAGENT( NtMainAgency_2, CSkipNtMain_2, "none")
    )
)

// 跳转到主菜单，上文会被清空
DEFINE_AGENCY( CSkipPrimaryMenuAgency,
    PRECONDITION(
        (int)C("nt_main") == 1 || (int)C("nt_main") == 2 || \
        (int)C("nt_main") == 3 || (int)C("nt_main") == 4 || \
        (int)C("nt_main") == 5 || (int)C("nt_main") == 7 || \
        (int)C("nt_main") == 8
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( RequestUserInput, CRequestUserInput, "none")
        SUBAGENT( PossibleUserReqAgency, CPossibleUserReqAgency, "none")
        SUBAGENT( TriggerSpecificBusinessAgency, CTriggerSpecificBusinessAgency, "none")
    )
)

DEFINE_AGENCY( CNtMain6PrimaryMenuAgency,
    IS_MAIN_TOPIC()
    ERROR_HANDLE( "TrasferIVR" )
    DEFINE_SUBAGENTS(
        SUBAGENT( nt_main_6, CTaskPromptReqUser, "none")
        SUBAGENT( NtMain5PossibleUserReqAgency, CNtMain6PossibleUserReqAgency, "none")
        SUBAGENT( TriggerSpecificBusinessAgency, CTriggerSpecificBusinessAgency, "none")
    )
)

DEFINE_REQUEST_AGENT( CRequestUserInput,
    PROMPT( "template nt_main." + (std::string)C("nt_main") )
)

DEFINE_AGENCY( CPossibleUserReqAgency,
    SUCCESS_POLICY_IS_SELECT()
    ON_EXECUTE_FAIL(
        TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SpecificBusinessAgency, CSpecificBusinessAgency, "none")
    )
)

DEFINE_AGENCY( CTriggerSpecificBusinessAgency,
    SUCCESS_POLICY_IS_SELECT()
    ON_EXECUTE_FAIL(
       TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT 
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( TriggerSpecificBusinessExecute, CTriggerSpecificBusinessExecute, "none")
    )
)

DEFINE_EXECUTE_AGENT( CTriggerSpecificBusinessExecute,
    PRECONDITION( false )
    public:
        virtual bool SuccessCriteriaSatisfied() {
            if ( _owner_core_agent == NULL ) {
                VLOG(FATAL) << __FUNCTION__ << ", _owner_core_agent is NULL, AgentName: " << _name;
                return false;
            }
            return _owner_core_agent->IsTriggerNewBusiness();
        }
)

DEFINE_AGENCY( CNtMain6PossibleUserReqAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT( SpecificBusinessAgency, CSpecificBusinessAgency, "none")
    )
)

DEFINE_AGENCY( CSpecificBusinessAgency,
    PRECONDITION(false)
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        //SUBAGENT( ExpectConceptAgency, CExpectConceptAgency, "none")
        SUBAGENT( RequestHandle, CRequestAction, "none")
        SUBAGENT( RequestTarget, CRequestTarget, "none")
        SUBAGENT( RequestEntityTag, CRequestEntityTag, "none")
    )
)

DEFINE_REQUEST_AGENT( CRequestAction,
    REQUEST_CONCEPT( action )
    GRAMMAR_MAPPING("*[action]")
)

DEFINE_REQUEST_AGENT( CRequestTarget,
    REQUEST_CONCEPT( target )
    GRAMMAR_MAPPING("*[target]")
)

DEFINE_EXPECT_AGENT( CRequestEntityTag,
    EXPECT_CONCEPT( entity_tag )
    GRAMMAR_MAPPING( "*[entity_tag]" )
)

DEFINE_REQUEST_AGENT( CRequestBusinessesBroadcast,
    PRECONDITION( false )
    RESET_ON_COMPLETION
    TRIGGERED_BY_COMMANDS("[businesses]", "none")
    PROMPT( std::string("template PrimaryMenu.businesses") )
)

#define NT_MAIN_AGENCY(Value) \
    DEFINE_OPERATE_AGENT(\
        CModifyNtMain_##Value,\
        OPERATE(\
            C("nt_main") = Value;\
        )\
    )\
    DEFINE_AGENCY( CNtMainAgency_##Value,\
        IS_MAIN_TOPIC() \
        RESET_ON_COMPLETION\
        DEFINE_CONCEPTS(\
            INT_SYSTEM_CONCEPT(nt_main)\
        )\
        DEFINE_SUBAGENTS(\
            SUBAGENT( ModifyNtMain_##Value, CModifyNtMain_##Value, "none")\
            SUBAGENT( PrimaryMenu, CPrimaryMenuAgency, "none")\
        )\
    )\

NT_MAIN_AGENCY(1)
NT_MAIN_AGENCY(2)
NT_MAIN_AGENCY(3)
NT_MAIN_AGENCY(4)
NT_MAIN_AGENCY(5)
NT_MAIN_AGENCY(7)
NT_MAIN_AGENCY(8)

#define SKIP_NT_MAIN( Value ) \
    DEFINE_TASK_SKIP_AGENT( CSkipNtMain_##Value,\
        DIALOG_TYPE( CNtMainAgency_##Value )\
        DIALOG_NAME( NtMainAgency_##Value )\
    )\

SKIP_NT_MAIN(1)
SKIP_NT_MAIN(2)
SKIP_NT_MAIN(3)
SKIP_NT_MAIN(4)
SKIP_NT_MAIN(5)
SKIP_NT_MAIN(7)
SKIP_NT_MAIN(8)

DEFINE_AGENCY( CQuitAgency,
    PRECONDITION( false )
    IS_MAIN_TOPIC()
    IS_BUSINESS()

    DEFINE_SUBAGENTS(
        SUBAGENT( RequestTrasferIVRFloor, CRequestTrasferIVRFloor, "none")
    )
)

void RegisterNtMain() {
    DECLARE_AGENT(CModifyNtMain_1)
    DECLARE_AGENT(CModifyNtMain_2)
    DECLARE_AGENT(CModifyNtMain_3)
    DECLARE_AGENT(CModifyNtMain_4)
    DECLARE_AGENT(CModifyNtMain_5)
    DECLARE_AGENT(CModifyNtMain_7)
    DECLARE_AGENT(CModifyNtMain_8)

    DECLARE_AGENT(CNtMainAgency_1)
    DECLARE_AGENT(CNtMainAgency_2)
    DECLARE_AGENT(CNtMainAgency_3)
    DECLARE_AGENT(CNtMainAgency_4)
    DECLARE_AGENT(CNtMainAgency_5)
    DECLARE_AGENT(CNtMainAgency_7)
    DECLARE_AGENT(CNtMainAgency_8)

    DECLARE_AGENT(CSkipNtMain_1)
    DECLARE_AGENT(CSkipNtMain_2)
    DECLARE_AGENT(CSkipNtMain_3)
    DECLARE_AGENT(CSkipNtMain_4)
    DECLARE_AGENT(CSkipNtMain_5)
    DECLARE_AGENT(CSkipNtMain_7)
    DECLARE_AGENT(CSkipNtMain_8)

}

void RegisterPrimaryMenu() {
    RegisterNtMain();

    DECLARE_AGENT( CPrimaryMenuAgency)
    DECLARE_AGENT( CReturnPrimaryMenuAgency)
    DECLARE_AGENT( CSkipPrimaryMenuAgency)
    DECLARE_AGENT( CNtMain6PrimaryMenuAgency)
    DECLARE_AGENT( CNtMain6PossibleUserReqAgency)
    DECLARE_AGENT( CRequestUserInput)
    DECLARE_AGENT( CPossibleUserReqAgency)
    DECLARE_AGENT( CTriggerSpecificBusinessAgency)
    DECLARE_AGENT( CTriggerSpecificBusinessExecute)
    DECLARE_AGENT( CSpecificBusinessAgency)
    DECLARE_AGENT( CRequestAction)
    DECLARE_AGENT( CRequestTarget)
    DECLARE_AGENT( CRequestEntityTag)
    DECLARE_AGENT( CRequestBusinessesBroadcast)
    DECLARE_AGENT( CQuitAgency)
}

}
