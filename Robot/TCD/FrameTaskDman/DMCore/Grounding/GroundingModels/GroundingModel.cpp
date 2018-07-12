#include "GroundingModel.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

namespace TrioTDM {

CExternalPolicyInterface::CExternalPolicyInterface(const std::string& host) {
}

CExternalPolicyInterface::~CExternalPolicyInterface() {
}

int CExternalPolicyInterface::ComputeSuggestedActionIndex(CState& state) {
    return 0;
}

CGroundingModel::CGroundingModel(const std::string& model_policy, const std::string& model_name) {
    _model_policy = model_policy;
    _name = model_name;
    _is_external_policy = false;
    _exploration_mode = "epsilon-greedy";
    _exploration_parameter = 0.2;
    _suggested_action_index = -1;
}

CGroundingModel::CGroundingModel(CGroundingModel& grounding_model) {
    _model_policy = grounding_model._model_policy;
    _name = grounding_model._name;
    _is_external_policy = grounding_model._is_external_policy;
    _policys = grounding_model._policys;
    _action_mappings = grounding_model._action_mappings;
    _exploration_mode = grounding_model._exploration_mode;
    _exploration_parameter = grounding_model._exploration_parameter;
    _belief_state = grounding_model._belief_state;
    _full_state = grounding_model._full_state;
    _suggested_action_index = grounding_model._suggested_action_index;
}

CGroundingModel::~CGroundingModel() {
    ;
}

CGroundingModel* CGroundingModel::GroundingModelFactory(const std::string& model_policy) {
    return NULL;
}

std::string CGroundingModel::GetType() {
    return "generic";
}

std::string CGroundingModel::GetModelPolicy() {
    return _model_policy;
}

std::string CGroundingModel::GetName() {
    return _name;
}

void CGroundingModel::SetName(const std::string& name) {
    _name = name;
}

void CGroundingModel::Initialize() {
    if ( !LoadPolicy() ) {
        VLOG(WARNING) << "Invalid policy for grounding model " << _model_policy;
    }
}

bool CGroundingModel::LoadPolicy() {
    if ( CGroundingManagerAgent::Instance() == NULL ) {
        VLOG(WARNING) << "CGroundingManagerAgent::Instance() == NULL";
        return false;
    }
    std::string data = CGroundingManagerAgent::Instance()->GetPolicy(_model_policy);
    std::vector<std::string> lines;
    SplitString(data, '\n', &lines);
    bool is_exploration_mode_line = true;
    bool is_exploration_parameter_line = false;
    bool is_actions_line = false;
    bool is_external_policy_host_line = true;
    
    for ( size_t i = 0;i < lines.size(); ) {
        if ( lines[i].empty() || lines[i][0] == '%' || lines[i][0] == '/' || lines[i][0] == '#') {
            i++;
            continue;
        }
        if ( is_exploration_mode_line || is_external_policy_host_line ) {
            std::string temp1, temp2;
            SplitOnFirst(lines[i], "=", temp1, temp2);
            if ( StringToLowerASCII(Trim(temp1)) == "exploration_mode" ) {
                _is_external_policy = false;
                _exploration_mode = StringToLowerASCII(Trim(temp2));
                is_exploration_mode_line = false;
                is_exploration_parameter_line = true;
                is_external_policy_host_line = false;
            } else {
                return false;
            }
        } else if ( is_exploration_parameter_line ) {
            std::string temp1, temp2;
            SplitOnFirst(lines[i], "=", temp1, temp2);
            if ( StringToLowerASCII(Trim(temp1)) != "exploration_parameter" ) {
                return false;
            }
            _exploration_parameter = atof(Trim(temp2).c_str());
            is_exploration_parameter_line = false;
            is_actions_line = true;
        } else if ( is_actions_line ) {
            std::vector<std::string> actions;
            SplitString(lines[i], '\t', &actions);
            if ( actions.size() < 1 ) { return false; }
            for ( size_t j = 0;j < actions.size(); ++j ) {
                _action_mappings.push_back(CGroundingManagerAgent::Instance()->GroundingActionNameToIndex(actions[j]));
                is_actions_line = false;
            }
        } else if ( _is_external_policy ) {
            std::vector<std::string> values;
            SplitString(lines[i], '\t', &values);
            if ( values.size() != _action_mappings.size() + 1 ) {
                return false;
            }
            TStateActionsValues data;
            data.sStateName = values[0];
            for ( size_t j = 1;j < values.size(); ++j ) {
                if ( values[j] == "-") {
                    data.i2fActionsValues[j-1] = UNAVAILABLE_ACTION;
                } else {
                    data.i2fActionsValues[j-1] = atof(values[j].c_str());
                }
            }
            _policys.push_back(data);
        }
        ++i;
    }
    _action_values.Resize(_action_mappings.size());
    // TODO
    if( _is_external_policy ) {
        ;
    }
    return true;
}

void CGroundingModel::ComputeState() {
    ComputeFullState();
    ComputeBeliefState();
    _suggested_action_index = -1;
}

void CGroundingModel::ComputeActionValuesDistribution() {
    for ( size_t i = 0;i < _action_mappings.size(); ++i ) {
        _action_values[i] = 0;
        bool is_unavailable_action = true;
        for ( size_t j = 0;j < _policys.size(); ++i ) {
            float state_action_value = _policys[j].i2fActionsValues[i];
            if ( state_action_value != UNAVAILABLE_ACTION ) {
                if ( _belief_state[j] != 0 ) {
                    _action_values[i] += state_action_value * _belief_state[j];
                    is_unavailable_action = false;
                }
            }
        }
        if ( is_unavailable_action ) {
            _action_values[i] = UNAVAILABLE_ACTION;
        }
    }
}

int CGroundingModel::ComputeSuggestedActionIndex() {
    if ( _suggested_action_index != -1 ) {
        return _suggested_action_index;
    }
    // TODO
    if ( _is_external_policy ) {
        ;
    }
    ComputeActionValuesDistribution();
    if (  _exploration_mode == "greedy" ) {
        _suggested_action_index = _action_values.GetModeEvent();
    } else if ( _exploration_mode == "stochastic" ) {
        _suggested_action_index = _action_values.GetRandomlyDrawnEvent();
    } else if ( _exploration_mode == "epsilon-greedy" ) {
        _suggested_action_index = _action_values.GetEpsilonGreedyEvent(_exploration_parameter);
    } else if ( _exploration_mode == "soft-max" ) {
        _suggested_action_index = _action_values.GetSoftMaxEvent(_exploration_parameter);
    } else {
        VLOG(WARNING) << "Unknown exploration mode: " << _exploration_mode;
    }
    _suggested_action_index = _action_mappings[_suggested_action_index];
    return _suggested_action_index;
}

void CGroundingModel::Run() {
    ComputeSuggestedActionIndex();
    RunAction(_suggested_action_index);
}

void CGroundingModel::RunAction(int index) {
    if ( CGroundingManagerAgent::Instance() != NULL ) {
        CGroundingManagerAgent::Instance()->operator [](index)->Run();
    }
}

void CGroundingModel::LogStateAction() {
}

std::string CGroundingModel::BeliefStateToString() {
    std::string result;
    for ( size_t i = 0;i < _policys.size(); ++i ) {
        result += FormatString("%s:%.2f  ", _policys[i].sStateName.c_str(), _belief_state[i]);
    }
    return result;
}

std::string CGroundingModel::ActionValuesToString() {
    std::string result;
    if (CGroundingManagerAgent::Instance() == NULL ) {
        VLOG(WARNING) << "CGroundingManagerAgent::Instance() == NULL";
        return result;
    }

    for ( size_t i = 0;i < _action_mappings.size(); ++i ) {
        if ( _action_values[i] == UNAVAILABLE_ACTION ) {
            result += FormatString("  %s: -\n", CGroundingManagerAgent::Instance()->GroundingActionIndexToName(_action_mappings[i]).c_str());
        } else {
            if ( _action_values.LowBound(i) == INVALID_EVENT ) {
                result += FormatString("  %s:%.4f\n", CGroundingManagerAgent::Instance()->GroundingActionIndexToName(_action_mappings[i]).c_str(), _action_values[i]);
            } else {
                result += FormatString("  %s:%4.f [%4.f-%4.f]\n",
                        CGroundingManagerAgent::Instance()->GroundingActionIndexToName(_action_mappings[i]).c_str(),
                        _action_values[i], _action_values.LowBound(i),
                        _action_values.HiBound(i));
            }
        }
    }
    return result;
}

}
