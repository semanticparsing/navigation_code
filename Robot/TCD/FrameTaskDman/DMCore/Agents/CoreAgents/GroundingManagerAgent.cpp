#include "GroundingManagerAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Common/Util/File/simpleLineReader.h"

namespace TrioTDM {

CGroundingManagerAgent* CGroundingManagerAgent::s_instance = NULL;

static std::string GRS[] = {"UNPROCESSD", "PENDING", "READY", "SCHEDULED", "ONSTACK", "EXECUTING", "DONE"};

CGroundingManagerAgent::CGroundingManagerAgent(const std::string& name, const std::string& configuration, const std::string& type) :
    CAgent(name, configuration, type) {
        _is_turn_grounding_request = false;
        _is_locked_grounding_requests = false;
        SetConfiguration(configuration);
}

CGroundingManagerAgent::~CGroundingManagerAgent() {
    ReleaseExternalPolicyInterfaces();
}

CAgent* CGroundingManagerAgent::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CGroundingManagerAgent(name, configuration);
}

CGroundingManagerAgent* CGroundingManagerAgent::Instance() {
    if ( s_instance == NULL && CRegistryAgentType::Instance() != NULL ) {
        s_instance = (CGroundingManagerAgent*)CRegistryAgentType::Instance()->CreateAgent("CGroundingManagerAgent", "GroundingManagerAgent");
    }
    return s_instance;
}

void CGroundingManagerAgent::SetConfiguration(const std::string& configuration) {
    VLOG(DEBUG) << "CGroundingManagerAgent::SetConfiguration..";
    _config.sGroundingManagerConfiguration = StringToLowerASCII(configuration);
    std::vector<std::string> tokens;
    SplitString(_config.sGroundingManagerConfiguration, ';', &tokens);
    _config.bGroundConcepts = false;
    _config.bGroundTurns = false;
    _config.sBeliefUpdatingModelName = "npu";

    for (size_t i = 0;i < tokens.size(); ++i ) {
        if ( tokens[i] == "no_grounding" ) {
            _config.bGroundConcepts = false;
            _config.bGroundTurns = false;
            _config.sBeliefUpdatingModelName = "npu";
            break;
        } else {
            std::string slot(""), value("default");
            SplitOnFirst(tokens[i], ":", slot, value);
            if ( slot == "concepts" ) {
                if ( StringToLowerASCII(value) == "default" ) {
                    value = "concept_default";
                }
                _config.sConceptGM = value;
                _config.bGroundConcepts = !(value == "none");
            } else if ( slot == "turns" ) {
                if ( StringToLowerASCII(value) == "default" ) {
                    value = "request_default";
                }
                _config.sTurnGM = value;
                _config.bGroundTurns = !(value == "none");
            } else if ( slot == "beliefupdatingmodel" ) {
                SetBeliefUpdatingModelName(value);
            }

        }
    }
}

TGroundingManagerConfiguration CGroundingManagerAgent::GetConfiguration() {
    return _config;
}

void CGroundingManagerAgent::LoadPoliciesFromString(const std::string& strg) {
    std::vector<std::string> policies;
    SplitString(strg, ';', &policies);

    for ( size_t i = 0;i < policies.size(); ++i ) {
        std::string model_name, model_file_name, model_data;
        if ( SplitOnFirst(policies[i], "=", model_name, model_file_name) ) {
            model_name = StringToLowerASCII(Trim(model_name));
            model_data = LoadPolicy(Trim(model_file_name));
            if ( model_data.empty() ) {
                VLOG(WARNING) << "Unable to load policy for " << policies[i];
            } else {
                _policies.insert(std::make_pair(model_name, model_data));
            }
        }
    }
}

void CGroundingManagerAgent::LoadPoliciesFromFile(const std::string& file) {
}

std::string CGroundingManagerAgent::GetPolicy(const std::string& model_name) {
    return _policies[model_name];
}

