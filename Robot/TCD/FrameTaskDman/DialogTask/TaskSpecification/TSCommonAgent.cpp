#include "TSCommonAgent.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

DEFINE_INFORM_AGENT( CTaskPromptInform,
)
 
DEFINE_REQUEST_AGENT( CTaskPromptReqUser,
)

DEFINE_AGENCY( CRequestConfirmAgency,
    DEFINE_SUBAGENTS(
        SUBAGENT( RequestConfirmPrompt, CRequestConfirmPrompt, "none")
        SUBAGENT( RequestConfirmUserInputAgency, CRequestConfirmUserInputAgency, "none")
    )
)

DEFINE_REQUEST_AGENT( CRequestConfirmPrompt,
)

DEFINE_AGENCY( CRequestConfirmUserInputAgency,
    SUCCESS_POLICY_IS_SELECT()
    ON_EXECUTE_FAIL(
        TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( RequestConfirm, CRequestConfirm, "none")
        SUBAGENT( SpecificBusinessAgency, CSpecificBusinessAgency, "none")
    )
)

DEFINE_REQUEST_AGENT( CRequestConfirm,
    PRECONDITION( false )
    REQUEST_CONCEPT( is_need )
    GRAMMAR_MAPPING("[确认]>true")
)

DEFINE_AGENCY( CNotNeedToNtMain2Agency,
    PRECONDITION(
        C("is_need").IsUpdated() && (bool)C("is_need") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_2, CSkipNtMain_2, "none")
    )
)

DEFINE_AGENCY( CNotNeedToNtMain3Agency,
    PRECONDITION(
        C("is_need").IsUpdated() && (bool)C("is_need") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

/*
typedef std::map<std::string, std::string> TMapString2String;

DEFINE_EXECUTE_AGENT( CSkipOtherMainTopic,
    PRECONDITION( false )
    public:
        virtual bool SuccessCriteriaSatisfied() {
            CDialogAgent* business = GetBusiness();
            if ( business == NULL ) {
                VLOG(WARNING) << __FUNCTION__ << ", business is NULL.";
                return false;
            } else {
                const TMapString2String& trigger_concepts = business->GetTriggerConcepts();
                if ( trigger_concepts.empty() ) {
                    VLOG(FATAL) << __FUNCTION__ << ", trigger concepts of business is empty...";
                    return false;
                }
                for ( TMapString2String::const_iterator c_iter = trigger_concepts.begin();
                        c_iter != trigger_concepts.end(); ++c_iter ) {
                    if ( (std::string)C(c_iter->first) != c_iter->second ) {
                        return true;
                    }
                }
                return false;
            }
        }
)
*/

void RegisterCommonAgent() {
    DECLARE_AGENT(CTaskPromptInform)
    DECLARE_AGENT(CTaskPromptReqUser)
    DECLARE_AGENT(CRequestConfirmAgency)
    DECLARE_AGENT(CRequestConfirmUserInputAgency)
    DECLARE_AGENT(CRequestConfirmPrompt)
    DECLARE_AGENT(CRequestConfirm)
    DECLARE_AGENT( CNotNeedToNtMain2Agency)
    DECLARE_AGENT( CNotNeedToNtMain3Agency)
}

}
