#include "DialogAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Registry.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

#define AGENT_MAX_EXECUTE_NUM 10

CDialogAgent CDialogAgent::NULLDialogAgent("NULL");

CDialogAgent::CDialogAgent(const std::string& name, const std::string& configuration, const std::string& type) :
    CAgent(name, configuration, type) {
        _dialog_agent_name = name;
        _da_parent = NULL;
        _da_context_agent = NULL;
        _grounding_model = NULL;
        _is_completed = false;
        _is_updated = false;
        _completion_type = ctFailed;
        _is_blocked = false;
        _is_dynamic_agent = false;
        _dynamic_agent_id = "";
        _triggered_by_commands = "";
        _trigger_commands_grounding_model_spec = "";
        _execute_counter = 0;
        _reset_counter = 0;
        _re_open_counter = 0;
        _turns_in_focus_counter = 0;
        _last_input_index = -1;
        _last_bindings_index = -1;
        _last_excution_index = -1;
        _last_persistence_count = 0;
        _last_execution_input_index = -1;
        _is_inherited_parent_input_configuration = false;
        _is_initial_status = true;
        _is_create_sub_agent = false;
        _is_create_skip_task = false;
}

CDialogAgent::~CDialogAgent() {
    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        delete _concepts[i];
    }

    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        delete _sub_agents[i];
    }

    if ( _grounding_model != NULL ) {
        delete _grounding_model;
        _grounding_model = NULL;
    }

    _da_parent = NULL;
}

CAgent* CDialogAgent::AgentFactory(const std::string& name, const std::string& configuration) {
    return NULL;
}

void CDialogAgent::Register() {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    if ( _owner_core_agent != NULL && _owner_core_agent->GetRegistryAgent() != NULL ) {
        _owner_core_agent->GetRegistryAgent()->RegisterAgent(_name, this);
        RegisterSubAgent();
        CreateTriggerConcept();
        UpdateConfig();
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", Fail, _owner_core_agent != NULL: " << (_owner_core_agent != NULL);
    }
}

void CDialogAgent::RegisterSubAgent() {
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->Register();
    }
}

void CDialogAgent::Create() {
    CreateConcepts();
    OnCreation();
}

void CDialogAgent::CreateSubAgents() {
}

void CDialogAgent::Initialize() {
    OnInitialization();
}

void CDialogAgent::Reset() {
    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        _concepts[i]->Clear();
    }
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->Reset();
    }
    _is_completed = false;
    _completion_type = ctFailed;
    _is_blocked = false;
    _execute_counter = 0;
    _re_open_counter = 0;
    _reset_counter += 1;
    _turns_in_focus_counter = 0;
    _last_input_index = -1;
    _last_excution_index = -1;
    _last_bindings_index = -1;
    _is_initial_status = true;
    _is_updated = true;

    OnInitialization();
}

void CDialogAgent::ClearTriggerConcept() {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    for (std::map<std::string, TTriggerInfo>::const_iterator c_iter = _trigger_concepts.begin();
            c_iter != _trigger_concepts.end(); ++c_iter ) {
        if ( _need_concepts.find( c_iter->first) == _need_concepts.end() ) {
            C(c_iter->first).Clear();
        }
    }
    
    // 触发业务流时，全局不需要的变量清除，  1213 有问题回滚
    if ( _owner_core_agent != NULL ) {
        CDialogAgent* root = _owner_core_agent->GetTaskRoot();

        if ( root != NULL ) {
            if ( !IsDTSAgent() ) {
                CDialogAgent* da_focused = _owner_core_agent->GetCurrentBusinessAgent();
                if ( da_focused != NULL ) {
                    std::set<std::string> must_concepts = da_focused->GetMustConcepts();
                    must_concepts.insert( _need_concepts.begin(), _need_concepts.end() );
                    root->ClearCurrentConcepts( csUser, must_concepts, 0);
                    return ;
                }
            }
            root->ClearCurrentConcepts( csUser, _need_concepts, 0);
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", Root is NULL, AgentName: " << _name;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", OwnerCoerAgetn is NULL, AgentName: " << _name;
    }
}

void CDialogAgent::ClearConcepts( TConceptSource source, const std::set<std::string>& need_retain_concepts, int l_round, int r_round ) {
    ClearCurrentConcepts(source, need_retain_concepts, l_round, r_round);
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        if ( _sub_agents[i] != NULL ) {
            _sub_agents[i]->ClearConcepts(source, need_retain_concepts, l_round, r_round);
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", SubAgent is NULL, AgentName: " << _name;
        }
    }
}

void CDialogAgent::ClearCurrentConcepts( TConceptSource source, const std::set<std::string>& need_retain_concepts, int l_round, int r_round ) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name << ", L_Round: " << l_round << "R_Round: " << r_round << ", Source: " << source;
    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        if ( _concepts[i] != NULL ) {
            if ( ( source == csUnKnow || _concepts[i]->GetConceptSource() == source) &&
                    _concepts[i]->IsUpdated() && 
                    ( _concepts[i]->GetTurnsSinceLastUpdated() > l_round && 
                      _concepts[i]->GetTurnsSinceLastUpdated() < r_round) ) {
                if ( need_retain_concepts.find(_concepts[i]->GetName()) == need_retain_concepts.end() ) {
                    VLOG(DEBUG) << __FUNCTION__ << ", Clear Concept: " << _concepts[i]->GetName();
                    _concepts[i]->Clear();
                } else {
                    VLOG(DATA) << __FUNCTION__ << ", Can Not Clear, Concept: " << _concepts[i]->GetName();
                }
            } else {
                VLOG(DEBUG) << __FUNCTION__ << ", ConceptName: " << _concepts[i]->GetName()
                    << ", TurnsSinceLastUpdated: " << _concepts[i]->GetTurnsSinceLastUpdated()
                    << ", Source: " << _concepts[i]->GetConceptSource() << ", IsUpdated: " << _concepts[i]->IsUpdated();
            }
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", concept is NULL, AgentName: " << _name;
        }
    }
}