CExternalPolicyInterface* CGroundingManagerAgent::CreateExternalPolicyInterface(const std::string& host) {
    return NULL;
}

void CGroundingManagerAgent::ReleaseExternalPolicyInterfaces() {
}

void CGroundingManagerAgent::LoadBeliefUpdatingModel(const std::string& file_name) {
    file::SimpleLineReader reader(true);
    if (!reader.Open(file_name)) {
        VLOG(FATAL) << "Can not read file, " << file_name;
        return ;
    }
    std::vector<std::string> lines;
    reader.ReadLines(&lines);

    for (size_t i = 0;i < lines.size(); ++i ) {
        std::string line = Trim(lines[i]);
        std::string model_name;
        std::string model_file_name;

        if ( line[0] == '#' || (line[0] == '/' && line[1] == '/') ) {
            continue;
        }
        if ( StringToLowerASCII(line.substr(0, 9)) == "constants" ) {
            for ( i = i+1;i < lines.size() && StringToLowerASCII(Trim(lines[i])) != "end"; ++i  ) {
                line = Trim(lines[i]);
                std::string param, coef;
                SplitOnFirst(line, "=", param, coef);
                _constant_parameters.insert(std::make_pair(Trim(param), atof(coef.c_str())));
            }
        } else if ( StringToLowerASCII(line.substr(0, 8)) == "lr_model" ) {
            std::string foo, action;
            SplitOnFirst(line, "(", foo, action);
            action = TrimRight(action, "\n)");

            if ( (action != SA_REQUEST) && (action != SA_EXPL_CONF) &&
                    (action != SA_IMPL_CONF) && (action != SA_OTHER) &&
                    (action != SA_UNPLANNED_IMPL_CONF)) {
                VLOG(WARNING) << "Error loading belief updating model, Unknow action " << action;
                return ;
            }
            std::map<std::string, std::vector<float> > s2vf_mr_model;
            for (i = i+1;i < lines.size() && StringToLowerASCII(Trim(lines[i])) != "end"; ++i ) {
                line = Trim(lines[i]);
                std::string param, coef;
                SplitOnFirst(line, "=", param, coef);
                std::vector<std::string> coefs;
                SplitString(coef, ';', &coefs);
                std::vector<float> f_coefs;
                for ( size_t j = 0;j < coefs.size(); ++j ) {
                    f_coefs.push_back(atof(coefs[j].c_str()));
                }
                s2vf_mr_model.insert(std::make_pair(StringToLowerASCII(Trim(param)), f_coefs));
            }
            _belief_updating_models.insert(std::make_pair(action, s2vf_mr_model));
        } else if ( StringToLowerASCII(line.substr(0, 14)) == "concept_values" ) {
            std::string foo, foo2, concept, concept_type;
            SplitOnFirst(line, "(", foo, foo2);
            foo2 = TrimRight(foo2, "\n)");
            SplitOnFirst(foo2, ":", concept, concept_type);
            std::map<std::string, std::vector<float> > s2vf_concept_values;
            for ( i = i+1;i < lines.size() && StringToLowerASCII(Trim(lines[i])) != "end"; ++i ) {
                line = Trim(lines[i]);
                std::vector<std::string> items;
                SplitString(line, '\t', &items);
                std::vector<float> f_items;
                for (size_t j = 0;j < items.size(); ++j ) {
                    f_items.push_back(atof(items[j].c_str()));
                }
                s2vf_concept_values.insert(std::make_pair(StringToLowerASCII(Trim(items[0])), f_items));
            }
            _belief_updating_models.insert(std::make_pair(concept, s2vf_concept_values));
        }
    }
}

void CGroundingManagerAgent::SetBeliefUpdatingModelName(const std::string& model_name) {
}

std::string CGroundingManagerAgent::GetBeliefUpdatingModelName() {
    return _config.sBeliefUpdatingModelName;
}

