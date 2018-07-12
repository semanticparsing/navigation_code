#include "GMConcept.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Concepts/Concept.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/CoreAgents/AllCoreAgents.h"

namespace TrioTDM {

CGMConcept::CGMConcept(const std::string& model_policy, const std::string& name) :
    CGroundingModel(model_policy, name) {
    _concept = NULL;
    _belief_state.Resize(4);
}

CGMConcept::CGMConcept(CGMConcept& gm_concept) {
    _model_policy = gm_concept._model_policy;
    _name = gm_concept._name;
    _policys = gm_concept._policys;
    _action_mappings = gm_concept._action_mappings;
    _exploration_mode = gm_concept._exploration_mode;
    _exploration_parameter = gm_concept._exploration_parameter;
    _concept = gm_concept._concept;
    _full_state = gm_concept._full_state;
    _belief_state = gm_concept._belief_state;
}

CGMConcept::~CGMConcept() {
}

CGroundingModel* CGMConcept::GroundingModelFactory(const std::string& model_policy) {
    return new CGMConcept(model_policy);
}

std::string CGMConcept::GetType() {
    return "concept_default";
}

void CGMConcept::SetName(const std::string& name) {
    VLOG(WARNING) << "Cannot perform SetName on a concept grounding model.";
}

std::string CGMConcept::GetName() {
    if ( _concept != NULL ) {
        return _concept->GetAgentQualifiedName();
    } else {
        return "UNKNOWN";
    }
}

void CGMConcept::SetConcept(CConcept* concept) {
    _concept = concept;
}

CConcept* CGMConcept::GetConcept() {
    return _concept;
}

CGroundingModel* CGMConcept::Clone() {
    return new CGMConcept(*this);
}

bool CGMConcept::LoadPolicy() {
    if ( !CGroundingModel::LoadPolicy() ) {
        return false;
    } else if ( _is_external_policy ) {
        if ( _policys.size() != 4 ) {
            VLOG(WARNING) << "LoadPolicy _policys.size() != 4";
            return false;
        } else if ( (_policys[0].sStateName != SS_INACTIVE) ||
                (_policys[1].sStateName != SS_CONFIDENT) ||
                (_policys[2].sStateName != SS_UNCONFIDENT) ||
                (_policys[3].sStateName != SS_GROUNDED))  {
            VLOG(WARNING) << "LoadPolicy Invalid state-space.";
            return false;
        }
    }
    return true;
}

void CGMConcept::RunAction(int index) {
    if ( CGroundingManagerAgent::Instance() != NULL ) {
        CGroundingManagerAgent::Instance()->operator[](index)->Run((void*)_concept);
    }
}

void CGMConcept::LogStateAction() {
    CGroundingModel::LogStateAction();
}

void CGMConcept::ComputeFullState() {
    _full_state.Clear();
    _full_state["updated"] = BoolToString(_concept->IsUpdated());
    _full_state["grounded"] = BoolToString(_concept->IsGrounded());
    _full_state["top_confidence"] = FloatToString(_concept->GetTopHypConfidence());
}

void CGMConcept::ComputeBeliefState() {
    _belief_state[SI_INACTIVE] = (_full_state["updated"] == "false" );
    _belief_state[SI_GROUNDED] = !_belief_state[SI_INACTIVE] && ( _full_state["grounded"] == "true");
    _belief_state[SI_CONFIDENT] = (_belief_state[SI_INACTIVE] || _belief_state[SI_GROUNDED]) ? (float)0 : (1 - _belief_state[SI_CONFIDENT]);
    _suggested_action_index = -1;
}

}