void CDialogAgent::RecoverTriggerConcept() {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    for (std::map<std::string, TTriggerInfo>::const_iterator c_iter = _trigger_concepts.begin();
            c_iter != _trigger_concepts.end(); ++c_iter ) {

        if ( !(C(c_iter->first).IsUpdated()) ) {
            const TTriggerInfo& trigger_info = c_iter->second;
            if ( trigger_info.sOperator == "=" ) {
                C(c_iter->first) = trigger_info.sValue;
            }
        } else {
            VLOG(DEBUG) << __FUNCTION__ << ", ConceptName: " << c_iter->first;
        }
    }
}

void CDialogAgent::SetOwnerCoreAgent(CDMCoreAgent* core_agent) {
    if ( core_agent == NULL ) {
        VLOG(DEBUG) << __FUNCTION__ << "NULL_NULL..";
    }
    _owner_core_agent = core_agent;

    SetSubAgentOwnerCoreAgent( core_agent );

}

void CDialogAgent::SetSubAgentOwnerCoreAgent(CDMCoreAgent* core_agent) {
    for (size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->SetOwnerCoreAgent(core_agent);
    }
}

CDMCoreAgent* CDialogAgent::GetOwnerCoreAgent() {
    return _owner_core_agent;
}

void CDialogAgent::IncomingStack() {
}

bool CDialogAgent::IsStartAgain() {
    if ( _completion_type == ctStartAgain ) {
        return true;
    }
    for ( size_t i = 0;i < _sub_agents.size(); ++i) {
        if ( _sub_agents[i]->IsStartAgain() ) {
            return true;
        }
    }
    return false;
}

void CDialogAgent::RollbackLastUpdateConcept(bool is_recursion) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name << ", is_recursion: " << is_recursion;
    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        if ( _concepts[i] != NULL ) {
            if ( _concepts[i]->GetTurnsSinceLastUpdated() <= 1 ) {
                // 之后考虑恢复到最新修改前的值
                VLOG(DEBUG) << __FUNCTION__ << ", Clear Concept: " << _concepts[i]->GetName();
                _concepts[i]->Clear();
            }
        }
    }
    if ( is_recursion ) {
        for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
            if ( _sub_agents[i] != NULL ) {
                _sub_agents[i]->RollbackLastUpdateConcept( is_recursion );
            }
        }
    }
}

bool CDialogAgent::Load(const std::map<std::string, std::string>& agents, const std::map<std::string, std::string>& concepts) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;

    _is_updated = false;
    std::map<std::string, std::string>::const_iterator c_iter = agents.find(_name);
    if ( c_iter != agents.end() ) {
        if ( !FromJsonString(c_iter->second) ) {
            VLOG(WARNING) << __FUNCTION__ << ", FromJsonString Fail, AgentName: " << _name;
            return false;
        }
    } else {
        VLOG(DATA) << __FUNCTION__ << ", Not Agent, AgentName: " << _name;
        return true;
    }
    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        std::string concept_name = _concepts[i]->GetAgentQualifiedName();
        std::map<std::string, std::string>::const_iterator c_iter = concepts.find(concept_name);
        if ( c_iter != concepts.end() ) {
            VLOG(DEBUG) << "ConceptName: " << concept_name << ", Strg: " << c_iter->second; 
            _concepts[i]->Update_NPU_AssignFromString(&(c_iter->second));
        }
    }
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        if ( !_sub_agents[i]->Load(agents, concepts) ) {
            VLOG(WARNING) << __FUNCTION__ << ", SubAgent Load Fail, AgentName: " << _name << ", SubAgentName: " << _sub_agents[i]->GetName();
            return false;
        }
    }
    return true;
}

bool CDialogAgent::DeclareSaveData(std::map<std::string, std::string>& agents, std::map<std::string, std::string>& concepts) {
    // 初始化的状态
    // Clear Redis
    
    // if ( _is_initial_status && !_is_create_sub_agent && !_is_create_skip_task ) {
    if ( _is_initial_status && !IsNeedSaveForTaskSkip() ) {
        VLOG(DEBUG) << __FUNCTION__ << ", Not Need Save To Redis, AgentName: " << _name;
        return true;
    }

    VLOG(DATA) << __FUNCTION__ << ", Initial: " << _is_initial_status << ", IsNeedSaveForTaskSkip: " << IsNeedSaveForTaskSkip() << ", AgentName: " << _name;

    agents[_name] = ToJsonString();
    
    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        std::string concept_name = _concepts[i]->GetAgentQualifiedName();
        if ( !_concepts[i]->IsUpdated() ) {
            concepts.erase(concept_name);
        } else {
            concepts[concept_name] = _concepts[i]->HypSetToString();
        }
    }

    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->DeclareSaveData(agents, concepts);
    }
    
    return true;
}

std::string CDialogAgent::ToJsonString() {
    Json::Value j_data;
    Json::FastWriter fast_writer;

    JSON_ADD_ITEM(j_data, _is_create_sub_agent);
    JSON_ADD_ITEM(j_data, _is_create_skip_task);
    JSON_ADD_ITEM(j_data, _is_initial_status);
    if ( !_is_initial_status ) {
        JSON_ADD_ITEM(j_data, _last_persistence_count);
        JSON_ADD_ITEM(j_data, _is_completed);
        JSON_ADD_ITEM(j_data, _completion_type);
        JSON_ADD_ITEM(j_data, _is_blocked);
        JSON_ADD_ITEM(j_data, _is_dynamic_agent);
        JSON_ADD_ITEM(j_data, _dynamic_agent_id);
        JSON_ADD_ITEM(j_data, _execute_counter);
        JSON_ADD_ITEM(j_data, _reset_counter);
        JSON_ADD_ITEM(j_data, _re_open_counter);
        JSON_ADD_ITEM(j_data, _turns_in_focus_counter);
        JSON_ADD_ITEM(j_data, _last_input_index);
        JSON_ADD_ITEM(j_data, _last_execution_input_index);
        JSON_ADD_ITEM(j_data, _last_excution_index);
        JSON_ADD_ITEM(j_data, _last_bindings_index);
    }

    return fast_writer.write(j_data);
}