std::map<std::string, std::vector<float> >& CGroundingManagerAgent::GetBeliefUpdatingModelForAction(const std::string& system_action) {
    static std::map<std::string, std::vector<float> > not_found_ret;
    std::map<std::string, std::map<std::string, std::vector<float> > >::iterator f_iter = _belief_updating_models.find(system_action);
    if ( f_iter == _belief_updating_models.end() ) {
        VLOG(WARNING) << "Could not find belief updating model for action " << system_action;
        return not_found_ret;
    }
    return f_iter->second;
}

float CGroundingManagerAgent::GetConstantParameter(const std::string& param) {
    std::map<std::string, float>::iterator f_iter = _constant_parameters.find(param);
    if ( f_iter == _constant_parameters.end() ) {
        VLOG(WARNING) << "Could not find const parameter " << param;
        return 0.0;
    } else {
        return f_iter->second;
    }
}

// TODO
void CGroundingManagerAgent::PrecomputeBeliefUpdatingFeatures(CConcept* i_concept, CConcept* new_concept, const std::string& system_action) {
    if ( i_concept == NULL ) {
        VLOG(WARNING) << "Cannot precompute belief updating features on NULL concept";
        return ;
    }
}

float CGroundingManagerAgent::GetGroundingFeature(const std::string& feature_name) {
    return 0.0;
}

std::string CGroundingManagerAgent::GetGroundingFeatureAsString(const std::string& feature_name) {
    return "";
}

void CGroundingManagerAgent::ClearBeliefUpdatingFeatures() {
    _belief_updating_features.clear();
}

float CGroundingManagerAgent::GetPriorForConceptHyp(const std::string& concept, const std::string& hyp) {
    std::map<std::string, std::map<std::string, std::vector<float> > >::iterator f_iter = _concept_values_info.find(concept);
    if ( f_iter != _concept_values_info.end() ) {
        std::map<std::string, std::vector<float> >::iterator ff_iter = f_iter->second.find(StringToLowerASCII(hyp));
        if ( ff_iter != f_iter->second.end() ) {
            return (ff_iter->second)[0];
        } else {
            ff_iter = f_iter->second.find("<undefined>");
            VLOG(WARNING) << "Could not find prior info for concept " << concept << ", hypothesis " << hyp << ", defaulting to " << (ff_iter->second)[0];
            return (ff_iter->second)[0];
        }
    } else {
        VLOG(WARNING) << "Could not find prior info for concept " << concept << ", hypothesis " << hyp;
        return -1;
    }
}

float CGroundingManagerAgent::GetConfusabilityForConceptHyp(const std::string& concept, const std::string& hyp) {
    return 0.0;
}

std::string CGroundingManagerAgent::GetConceptTypeInfoForConcept(const std::string& concept) {
    std::map<std::string, std::string>::iterator f_iter = _concept_type_info.find(concept);
    if ( f_iter != _concept_type_info.end() ) {
        return f_iter->second;
    } else {
        return "unknow";
    }
}

void CGroundingManagerAgent::UseGroundingAction(const std::string& action_name, CGroundingAction* action) {
    _ga_actions.push_back(action);
    _action_names.push_back(action_name);
    action->RegisterDialogAgency();
}

int CGroundingManagerAgent::GroundingActionNameToIndex(const std::string& action_name) {
    for ( size_t i = 0;i < _action_names.size(); ++i ) {
        if ( _action_names[i] == action_name ) {
            return i;
        }
    }
    return -1;
}

std::string CGroundingManagerAgent::GroundingActionIndexToName(size_t index) {
    if ( index < _action_names.size() ) {
        return _action_names[index];
    }
    return "";
}

CGroundingAction* CGroundingManagerAgent::operator[] (const std::string& action_name) {
    return operator [] (GroundingActionNameToIndex(action_name));
}

CGroundingAction* CGroundingManagerAgent::operator[] (size_t index) {
    if ( index < _ga_actions.size() ) {
        return _ga_actions[index];
    }
    return NULL;
}

void CGroundingManagerAgent::RequestTurnGrounding(bool is_turn_grounding_request) {
    _is_turn_grounding_request = is_turn_grounding_request && _config.bGroundTurns;
}

