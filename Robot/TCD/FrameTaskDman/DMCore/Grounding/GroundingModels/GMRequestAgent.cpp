#include "GMRequestAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/BasicAgents/MARequest.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/CoreAgents/AllCoreAgents.h"

namespace TrioTDM {

CGMRequestAgent::CGMRequestAgent(const std::string& sAModelPolicy, const std::string& sAName): 
    CGroundingModel(sAModelPolicy, sAName) {
    _request_agent = NULL;
    // this model has 3 states
    _belief_state.Resize(3);
}

CGMRequestAgent::CGMRequestAgent(const CGMRequestAgent& rAGMRequestAgent) {
    _model_policy = rAGMRequestAgent._model_policy;
    _name = rAGMRequestAgent._name;
    _policys = rAGMRequestAgent._policys;
    _action_mappings = rAGMRequestAgent._action_mappings;
    _exploration_mode = rAGMRequestAgent._exploration_mode;
    _exploration_parameter = rAGMRequestAgent._exploration_parameter;
    _request_agent = rAGMRequestAgent._request_agent;
    _full_state = rAGMRequestAgent._full_state;
    _belief_state = rAGMRequestAgent._belief_state;
}

CGMRequestAgent::~CGMRequestAgent() {
}

CGroundingModel* CGMRequestAgent::GroundingModelFactory(const std::string& _model_policy) {
    return new CGMRequestAgent(_model_policy);
}

std::string CGMRequestAgent::GetType() {
    return "request_default";
}

void CGMRequestAgent::SetName(const std::string& sAName) {
}

std::string CGMRequestAgent::GetName() {
    if (_request_agent != NULL) {
        return _request_agent->GetName();
    } else {
        return "UNKNOWN";
    }
}

void CGMRequestAgent::SetRequestAgent(CDialogAgent* pARequestAgent) {
    _request_agent = pARequestAgent;
}

CDialogAgent* CGMRequestAgent::GetRequestAgent() {
    return _request_agent;
}

CGroundingModel* CGMRequestAgent::Clone() {
    return new CGMRequestAgent(*this);
}

bool CGMRequestAgent::LoadPolicy() {
    if(!CGroundingModel::LoadPolicy()) {
        return false;
    } else if(!_is_external_policy) {
        // then check that the model has the presumed state-space
        if(_policys.size() != 3) {
            return false;
        } else if((_policys[0].sStateName != SS_FAILED) ||
                  (_policys[1].sStateName != SS_UNDERSTANDING) ||
                  (_policys[2].sStateName != SS_NONUNDERSTANDING)) {
            return false;
        }
    }
    return true;
}

void CGMRequestAgent::ComputeFullState() {
    _full_state.Clear();
    // set the failed variable
    _full_state["failed"] = BoolToString(_request_agent->HasFailed());
    // set the nonunderstanding state variable
    _full_state["nonunderstanding"] = 
        BoolToString(CDMCoreAgent::Instance()->LastTurnNonUnderstanding());

    // now go through the last input data and add all that to the state
    // _full_state.Add(pInteractionEventManager->GetLastInput()->GetProperties());
}

// D: Compute the belief state for this model
void CGMRequestAgent::ComputeBeliefState() {
    if(_full_state["failed"] == "true") {
        _belief_state[SI_FAILED] = 1;
        _belief_state[SI_NONUNDERSTANDING] = 0;
        _belief_state[SI_UNDERSTANDING] = 0;
    } else {
        // o/w check the non-understanding/understanding        
        _belief_state[SI_FAILED] = 0;
        // the state is non-understanding if the last turn was a 
        // non-understanding
        _belief_state[SI_NONUNDERSTANDING] = 
            (_full_state["nonunderstanding"] == "true")?(float)1:(float)0;
        // the state is understanding otherwise
        _belief_state[SI_UNDERSTANDING] = 
            1 - _belief_state[SI_NONUNDERSTANDING];
    }

    // and invalidate the suggested action (not computed yet for this state)
    _suggested_action_index = -1;
}

void CGMRequestAgent::RunAction(int iActionIndex) {
    CGroundingManagerAgent::Instance()->operator [](iActionIndex)->
        Run((void *)_request_agent);
}

}