bool CDialogAgent::FromJsonString(const std::string& strg) {
    Json::Reader reader;
    Json::Value j_data;

    if ( !reader.parse(strg, j_data) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Parse Json Fail, AgentName: " << _name;
        return false;
    }
    JSON_ITEM_TO_BOOL(j_data, _is_initial_status);
    JSON_ITEM_TO_BOOL(j_data, _is_create_sub_agent);
    JSON_ITEM_TO_BOOL(j_data, _is_create_skip_task);

    if ( !_is_initial_status ) {
        JSON_ITEM_TO_INT(j_data, _last_persistence_count);
        JSON_ITEM_TO_BOOL(j_data, _is_completed);
        JSON_ITEM_TO_ENUM(j_data, _completion_type, TCompletionType);
        JSON_ITEM_TO_BOOL(j_data, _is_blocked);
        JSON_ITEM_TO_BOOL(j_data, _is_dynamic_agent);
        JSON_ITEM_TO_STRING(j_data, _dynamic_agent_id);
        JSON_ITEM_TO_INT(j_data, _execute_counter);
        JSON_ITEM_TO_INT(j_data, _reset_counter);
        JSON_ITEM_TO_INT(j_data, _re_open_counter);
        JSON_ITEM_TO_INT(j_data, _turns_in_focus_counter);
        JSON_ITEM_TO_INT(j_data, _last_input_index);
        JSON_ITEM_TO_INT(j_data, _last_execution_input_index);
        JSON_ITEM_TO_INT(j_data, _last_excution_index);
        JSON_ITEM_TO_INT(j_data, _last_bindings_index);
    }

    if ( _is_create_sub_agent && _sub_agents.empty() ) {
        DO_CREATE_SUB_AGENTS;
    }

    return true;
}

void CDialogAgent::NotifyChange() {
    _is_initial_status = false;
    _is_updated = true;
}

bool CDialogAgent::IsNeedSaveForTaskSkip() {
    return false;
}

int CDialogAgent::Outaction() {
    if ( Config::Instance() != NULL ) {
        return Config::Instance()->GetIntValue("session/field_definition/outaction_interactive");
    } else {
        VLOG(FATAL) << __FUNCTION__ << ", Config::Instance() is NULL.";
        return 0;
    }
}

int CDialogAgent::ModelTypeOperator() {
    return 0;
}
 
int CDialogAgent::ModelTypeDTMFLength() {
    return 0;
}
 
int CDialogAgent::ModelTypeEnd() {
    return 0;
}

TEOutputType CDialogAgent::OutputType() {
    return O_UNDEFINE;
}

bool CDialogAgent::IsHelpful() {
    return false;
}

void CDialogAgent::SetInputConfiguration(const std::string& configuration ) {
    std::string item, slot, value;
    std::vector<std::string> slot_values;
    SplitString(configuration, ',', &slot_values);

    for (size_t i = 0;i < slot_values.size(); ++i ) {
        SplitOnFirst(slot_values[i], "=", slot, value, '%');
        slot = TrioTDM::Trim(slot);
        value = TrioTDM::Trim(value);

        SetInputConfigurationParameter(slot, value);
    }
}

void CDialogAgent::SetInputConfigurationParameter(const std::string& slot, const std::string& value) {
    _input_line_configuration.insert(std::make_pair(slot, value));
}

bool CDialogAgent::HasInputConfigurationParameter(const std::string& slot) {
    return (_input_line_configuration.find(slot) != _input_line_configuration.end());
}

std::string CDialogAgent::GetInputConfigurationParameterValue(const std::string& slot) {
    std::map<std::string, std::string>::iterator f_iter = _input_line_configuration.find(slot);
    if ( f_iter != _input_line_configuration.end() ) {
        return f_iter->second;
    } else {
        return "";
    }
}

std::string CDialogAgent::InputLineConfigurationInitString() {
    return "";
}

std::map<std::string, std::string> CDialogAgent::GetInputLineConfiguration() {
    if ( !_is_inherited_parent_input_configuration ) {
        _is_inherited_parent_input_configuration = true;
        SetInputConfiguration(InputLineConfigurationInitString());
        if ( _da_parent ) {
            InheritParentInputConfiguration();
        }
    }
    return _input_line_configuration;
}

void CDialogAgent::InheritParentInputConfiguration() {
    std::map<std::string, std::string> parent_config = _da_parent->GetInputLineConfiguration();
    for (std::map<std::string, std::string>::iterator iter = parent_config.begin(); iter != parent_config.end(); ++iter) {
        if ( !HasInputConfigurationParameter(iter->first) ) {
            SetInputConfigurationParameter(iter->first, iter->second);
        }
    }
}

void CDialogAgent::CreateConcepts() {
}

bool CDialogAgent::IsExecutable() {
    return true;
}

TDialogExecuteReturnCode CDialogAgent::Execute(COutput* output) {
    IncrementExecuteCounter();

    return dercContinueExecution;
}

CDialogAgent* CDialogAgent::GetOwnerMainTopic() {
    if ( IsAMainTopic() ) {
        return this;
    } else if ( _da_parent != NULL ) {
        return _da_parent->GetOwnerMainTopic();
    } else {
        return this;
    }
}

std::map<std::string, TTriggerInfo>& CDialogAgent::GetTriggerConcepts() {
    return _trigger_concepts;
}

std::string CDialogAgent::GetErrorHandle() {
    return "common";
}

void CDialogAgent::OnConceptUpdated() {
    VLOG(DEBUG) << __FUNCTION__ << ", DoNothing AgentName: " << _name;
}


bool CDialogAgent::IsNeedDeclareExpectations() {
    return false;
}