void CGroundingManagerAgent::RequestConceptGrounding(CConcept* concept) {
    if ( !_config.bGroundConcepts ) { return ;}
    if ( _is_locked_grounding_requests ) {
        return ;
    }
    int index = GetConceptGroundingRequestIndex(concept);
    if ( index != -1 ) {
        _concept_grounding_requests.erase(_concept_grounding_requests.begin() + index);
    }
    TConceptGroundingRequest cgr;
    cgr.pConcept = concept;
    cgr.iGroundingRequestStatus = GRS_UNPROCESSED;
    cgr.iSuggestedActionIndex = -1;
    cgr.iTurnNumber = CDMCoreAgent::Instance()->GetLastInputTurnNumber();
    _concept_grounding_requests.push_back(cgr);
}

std::string CGroundingManagerAgent::ScheduleConceptGrounding(CConcept* concept) {
    RequestConceptGrounding(concept);
    int index = GetConceptGroundingRequestIndex(concept);
    LockConceptGroundingRequestsQueue();
    _concept_grounding_requests[index].iGroundingRequestStatus = GRS_PENDING;
    CGroundingModel* model = concept->GetGroundingModel();
    model->ComputeState();
    _concept_grounding_requests[index].iSuggestedActionIndex = model->ComputeSuggestedActionIndex();
    std::string action_name = GroundingActionIndexToName(_concept_grounding_requests[index].iSuggestedActionIndex);
    _concept_grounding_requests[index].pConcept->GetGroundingModel()->LogStateAction();

    if ( action_name == "EXPL_CONF" ) {
        for (size_t i = 0;i < _concept_grounding_requests.size(); ++i ) {
            TConceptGroundingRequest cgr = _concept_grounding_requests[i];
            if ( (cgr.iGroundingRequestStatus == GRS_SCHEDULED) && 
                    ( GroundingActionIndexToName(cgr.iSuggestedActionIndex) == "EXPL_CONF")) {
                _concept_grounding_requests[i].iGroundingRequestStatus = GRS_PENDING;
            }
            if ( (cgr.iGroundingRequestStatus == GRS_EXECUTING) &&
                 ( GroundingActionIndexToName(cgr.iSuggestedActionIndex) == "EXPL_CONF") ) {
                VLOG(WARNING) << "Could not schedule, concept name: " << concept->GetName();
            }
        }
    }
    _concept_grounding_requests[index].iGroundingRequestStatus = GRS_SCHEDULED;
    UnlockConceptGroundingRequestsQueue();

    return action_name;
}

void CGroundingManagerAgent::LockConceptGroundingRequestsQueue() {
    _is_locked_grounding_requests = true;
}

void CGroundingManagerAgent::UnlockConceptGroundingRequestsQueue() {
    _is_locked_grounding_requests = false;
}

void CGroundingManagerAgent::SetConceptGroundingRequestStatus(CConcept* concept, int status) {
}

int CGroundingManagerAgent::GetConceptGroundingRequestStatus(CConcept* concept) {
    int index = GetConceptGroundingRequestIndex(concept);
    if ( index == -1 ) {
        return -1;
    } else {
        return _concept_grounding_requests[index].iGroundingRequestStatus;
    }
}

void CGroundingManagerAgent::ConceptGroundingRequestCompleted(CConcept* concept) {
    int index = GetConceptGroundingRequestIndex(concept);
    if ( index != -1 && _concept_grounding_requests[index].iGroundingRequestStatus == GRS_EXECUTING ) {
        if ( _is_locked_grounding_requests ) {
            _concept_grounding_requests[index].iGroundingRequestStatus = GRS_DONE;
        } else {
            _concept_grounding_requests.erase(_concept_grounding_requests.begin() + index);
        }
    }
}

