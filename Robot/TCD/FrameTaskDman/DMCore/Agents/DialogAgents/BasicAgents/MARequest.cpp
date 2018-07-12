#include "MARequest.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Outputs/NLG.h"

namespace TrioTDM {

CMARequest::CMARequest(const std::string& name, const std::string& configuration, const std::string& type) :
    CDialogAgent(name, configuration, type) {
        ;
}

CMARequest::~CMARequest() {
    ;
}

CAgent* CMARequest::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CMARequest(name, configuration);
}

TDialogExecuteReturnCode CMARequest::Execute(COutput* output) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    IncrementExecuteCounter();
    if ( CNLG::Instance() == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", CNLG::Instance() is NULL.";
        return dercError;
    }
    if ( output == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", output is NULL.";
        return dercError;
    }
    if ( _owner_core_agent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _owner_core_agent is NULL.";
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
    } else {
        VLOG(WARNING) << __FUNCTION__ <<  ", Execute fail, prompt: " << prompt;
        return dercError;
    }
    
    if ( _owner_core_agent->GetOutputManagerAgent() != NULL ) {
        _owner_core_agent->GetOutputManagerAgent()->AddOutputs(_owner_core_agent->GetSession(), *output);
    } else {
        VLOG(FATAL) << __FUNCTION__ << ", OutputManagerAgent is NULL.";
    }
    _owner_core_agent->SetTimeoutPeriod(GetTimeoutPeriod());
    _owner_core_agent->SetNonunderstandingThreshold(GetNonunderstandingThreshold());

    return dercWaitForEvent;
}

void CMARequest::Reset() {
    CDialogAgent::Reset();
}

int CMARequest::DeclareExpectations(std::vector<TConceptExpectation>& expectations, bool is_must_not_dts_agent) {
    int expectations_added = 0;
    std::vector<TConceptExpectation> cel_local_expectations;
    bool is_expect_conition = ExpectCondition();

    std::string requested_concept_name = RequestedConceptName();
    std::string grammar_mapping = GrammarMapping();
    if ( !requested_concept_name.empty() && !grammar_mapping.empty() ) {
        ParseGrammarMapping(requested_concept_name, grammar_mapping, cel_local_expectations);
    }
    for ( size_t i = 0;i < cel_local_expectations.size(); ++i ) {
        if ( !is_expect_conition ) {
            cel_local_expectations[i].bDisabled = true;
            cel_local_expectations[i].sReasonDisabled = "expect condition false";
        }
        expectations.push_back(cel_local_expectations[i]);
        expectations_added += 1;
    }
    expectations_added += CDialogAgent::DeclareExpectations(expectations, is_must_not_dts_agent);

    return expectations_added;
}

bool CMARequest::PreconditionsSatisfied() {
   if ( RequestedConceptName() != "" ) {
       return !RequestedConcept()->IsAvailableAndGrounded();
   } else {
       return true;
   }
}

bool CMARequest::SuccessCriteriaSatisfied() {
    if ( RequestedConceptName() != "" ) {
        return RequestedConcept()->IsUpdatedAndGrounded();
    } else {
        if ( _owner_core_agent == NULL ) {
            return false;
        }
        // return ( (_turns_in_focus_counter > 0) && !_owner_core_agent->LastTurnNonUnderstanding());
        return (_turns_in_focus_counter > 0);
    }
}

bool CMARequest::FailureCriteriaSatisfied() {
    bool is_failed = ( _turns_in_focus_counter >= GetMaxExecuteCounter()) && !SuccessCriteriaSatisfied();
    return is_failed;
}

int CMARequest::ModelTypeOperator() {
    return BROADCAST | RECOGNITION;
}

void CMARequest::ClearOtherUserConcepts() {
    std::set<std::string> need_retain_concepts;
    if ( RequestedConceptName() != "" ) {
        std::string concept_name, foo;
        SplitOnFirst(RequestedConceptName(), ";", concept_name, foo);
        need_retain_concepts.insert( concept_name );
    }
    ClearConcepts( csUser, need_retain_concepts, 0, 2);
}

void CMARequest::UnSignalFocus() {
    if ( _owner_core_agent != NULL ) {
        _owner_core_agent->SignalFocusClaimsPhase(false);
    }
}

std::string CMARequest::GrammarMapping() {
    return "";
}

std::string CMARequest::RequestedConceptName() {
    return "";
}

CConcept* CMARequest::RequestedConcept() {
    if ( RequestedConceptName() != "" ) {
        std::string concept_name, foo;
        SplitOnFirst(RequestedConceptName(), ";", concept_name, foo);
        return &C(concept_name);
    } else {
        return &CConcept::NULLConcept;
    }
}

int CMARequest::GetTimeoutPeriod() {
    if ( _owner_core_agent != NULL ) {
        return _owner_core_agent->GetDefaultTimeoutPeriod();
    }
    return 0;
}

float CMARequest::GetNonunderstandingThreshold() {
    if ( _owner_core_agent != NULL ) {
        return _owner_core_agent->GetDefaultNonunderstandingThreshold();
    }
    return 0.0;
}


}