int CDialogAgent::DeclareExpectations(std::vector<TConceptExpectation>& expectations, bool is_must_not_dts_agent) {
    // VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    int expectations_added = 0;
    bool expect_condition = ExpectCondition();

    if ( TriggeredByCommands() != "" ) {
        std::vector<TConceptExpectation> cel_trigger_expectations;
        ParseGrammarMapping(
                C("_%s_trigger", _dialog_agent_name.c_str()).GetAgentQualifiedName(),
                TriggeredByCommands(),
                cel_trigger_expectations);
        for ( size_t i = 0;i < cel_trigger_expectations.size(); ++i ) {
            cel_trigger_expectations[i].bmBindMethod = bmExplicitValue;
            cel_trigger_expectations[i].sExplicitValue = "true";

            if ( !expect_condition ) {
                cel_trigger_expectations[i].bDisabled = true;
                cel_trigger_expectations[i].sReasonDisabled = "expect condition false";
            }
            expectations.push_back(cel_trigger_expectations[i]);
            expectations_added += 1;
        }
    }
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        if ( is_must_not_dts_agent && _sub_agents[i]->IsDTSAgent() ) {
            continue;
        }
        expectations_added += _sub_agents[i]->DeclareExpectations(expectations, is_must_not_dts_agent);
    }
    return expectations_added;
}

void CDialogAgent::DeclareConcepts(std::vector<CConcept*>& concepts,
        std::set<CConcept*>& exclude) {
    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        _concepts[i]->DeclareConcepts(concepts, exclude);
    }
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->DeclareConcepts(concepts, exclude);
    }
}

void CDialogAgent::CreateGroundingModel(const std::string& grounding_model_spec) {
    if ( CGroundingManagerAgent::Instance() != NULL ) {
        if ( !(CGroundingManagerAgent::Instance()->GetConfiguration().bGroundTurns) ||
                (grounding_model_spec == "none") ||
                (grounding_model_spec == "") ) {
            _grounding_model = NULL;
        } else {
            std::string model_type, model_policy;
            if ( !SplitOnFirst(grounding_model_spec, ".", model_type, model_policy) ) {
                model_type = CGroundingManagerAgent::Instance()->GetConfiguration().sTurnGM;
                model_policy = grounding_model_spec;
            }
            _grounding_model = CGroundingManagerAgent::Instance()->CreateGroundingModel(model_type, model_policy);
            _grounding_model->Initialize();
            ((CGMRequestAgent*)_grounding_model)->SetRequestAgent(this);
        }
    }
}

CGroundingModel* CDialogAgent::GetGroundingModel() {
    return _grounding_model;
}

void CDialogAgent::DeclareGroundingModels(
        std::vector<CGroundingModel*>& models,
        std::set<CGroundingModel*>& exclude) {
    if ( _grounding_model && exclude.find(_grounding_model) == exclude.end() ) {
        models.push_back(_grounding_model);
        exclude.insert(_grounding_model);
    }
    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        _concepts[i]->DeclareGroundingModels(models, exclude);
    }
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->DeclareGroundingModels(models, exclude);
    }
}

bool CDialogAgent::ExpectCondition() {
    return true;
}

std::string CDialogAgent::DeclareBindingPolicy() {
    return MIXED_INITIATIVE;
}

int CDialogAgent::DeclareFocusClaims(std::vector<TFocusClaim>& focus_claims) {
    int claims_added = 0;
    bool is_declare_focus_claim = ClaimsFocus();
    if ( !TriggeredByCommands().empty() ) {
        is_declare_focus_claim = is_declare_focus_claim || C(FormatString("_%s_trigger", _dialog_agent_name.c_str())).IsUpdatedAndGrounded();
    }

    if ( is_declare_focus_claim ) {
        TFocusClaim claim;
        claim.sAgentName = _name;
        claim.bClaimDuringGrounding = ClaimsFocusDuringGrounding();
        focus_claims.push_back(claim);
        claims_added += 1;
        if ( !TriggeredByCommands().empty() ) {
            C(FormatString("_%s_trigger", _dialog_agent_name.c_str())).Clear();
        }
        OnFocusclaims();
    }
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        claims_added += _sub_agents[i]->DeclareFocusClaims(focus_claims);
    }
    return claims_added;
}

void CDialogAgent::OnFocusclaims() {
}

bool CDialogAgent::PreconditionsSatisfied() {
    return true;
}

bool CDialogAgent::ClaimsFocus() {
    if ( _trigger_concepts.empty() ) {
        return false;
    } else {
        VLOG(DEBUG)  << __FUNCTION__ << "TriggerConcepts.size: " << _trigger_concepts.size() << ", AgentName: " << _name;
        for ( std::map<std::string, TTriggerInfo>::const_iterator c_iter = _trigger_concepts.begin();
                c_iter != _trigger_concepts.end(); ++c_iter ) {
            const TTriggerInfo& trigger_info = c_iter->second;
            if ( trigger_info.sOperator == "=" ) {
                if ( (std::string)C(c_iter->first) != trigger_info.sValue ) {
                    return false;
                }
            } else if ( trigger_info.sOperator == "!=" ) {
                if ( (std::string)C(c_iter->first) == trigger_info.sValue ) {
                    return false;
                }
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", Can not Deal, Operator: " << trigger_info.sOperator;
                return false;
            }
        }
        ClearTriggerConcept();
        return true;
    }
}

bool CDialogAgent::ClaimsFocusDuringGrounding() {
    return false;
}

std::string CDialogAgent::TriggeredByCommands() {
    return "";
}

void CDialogAgent::CreateTriggerConcept() {
    if ( !TriggeredByCommands().empty() ) {
        _concepts.push_back( new CBoolConcept(FormatString("_%s_trigger", _dialog_agent_name.c_str()), csUser));
        _concepts.back()->CreateGroundingModel( _trigger_commands_grounding_model_spec );
        _concepts.back()->SetOwnerDialogAgent(this);
    }
}

bool CDialogAgent::SuccessCriteriaSatisfied() {
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        if ( !_sub_agents[i]->HasCompleted() ) {
            return false;
        }
    }
    return true;
}

bool CDialogAgent::FailureCriteriaSatisfied() {
    bool is_failed = (_execute_counter >= GetMaxExecuteCounter()) && !SuccessCriteriaSatisfied();

    return is_failed;
}

int CDialogAgent::GetMaxExecuteCounter() {
    if ( Config::Instance() != NULL ) {
        int n = Config::Instance()->GetIntValue("task_dman/agent_max_execute_num_limit");
        if ( n > 0 ) {
            return n;
        }
    }
    return AGENT_MAX_EXECUTE_NUM;
}