void CGroundingManagerAgent::RemoveConceptGroundingRequest(CConcept* concept) {
    int index = GetConceptGroundingRequestIndex(concept);

    if ( index != -1 ) {
        if ( _is_locked_grounding_requests ) {
            _concept_grounding_requests[index].iGroundingRequestStatus = GRS_DONE;
        } else {
            _concept_grounding_requests.erase(_concept_grounding_requests.begin() + index);
        }
    }
}

void CGroundingManagerAgent::PurgeConceptGroundingRequestsQueue() {
    bool is_something_purged = true;
    while ( is_something_purged ) {
        is_something_purged = false;
        for ( size_t i = 0;i < _concept_grounding_requests.size(); ++i ) {
            if ( _concept_grounding_requests[i].iGroundingRequestStatus == GRS_DONE ) {
                _concept_grounding_requests.erase(_concept_grounding_requests.begin() + i);
                is_something_purged = true;
                break;
            }
        }
    }
}

void CGroundingManagerAgent::GAHAddHistoryItem(const std::string& model_name, const std::string& action_name, int action_type) {
    TGroundingActionHistoryItem gahi;
    gahi.iGroundingActionType = action_type;
    gahi.sActionName = action_name;
    gahi.sGroundingModelName = model_name;
    gahi.bBargeIn = false;

    int turn_num = CDMCoreAgent::Instance()->GetLastInputTurnNumber();
    _grounding_actions_history.resize(turn_num + 1);
    _grounding_actions_history[turn_num].push_back(gahi);
}

void CGroundingManagerAgent::GAHSetBargeInFlag(int turn_num, bool flag) {
}

std::string CGroundingManagerAgent::GAHGetTurnGroundingAction(int turn_num) {
    std::string action_name = "NO_ACTION";
    if ( turn_num >= 1 && turn_num < _grounding_actions_history.size() ) {
        std::vector<TGroundingActionHistoryItem> gahis = _grounding_actions_history[turn_num];
        for ( size_t i = 0;i < gahis.size(); ++i ) {
            if ( gahis[i].iGroundingActionType == GAT_TURN ) {
                action_name = gahis[i].sActionName;
                break;
            }
        }
    }
    return action_name;
}

int CGroundingManagerAgent::GAHCountTakenInLastNTurns(bool is_also_heard, const std::string& action_name, int num_turns) {
    return 0;
}

int CGroundingManagerAgent::GAHCountTakenByGroundingModelInLastNTurns(bool is_also_head, const std::string& action_name, const std::string& grounding_model_name, int num_turns) {
    return 0;
}

void CGroundingManagerAgent::RegisterGroundingModelType(const std::string& name, FCreateGroundingModel funct) {
}

CGroundingModel* CGroundingManagerAgent::CreateGroundingModel(const std::string& model_type, const std::string& model_policy) {
    return NULL;
}

bool CGroundingManagerAgent::HasPendingRequests() {
    return HasPendingTurnGroundingRequest() || HasPendingConceptGroundingRequests();
}

bool CGroundingManagerAgent::HasPendingTurnGroundingRequest() {
    return _is_turn_grounding_request;
}

bool CGroundingManagerAgent::HasPendingConceptGroundingRequests() {
    return false;
}

bool CGroundingManagerAgent::HasUnprocessedConceptGroundingRequests() {
    return false;
}

bool CGroundingManagerAgent::HasScheduledConceptGroundingRequests() {
    return false;
}

bool CGroundingManagerAgent::HasExecutingConceptGroundingRequests() {
    return false;
}

bool CGroundingManagerAgent::GroundingInProgressOnConcept(CConcept* concept) {
    return (GetConceptGroundingRequestIndex(concept) != -1);
}

std::string CGroundingManagerAgent::GetScheduledGroundingActionOnConcept(CConcept* concept) {
    return "";
}

void CGroundingManagerAgent::Run() {
}

int CGroundingManagerAgent::GetConceptGroundingRequestIndex(CConcept* concept) {
    return -1;
}

std::string CGroundingManagerAgent::LoadPolicy(const std::string& file_name) {
    return "";
}

}
