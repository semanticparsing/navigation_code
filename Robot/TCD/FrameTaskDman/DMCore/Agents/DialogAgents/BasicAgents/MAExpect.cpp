#include "MAExpect.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

namespace TrioTDM {

CMAExpect::CMAExpect(const std::string& name, const std::string& configuration, const std::string& type) :
    CDialogAgent(name, configuration, type) {
    _is_completed = true;
    _completion_type = ctSuccess;
}

CMAExpect::~CMAExpect() {
}

CAgent* CMAExpect::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CMAExpect(name, configuration);
}

bool CMAExpect::PreconditionsSatisfied() {
    return false;
}

bool CMAExpect::IsExecutable() {
    return false;
}

TDialogExecuteReturnCode CMAExpect::Execute(COutput* output) {
    return dercContinueExecution;
}

int CMAExpect::DeclareExpectations(std::vector<TConceptExpectation>& expectations, bool is_must_not_dts_agent) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    int expectation_add = 0;
    std::vector<TConceptExpectation> cel_local_expectations;
    bool is_expect_condition = ExpectCondition();

    std::string expected_concept_name = ExpectedConceptName();
    std::string grammar_mapping = GrammarMapping();
    if ( !expected_concept_name.empty() && !grammar_mapping.empty() ) {
        ParseGrammarMapping(C(expected_concept_name).GetAgentQualifiedName(), grammar_mapping, cel_local_expectations);
    }
    for ( size_t i = 0;i < cel_local_expectations.size(); ++i ) {
        if ( !is_expect_condition ) {
            cel_local_expectations[i].bDisabled = true;
            cel_local_expectations[i].sReasonDisabled = "expect condition false";
        }
        VLOG(DEBUG) << "TConceptExpectation, bDisabled: " << cel_local_expectations[i].bDisabled << "\tsReasonDisabled: " << cel_local_expectations[i].sReasonDisabled;
        expectations.push_back(cel_local_expectations[i]);
        expectation_add += 1;
    }
    expectation_add += CDialogAgent::DeclareExpectations(expectations, is_must_not_dts_agent);

    return expectation_add;
}

void CMAExpect::SetCompleted(TCompletionType type) {
}

std::string CMAExpect::GrammarMapping() {
    return "";
}

std::string CMAExpect::ExpectedConceptName() {
    return "";
}

CConcept* CMAExpect::ExpectedConcept() {
    return &(C(ExpectedConceptName()));
}

}