void CDialogAgent::OnCreation() {
}

void CDialogAgent::OnDestruction() {
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->OnDestruction();
    }
}

void CDialogAgent::OnInitialization() {
}

void CDialogAgent::OnCompletion() {
}

void CDialogAgent::OnExecuteFail() {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    if ( _owner_core_agent == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", _owner_core_agent is NULL, AgentName: " << _name;
        return ;
    }
    if ( CDTTManagerAgent::Instance() == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", CDTTManagerAgent::Instance() is NULL, AgentName: " << _name;
        return ;
    }
    CDialogAgent* system_expect = _owner_core_agent->GetSystemExpectDialog();
    if ( system_expect != NULL ) {
        _owner_core_agent->ResetExecutionStack();
        _owner_core_agent->ContinueWith(_owner_core_agent, system_expect);
    } else {
        SetCompleted( ctFailed );
        VLOG(FATAL) << __FUNCTION__ << ", Can not get system expect deal agent, AgentName: " << _name;
    }
}

void CDialogAgent::ReOpen() {
    ReOpenConcepts();
    ReOpenTopic();
}

void CDialogAgent::ReOpenConcepts() {
    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        _concepts[i]->ReOpen();
    }
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->ReOpenConcepts();
    }
}

void CDialogAgent::ReOpenTopic() {
    _is_completed = false;
    _completion_type = ctFailed;
    _is_blocked = false;
    _execute_counter = 0;
    _re_open_counter += 1;
    _turns_in_focus_counter = 0;
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->ReOpenTopic();
    }
    OnInitialization();
    NotifyChange();
}

bool CDialogAgent::IsAMainTopic() {
    return false;
}

bool CDialogAgent::IsABusiness() {
    return false;
}

std::string CDialogAgent::Prompt() {
    CDialogAgent* agent = GetMainTopic();
    std::string prompt = "template ";
    if ( agent != NULL && agent != this ) {
        prompt += agent->GetDialogAgentName() + ".";
    }
    prompt += GetDialogAgentName();
    return prompt;
}

std::string CDialogAgent::TimeoutPrompt() {
    std::string prompt = TrioTDM::Trim(Prompt());
    if ( prompt == "" ) {
        return "";
    }
    std::string timeout_prompt;
    if ( prompt[0] == '{' ) {
        do {
            int closing_pos = FindClosingQuoteChar(prompt, 1, '{', '}');
            timeout_prompt += prompt.substr(0, closing_pos - 1);
            timeout_prompt += " version=timeout}";
            if ( closing_pos >= (int)prompt.length() ) { break; }
            prompt = prompt.substr(closing_pos, prompt.length() - closing_pos);
        } while ( prompt != "" );
    } else {
        timeout_prompt += prompt + " version=timeout";
    }
    return timeout_prompt;
}

std::string CDialogAgent::ExplainMorePrompt() {
    std::string prompt = TrioTDM::Trim(Prompt());
    if ( prompt == "" ) {
        return "";
    }
    std::string explain_more_prompt;
    if ( prompt[0] == '{') {
        do {
            int closing_pos = FindClosingQuoteChar(prompt, 1, '{', '}');
            explain_more_prompt += prompt.substr(0, closing_pos - 1);
            explain_more_prompt += " version=explain_more}";
            if ( closing_pos >= (int)prompt.length()) { break; }
            prompt = prompt.substr(closing_pos, prompt.length() - closing_pos);
        } while( prompt != "");
    } else {
        explain_more_prompt = prompt + " version=explain_more";
    }
    return explain_more_prompt;
}

std::string CDialogAgent::CreateVersionedPrompt(const std::string& version) {
    VLOG(DEBUG) << "CreateVersionedPrompt, Agent: " << GetName() << "\tversion: " << version;
    std::string versioned_prompt = "template " + _dialog_agent_name + "." + version;
    return versioned_prompt;
}

std::string CDialogAgent::EstablishContextPrompt() {
    if ( _da_context_agent && (_da_context_agent != this) ) {
        return _da_context_agent->EstablishContextPrompt();
    } else if ( _da_parent ) {
        return _da_parent->EstablishContextPrompt();
    } else {
        return "";
    }
}

std::string CDialogAgent::WhatCanISayPrompt() {
    std::string prompt = TrioTDM::Trim(Prompt());
    if ( prompt == "" ) {
        return "";
    }
    std::string what_can_i_say_prompt;
    if ( prompt[0] == '{' )   {
        do {
            int closing_pos = FindClosingQuoteChar(prompt, 1, '{', '}');
            what_can_i_say_prompt += prompt.substr(0, closing_pos - 1);
            what_can_i_say_prompt += " version=what_can_i_say}";
            if ( closing_pos >= (int)prompt.length() ) { break; }
            prompt = prompt.substr(closing_pos, prompt.length() - closing_pos);
        } while ( prompt != "");
    } else {
        what_can_i_say_prompt = prompt + " version=what_can_i_say";
    }
    return what_can_i_say_prompt;
}

bool CDialogAgent::IsDTSAgent() {
    return true;
}

bool CDialogAgent::IsConversationSynchronous() {
    return false;
}

bool CDialogAgent::RequiresFloor() {
    return false;
}

void CDialogAgent::Undo() {
    _execute_counter -= 1;
    ResetCompleted();
    NotifyChange();
}

CConcept& CDialogAgent::C(const std::string& concept_path) {
    // VLOG(DEBUG) << "GetConcept, AgentName: " << _name << "\tconcept_path: " << concept_path;
    std::string agent_path, concept_name;
    if ( SplitOnLast(concept_path, "/", agent_path, concept_name)) {
        return A(agent_path).LocalC(concept_name);
    } else {
        return LocalC(concept_path);
    }
}

CConcept& CDialogAgent::C(const char* concept_path, ...) {
    char buffer[STRING_MAX];
    va_list args;
    va_start(args, concept_path);
    vsnprintf(buffer, STRING_MAX, concept_path, args);

    return C((string)buffer);
}

CConcept& CDialogAgent::LocalC(const std::string& concept_name) {
    CDialogAgent* da_next_context = NULL;
    if ( _da_context_agent ) {
        da_next_context = _da_context_agent;
    } else {
        da_next_context = _da_parent;
    }

    std::string t_concept_name = ReplaceSubString(concept_name, "#", GetDynamicAgentID());

    if ( _concepts.size() == 0 ) {
        if ( da_next_context ) {
            return da_next_context->LocalC(t_concept_name);
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", da_next_context is NULL, AgentName: " << _name;
            return CConcept::NULLConcept;
        }
    }
    std::string base_concept_name, rest;
    SplitOnFirst(t_concept_name, ".", base_concept_name, rest);
    bool is_merge_concept = false;
    if ( base_concept_name.at(0) == '@') {
        is_merge_concept = true;
        base_concept_name = base_concept_name.substr(1, base_concept_name.length());
    }

    for ( size_t i = 0;i < _concepts.size(); ++i ) {
        if ( _concepts[i]->GetName() == base_concept_name ) {
            if ( is_merge_concept ) {
                CConcept* c_merged = _concepts[i]->operator[](rest).CreateMergedHistoryConcept();
                return *c_merged;
            } else {
                return _concepts[i]->operator[](rest);
            }
        }
    }

    if ( !da_next_context ) {
        VLOG(WARNING) << __FUNCTION__ << ", da_next_context is NULL, AgentName: " << _name << ", ConceptName: " << concept_name;
        return CConcept::NULLConcept;
    } else {
        return da_next_context->LocalC(t_concept_name);
    }
}

CDialogAgent& CDialogAgent::A(const std::string& dialog_agent_path) {
    std::string first_component, rest;
    if ( _owner_core_agent == NULL || _owner_core_agent->GetRegistryAgent() == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", Fail, _owner_core_agent == NULL: " << (_owner_core_agent == NULL);
        return NULLDialogAgent;
    }
    if ( SplitOnFirst(dialog_agent_path, "/", first_component, rest)) {
        if ( first_component == "" ) {
            CDialogAgent* da_agent = (CDialogAgent*)_owner_core_agent->GetRegistryAgent()->operator[](dialog_agent_path);
            if ( da_agent ) {
                return *da_agent;
            }
            return NULLDialogAgent;
        } else if ( first_component == ".." ) {
            if ( !_da_parent ) {
                return NULLDialogAgent;
            } else {
                return _da_parent->A(rest);
            }
        } else if ( first_component == ".") {
            return A(rest);
        } else {
            CDialogAgent* da_agent = (CDialogAgent*)_owner_core_agent->GetRegistryAgent()->operator[](GetName()+"/"+dialog_agent_path);
            if ( da_agent ) {
                return *da_agent;
            }
            return NULLDialogAgent;
        }
    } else {
        if ( dialog_agent_path == "" || dialog_agent_path == "." ) {
            return *this;
        }
        if ( dialog_agent_path == ".." ) {
            if ( !_da_parent ) {
                return NULLDialogAgent;
            } else {
                return *_da_parent;
            }
        }
        CDialogAgent* da_agent = (CDialogAgent*)_owner_core_agent->GetRegistryAgent()->operator[](GetName()+"/"+dialog_agent_path);
        if ( da_agent ) {
            return *da_agent;
        }
        return NULLDialogAgent;
    }
}

CDialogAgent& CDialogAgent::A(const char* dialog_agent_path, ...) {
    char buffer[STRING_MAX];
    va_list args;
    va_start(args, dialog_agent_path);
    vsnprintf(buffer, STRING_MAX, dialog_agent_path, args);
    return A((string)buffer);
}

void CDialogAgent::AddSubAgent(CDialogAgent* da_who, CDialogAgent* da_where, TAddSubAgentMethod method) {
    std::vector<CDialogAgent*>::iterator iter = _sub_agents.begin();

    switch(method) {
        case asamAsFirstChild:
            _sub_agents.insert(_sub_agents.begin(), da_who);
            break;
        case asamAsLastChild:
            _sub_agents.insert(_sub_agents.end(), da_who);
            break;
        case asamAsRightSibling:
            while ( (iter != _sub_agents.end()) && ((*iter) != da_where)) { iter++; }
            iter++;
            _sub_agents.insert(iter, da_who);
            break;
        case asamAsLeftSibling:
            while ((iter != _sub_agents.end()) && ((*iter) != da_where)) { iter++; }
            _sub_agents.insert(iter, da_who);
            break;
    }
    da_who->SetParent(this);
    da_who->SetDynamicAgent();
    da_who->Initialize();
    da_who->SetOwnerCoreAgent( _owner_core_agent );
    da_who->Register();
}

void CDialogAgent::DeleteSubAgent(CDialogAgent* da_who) {
    for ( std::vector<CDialogAgent*>::iterator iter = _sub_agents.begin(); iter != _sub_agents.end(); ++iter ) {
        if ( (*iter) == da_who ) {
            _sub_agents.erase(iter);
            da_who->OnDestruction();
            delete da_who;
            return ;
        }
    }
}

void CDialogAgent::DeleteDynamicSubAgents() {
    for ( std::vector<CDialogAgent*>::iterator iter = _sub_agents.begin();iter != _sub_agents.end();) {
        if ( (*iter)->IsDynamicAgent() ) {
            CDialogAgent* da_who = (*iter);
            iter = _sub_agents.erase(iter);
            da_who->OnDestruction();
            delete da_who;
        } else {
            (*iter)->DeleteDynamicSubAgents();
            ++iter;
        }
    }
}

void CDialogAgent::SetParent(CDialogAgent* da_parent) {
    _da_parent = da_parent;
    UpdateName();
}

CDialogAgent* CDialogAgent::GetParent() {
    return _da_parent;
}

void CDialogAgent::UpdateName() {
    if ( _da_parent ) {
        _name = _da_parent->GetName() + "/" + _dialog_agent_name;
    } else {
        _name = "/" + _dialog_agent_name;
    }
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->UpdateName();
    }
}

void CDialogAgent::UpdateConfig() {
    // size_t pos = _name.find("ROOT");
    // if ( pos != std::string::npos ) {
        size_t pos = _name.find("/", 1);
        if ( pos != std::string::npos ) {
            
            if ( Config::Instance() != NULL ) {
                std::string config_key = "task_specification/ROOT" + _name.substr(pos) + "/TRIGGER_CONDITION";
                std::map<std::string, std::string> trigger_concepts;
                if ( Config::Instance()->get_key_value(config_key, trigger_concepts) ) {
                    for ( std::map<std::string, std::string>::iterator iter = trigger_concepts.begin();
                            iter != trigger_concepts.end(); ++iter ) {
                        const std::string& value = iter->second;
                        TTriggerInfo trigger_info;
                        if ( StartsWithASCII(value, "deny:", false) ) {
                            trigger_info.sValue = value.substr(5);
                            trigger_info.sOperator = "!=";
                        } else if ( StartsWithASCII(value, "equal:", false) ) {
                            trigger_info.sValue = value.substr(6);
                            trigger_info.sOperator = "=";
                        } else {
                            trigger_info.sValue = value;
                            trigger_info.sOperator = "=";
                        }
                        _trigger_concepts.insert( std::make_pair( iter->first, trigger_info) );
                    }
                } else {
                    VLOG(WARNING) << __FUNCTION__ << ", read fail, KEY: " << config_key;
                }
                // 需要保留的槽位在发生拒识和超时时会清除
                std::string need_concept_key = "task_specification/ROOT" + _name.substr(pos) + "/NEED_CONCEPTS";
                std::string need_concepts = Config::Instance()->get_value( need_concept_key );
                if ( !need_concepts.empty() ) {
                    VLOG(DEBUG) << __FUNCTION__ << ", NeedConcept: " << need_concepts << ", AgentName: " << _name;
                    std::vector<std::string> need_conceptv;
                    SplitString(need_concepts, ',', &need_conceptv);
                    _need_concepts.insert(need_conceptv.begin(), need_conceptv.end());
                }
                
                // 必须保留的槽位只有在离开改业务树时清除
                std::string must_concept_key = "task_specification/ROOT" + _name.substr(pos) + "/MUST_CONCEPTS";
                std::string must_concepts = Config::Instance()->get_value( must_concept_key );
                if ( !must_concepts.empty() ) {
                    VLOG(DEBUG) << __FUNCTION__ << ", MustConcept: " << must_concepts << ", AgentName: " << _name;
                    std::vector<std::string> must_conceptv;
                    SplitString(must_concepts, ',', &must_conceptv);
                    _must_concepts.insert(must_conceptv.begin(), must_conceptv.end());
                    _need_concepts.insert(must_conceptv.begin(), must_conceptv.end());
                }
            } else {
                VLOG(FATAL) << __FUNCTION__ << ", Config::Instance() is NULL.";
            }
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", Error, AgentName: " << _name;
        }
    //} else {
    //    VLOG(WARNING) << __FUNCTION__ << ", Error, AgentName: " << _name;
    //}
}

void CDialogAgent::SetContextAgent(CDialogAgent* da_context_agent) {
    _da_context_agent = da_context_agent;
}

CDialogAgent* CDialogAgent::GetContextAgent() {
    if ( _da_context_agent ) {
        return _da_context_agent;
    } else {
        return this;
    }
}

CDialogAgent* CDialogAgent::GetMainTopic() {
    if ( IsAMainTopic() ) { return this; }
    else if ( !_da_parent ) {
        return NULL;
    } else {
        return _da_parent->GetMainTopic();
    }
}

CDialogAgent* CDialogAgent::GetBusiness() {
    if ( IsABusiness() ) {
        return this;
    } else if ( !_da_parent ) {
        return NULL;
    } else {
        return _da_parent->GetBusiness();
    }
}

std::string CDialogAgent::GetDialogAgentName() {
    return _dialog_agent_name;
}

bool CDialogAgent::HasCompleted() {
    //VLOG(DEBUG) << __FUNCTION__ << ", _is_completed: " << _is_completed << ", HasSucceeded: " << HasSucceeded() << ", AgentName: " << _name;
    if ( _is_completed ) return true;
    return HasSucceeded() || HasFailed();
}

void CDialogAgent::SetCompleted(TCompletionType completion_type) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name << "\tCompletionType: " << completion_type;
    _is_completed = true;
    _completion_type = completion_type;
    NotifyChange();
}

void CDialogAgent::ResetCompleted() {
    _is_completed = false;
    _completion_type = ctFailed;
    NotifyChange();
}

bool CDialogAgent::HasFailed() {
    if ( _is_completed && (_completion_type == ctFailed) ) {
        return true;
    }
    return FailureCriteriaSatisfied();
}

bool CDialogAgent::HasSucceeded() {
    if ( _is_completed && (_completion_type == ctSuccess) ) {
        return true;
    }
    return SuccessCriteriaSatisfied();
}

bool CDialogAgent::IsAgentPathBlocked() {
    if ( _da_parent ) {
        return _da_parent->IsAgentPathBlocked() || IsBlocked();
    } else {
        return IsBlocked();
    }
}

bool CDialogAgent::IsBlocked() {
    return _is_blocked;
}

void CDialogAgent::Block() {
    _is_blocked = true;
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->Block();
    }
    NotifyChange();
}

void CDialogAgent::UnBlock() {
    _is_blocked = false;
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->UnBlock();
    }
    NotifyChange();
}

void CDialogAgent::SetDynamicAgent() {
    _is_dynamic_agent = true;
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->SetDynamicAgent();
    }
    NotifyChange();
}

void CDialogAgent::SetName(const std::string& name) {
    _name = name;
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->UpdateName();
    }
}

bool CDialogAgent::IsDynamicAgent() {
    return _is_dynamic_agent;
}

void CDialogAgent::SetDynamicAgentID(const std::string& dynamic_agent_id) {
    _is_dynamic_agent = true;
    _dynamic_agent_id = dynamic_agent_id;
    for ( size_t i = 0;i < _sub_agents.size(); ++i ) {
        _sub_agents[i]->SetDynamicAgentID(dynamic_agent_id);
    }
    NotifyChange();
}

std::string CDialogAgent::GetDynamicAgentID() {
    return _dynamic_agent_id;
}

void CDialogAgent::IncrementExecuteCounter() {
    _execute_counter += 1;
    NotifyChange();
}

int CDialogAgent::GetExecuteCounter() {
    return _execute_counter;
}

bool CDialogAgent::WasReset() {
    return _reset_counter > 0;
}

bool CDialogAgent::WasReOpened() {
    return _re_open_counter > 0;
}

void CDialogAgent::IncrementTurnsInFocusCounter() {
    _turns_in_focus_counter += 1;
    NotifyChange();
}

int CDialogAgent::GetTurnsInFocusCounter() {
    return _turns_in_focus_counter;
}

void CDialogAgent::SetLastInputIndex(int index) {
    _last_input_index = index;
    NotifyChange();
}

int CDialogAgent::GetLastInputInex() {
    return _last_input_index;
}

void CDialogAgent::SetLastExecutionIndex(int index) {
    _last_excution_index = index;
    NotifyChange();
}

int CDialogAgent::GetLastExecutionIndex() {
    return _last_excution_index;
}

void CDialogAgent::SetLastBindingsIndex(int index) {
    _last_bindings_index = index;
    NotifyChange();
}

int CDialogAgent::GetLastBindingsIndex() {
    return _last_bindings_index;
}

void CDialogAgent::ParseGrammarMapping(const std::string& concept_names, const std::string& grammar_mapping, std::vector<TConceptExpectation>& expectations) {
    VLOG(DEBUG) << __FUNCTION__ << ", ConceptName: " << concept_names << "\tGrammarMapping: " << grammar_mapping;
    expectations.clear();
    std::vector<std::string> items;
    SplitString(grammar_mapping, ',', &items);

    for ( size_t i = 0;i < items.size(); ++i ) {
        TConceptExpectation expectation;
        std::string left_side, right_side;
        if ( SplitOnFirst(items[i], ">", left_side, right_side)) {
            right_side = TrioTDM::Trim(right_side);
            if ( right_side[0] == ':') {
                expectation.bmBindMethod = bmBindingFilter;
            } else {
                expectation.bmBindMethod = bmExplicitValue;
            }
        } else {
            expectation.bmBindMethod = bmSlotValue;
        }

        left_side = TrioTDM::Trim(left_side);
        if ( CDTTManagerAgent::Instance() != NULL && _owner_core_agent != NULL ) {
            if ( left_side[0] == '[' ) {
                expectation.bDisabled = (_owner_core_agent->GetCurrentMainTopicAgent() != GetMainTopic());
                if ( expectation.bDisabled ) {
                    expectation.sReasonDisabled = "[] not under topic";
                }
                expectation.sGrammarExpectation = left_side;
                expectation.sExpectationType = "";
            } else if ( left_side[0] == '!' ) {
                expectation.bDisabled = !_owner_core_agent->AgentIsInFocus(this);
                if ( expectation.bDisabled ) {
                    expectation.sReasonDisabled = "![] not under focus";
                }
                expectation.sGrammarExpectation = left_side.substr(1, left_side.length()-1);
                expectation.sExpectationType = "!";
            } else if ( left_side[0] == '@' || left_side[0] == '*' ) {
                if ( left_side[1] == '[') {
                    expectation.bDisabled = false;
                    expectation.sGrammarExpectation = left_side.substr(1, left_side.length()-1);
                } else if ( left_side[1] == '(' ) {
                    std::string agents;
                    SplitOnFirst(left_side, ")", agents, expectation.sGrammarExpectation);
                    agents = agents.substr(2, agents.length()-2);
                    std::vector<std::string> agentv;
                    SplitString(agents, ';', &agentv);
                    CDialogAgent* da_dts_agent_in_focus = _owner_core_agent->GetDTSAgentInFocus();
                    if ( !da_dts_agent_in_focus ) {
                        VLOG(WARNING) << "Counld not find a DTS agent in focus.";
                    }
                    std::string focused_agent_name = da_dts_agent_in_focus->GetName();
                    expectation.bDisabled = true;
                    for ( size_t i = 0;i < agentv.size(); ++i ) {
                        if ( CDTTManagerAgent::Instance()->IsAncestorOrEqualOf(A(agentv[i]).GetName(), focused_agent_name)) {
                            expectation.bDisabled = false;
                            break;
                        }
                    }
                    if ( expectation.bDisabled ) {
                        expectation.sReasonDisabled = FormatString("%c(%s) not containing focus", left_side[0], agents.c_str());
                    }
                }
                expectation.sExpectationType = FormatString("%c", left_side[0]);
            }
        }

        if ( IsAgentPathBlocked() ) {
            expectation.bDisabled = true;
            expectation.sReasonDisabled = "agent path blocked";
        }

        if ( expectation.bmBindMethod == bmExplicitValue ) {
            expectation.sExplicitValue = right_side;
        } else if ( expectation.bmBindMethod == bmBindingFilter ) {
            expectation.sBindingFilterName = right_side.substr(1, right_side.length() - 1);
        }
        expectation.pDialogAgent = this;
        expectation.vsOtherConceptNames = PartitionString(concept_names, ";,");
        for ( size_t i = 0;i < expectation.vsOtherConceptNames.size(); ++i ) {
            expectation.vsOtherConceptNames[i] = C(TrioTDM::Trim(expectation.vsOtherConceptNames[i])).GetAgentQualifiedName();
        }
        expectation.sConceptName = expectation.vsOtherConceptNames[0];
        expectation.vsOtherConceptNames.erase(expectation.vsOtherConceptNames.begin());
        expectation.sGrammarExpectation = StringToLowerASCII(expectation.sGrammarExpectation);
        expectations.push_back(expectation);
    }
}

CConcept& CDialogAgent::NLGConceptParam() {
    return C("result");
}

CConcept& CDialogAgent::NLGConceptErrorParam() {
    return C("error_param");
}

CConcept& CDialogAgent::NLGConceptHelpParam() {
    return C("help_param");
}

void CDialogAgent::FillBusinessTriggerConcept() {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    CDialogAgent* business = GetBusiness();
    if ( business != NULL ) {
        business->RecoverTriggerConcept();
    }
}

}
