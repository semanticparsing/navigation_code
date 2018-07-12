#include "DMCoreAgent.h"
#include <iostream>
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Registry.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"
#include "Projects/ChinaMobile/CRS/Src/Session/ExecuteApi.h"

namespace TrioTDM {

std::vector<std::string> CDMCoreAgent::s_floor_status_lables = std::vector<std::string>();

static bool InitStaticData() {
    CDMCoreAgent::s_floor_status_lables.push_back("unknown");
    CDMCoreAgent::s_floor_status_lables.push_back("user");
    CDMCoreAgent::s_floor_status_lables.push_back("system");
    CDMCoreAgent::s_floor_status_lables.push_back("free");
    return true;
}

static bool s_ret = InitStaticData();

#define MODIFY_NEED_UPDATE_STATE(agent) \
    do {\
        if ( agent != NULL && agent->IsNeedDeclareExpectations() ) {\
            VLOG(DEBUG) << __FUNCTION__ << ", _is_need_update_state = true, AgentName: " << agent->GetName();\
            _is_need_update_state = true;\
        }\
    } while(0)\

Json::Value TExecutionStackItem::ToJson() const {
    Json::Value j_data;

    JSON_ADD_ITEM(j_data, iEHIndex);
    if ( pdaAgent != NULL ) {
        std::string pdaAgentName = pdaAgent->GetName();
        JSON_ADD_ITEM(j_data, pdaAgentName);
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", pdaAgent is NULL.";
    }
    return j_data;
}

bool TExecutionStackItem::FromJson(const Json::Value& j_data, CRegistryAgent* registry_agent) {
    JSON_ITEM_TO_INT(j_data, iEHIndex);
    std::string pdaAgentName;
    JSON_ITEM_TO_STRING(j_data, pdaAgentName);

    if ( registry_agent != NULL ) {
        pdaAgent = (CDialogAgent*)registry_agent->operator[](pdaAgentName);
        if ( pdaAgent == NULL ) {
            VLOG(WARNING) << __FUNCTION__ << ", pdaAgent is NULL, pdaAgentName: " << pdaAgentName;
            return false;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", registry_agent is NULL.";
        return false;
    }
    return true;
}

Json::Value ExecutionStackToJson(const TExecutionStack& stack) {
    Json::Value j_data( Json::arrayValue );

    for ( TExecutionStack::const_iterator iter = stack.begin();iter != stack.end(); ++iter ) {
        j_data.append( iter->ToJson() );
    }
    return j_data;
}

bool ExecutionStackFromJson(const Json::Value& j_data, TExecutionStack& stack, CRegistryAgent* registry_agent) {
    if ( !j_data.isArray() ) {
        VLOG(WARNING) << __FUNCTION__ << ", JData is Not array.";
        return false;
    }
    stack.clear();
    for ( size_t i = 0;i < j_data.size(); ++i ) {
        TExecutionStackItem item;
        if ( !item.FromJson( j_data[i], registry_agent ) ) {
            VLOG(WARNING) << __FUNCTION__ << ", Index: " << i;
            return false;
        }
        stack.push_back( item );
    }
    return true;
}

Json::Value TExecutionHistoryItem::ToJson() const {
    Json::Value j_data;

    JSON_ADD_ITEM(j_data, sCurrentAgent);
    JSON_ADD_ITEM(j_data, sCurrentAgentType);
    JSON_ADD_ITEM(j_data, sScheduledBy);
    JSON_ADD_ITEM(j_data, bScheduled);
    JSON_ADD_ITEM(j_data, bExecuted);
    JSON_ADD_ITEM(j_data, bCommitted);
    JSON_ADD_ITEM(j_data, bCanceled);
    JSON_ADD_ITEM(j_data, timeScheduled);
    JSON_ADD_ITEM(j_data, timeTerminated);
    JSON_ADD_ITEM(j_data, iStateHistoryIndex);
    
    Json::Value jVtExecutionTimes( Json::arrayValue );
    for(size_t i = 0;i < vtExecutionTimes.size(); ++i ) {
        jVtExecutionTimes.append( vtExecutionTimes[i] );
    }
    JSON_ADD_ITEM(j_data, jVtExecutionTimes);
    
    return j_data;
}

bool TExecutionHistoryItem::FromJson(const Json::Value& j_data) {

    JSON_ITEM_TO_STRING(j_data, sCurrentAgent);
    JSON_ITEM_TO_STRING(j_data, sCurrentAgentType);
    JSON_ITEM_TO_STRING(j_data, sScheduledBy);

    JSON_ITEM_TO_BOOL(j_data, bScheduled);
    JSON_ITEM_TO_BOOL(j_data, bExecuted);
    JSON_ITEM_TO_BOOL(j_data, bCommitted);
    JSON_ITEM_TO_BOOL(j_data, bCanceled);

    JSON_ITEM_TO_INT(j_data, iStateHistoryIndex);

    JSON_ITEM_TO_UINT(j_data, timeScheduled);
    JSON_ITEM_TO_UINT(j_data, timeTerminated);

    Json::Value jVtExecutionTimes = j_data["jVtExecutionTimes"];
    if ( !jVtExecutionTimes.isArray() ) {
        VLOG(WARNING) << __FUNCTION__ << ", jVtExecutionTimes parse fail.";
        return false;
    }
    for ( size_t i = 0;i < jVtExecutionTimes.size(); ++i ) {
        if ( jVtExecutionTimes[i].isIntegral() ) {
            vtExecutionTimes.push_back(jVtExecutionTimes[i].asUInt());
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", jVtExecutionTimes parse fail, Index: " << i;
            return false;
        }
    }

    return true;
}

Json::Value ExecutionHistoryToJson(const CExecutionHistory& execution_history) {
    Json::Value j_data( Json::arrayValue );

    for ( size_t i = 0;i < execution_history.size(); ++i ) {
        j_data.append( execution_history[i].ToJson() );
    }
    return j_data;
}

bool ExecutionHistoryFromJson(const Json::Value& j_data, CExecutionHistory& execution_history) {
    if ( !j_data.isArray() ) {
        VLOG(WARNING) << __FUNCTION__ << ", JData is Not Array.";
        return false;
    }
    execution_history.clear();
    for ( size_t i = 0;i < j_data.size(); ++i ) {
        TExecutionHistoryItem item;
        if ( !item.FromJson( j_data[i] ) ) {
            VLOG(WARNING) << __FUNCTION__ << ", TExecutionHistoryItem Parse Fail, Index: " << i;
            return false;
        }
        execution_history.push_back( item );
    }
    return true;
}

Json::Value TBinding::ToJson() const {
    Json::Value j_data;

    JSON_ADD_ITEM(j_data, bBlocked);
    JSON_ADD_ITEM(j_data, sGrammarExpectation);
    JSON_ADD_ITEM(j_data, sValue);
    JSON_ADD_ITEM(j_data, fConfidence);
    JSON_ADD_ITEM(j_data, iLevel);
    JSON_ADD_ITEM(j_data, sAgentName);
    JSON_ADD_ITEM(j_data, sConceptName);
    JSON_ADD_ITEM(j_data, sReasonDisabled);

    return j_data;
}

bool TBinding::FromJson(const Json::Value& j_data) {
    
    JSON_ITEM_TO_BOOL(j_data, bBlocked);
    JSON_ITEM_TO_STRING(j_data, sGrammarExpectation);
    JSON_ITEM_TO_STRING(j_data, sValue);
    JSON_ITEM_TO_DOUBLE(j_data, fConfidence);
    JSON_ITEM_TO_INT(j_data, iLevel);
    JSON_ITEM_TO_STRING(j_data, sAgentName);
    JSON_ITEM_TO_STRING(j_data, sConceptName);
    JSON_ITEM_TO_STRING(j_data, sReasonDisabled);

    return true;
}

Json::Value TForcedConceptUpdate::ToJson() const {
    Json::Value j_data;

    JSON_ADD_ITEM(j_data, sConceptName);
    JSON_ADD_ITEM(j_data, iType);
    JSON_ADD_ITEM(j_data, bUnderstanding);

    return j_data;
}

bool TForcedConceptUpdate::FromJson(const Json::Value& j_data) {
    
    JSON_ITEM_TO_STRING(j_data, sConceptName);
    JSON_ITEM_TO_INT(j_data, iType);
    JSON_ITEM_TO_BOOL(j_data, bUnderstanding);

    return true;
}

Json::Value TBindingsDescr::ToJson() const {
    Json::Value j_data;

    JSON_ADD_ITEM(j_data, sEventType);
    JSON_ADD_ITEM(j_data, bNonUnderstanding);
    JSON_ADD_ITEM(j_data, iConceptsBound);
    JSON_ADD_ITEM(j_data, iSlotsMatched);
    JSON_ADD_ITEM(j_data, iSlotsBlocked);
    
    Json::Value jVbBindings( Json::arrayValue );
    for ( size_t i = 0;i < vbBindings.size(); ++i ) {
        jVbBindings.append( vbBindings[i].ToJson() );
    }
    JSON_ADD_ITEM(j_data, jVbBindings);

    Json::Value jVfcuForcedUpdates( Json::arrayValue );
    for ( size_t i = 0;i < vfcuForcedUpdates.size(); ++i ) {
        jVfcuForcedUpdates.append( vfcuForcedUpdates[i].ToJson() );
    }
    JSON_ADD_ITEM(j_data, jVfcuForcedUpdates);
    
    return j_data;
}

bool TBindingsDescr::FromJson(const Json::Value& j_data) {
    
    JSON_ITEM_TO_STRING(j_data, sEventType);
    JSON_ITEM_TO_BOOL(j_data, bNonUnderstanding);
    JSON_ITEM_TO_INT(j_data, iConceptsBound);
    JSON_ITEM_TO_INT(j_data, iSlotsMatched);
    JSON_ITEM_TO_INT(j_data, iSlotsBlocked);

    Json::Value jVbBindings;
    JSON_ITEM_TO_ARRAY(j_data, jVbBindings);
    for ( size_t i = 0;i < jVbBindings.size(); ++i ) {
        TBinding binding;
        if ( !binding.FromJson( jVbBindings[i] ) ) {
            VLOG(WARNING) << __FUNCTION__ << ", TBinding parse Fail, Index: " << i;
            return false;
        }
        vbBindings.push_back( binding );
    }

    Json::Value jVfcuForcedUpdates;
    JSON_ITEM_TO_ARRAY(j_data, jVfcuForcedUpdates);
    for ( size_t i = 0;i < jVfcuForcedUpdates.size(); ++i ) {
        TForcedConceptUpdate item;
        if ( !item.FromJson(jVfcuForcedUpdates[i]) ) {
            VLOG(WARNING) << __FUNCTION__ << ", TForcedConceptUpdate parse fail, Index: " << i;
            return false;
        }
        vfcuForcedUpdates.push_back( item );
    }

    return true;
}

Json::Value BindingHistoryToJson( const TBindingHistory& binding_history ) {
    Json::Value j_data( Json::arrayValue );

    for ( TBindingHistory::const_iterator c_iter = binding_history.begin();c_iter != binding_history.end(); ++c_iter) {
        j_data.append( c_iter->ToJson() );
    }
    return j_data;
}

bool BindingHistoryFromJson(const Json::Value& j_data, TBindingHistory& binding_history) {
    if ( !j_data.isArray() ) {
        VLOG(WARNING) << __FUNCTION__ << ", JData is not Array.";
        return false;
    }
    binding_history.clear();
    for ( size_t i = 0;i < j_data.size(); ++i ) {
        TBindingsDescr item;
        if ( !item.FromJson(j_data[i]) ) {
            VLOG(WARNING) << __FUNCTION__ << ", TBindingsDescr parse fail, Index: " << i;
            return false;
        }
        binding_history.push_back( item );
    }
    return true;
}

Json::Value TSystemAction::ToJson() const {
    Json::Value j_data;

    Json::Value jSetcpRequests(Json::arrayValue);
    for ( std::set<CConcept*>::const_iterator c_iter = setcpRequests.begin();c_iter != setcpRequests.end(); ++c_iter) {
        jSetcpRequests.append( (*c_iter)->GetAgentQualifiedName() );
    }
    JSON_ADD_ITEM(j_data, jSetcpRequests);

    Json::Value jSetcpExplicitConfirms(Json::arrayValue);
    for ( std::set<CConcept*>::const_iterator c_iter = setcpExplicitConfirms.begin();c_iter != setcpExplicitConfirms.end(); ++c_iter) {
        jSetcpExplicitConfirms.append( (*c_iter)->GetAgentQualifiedName() );
    }
    JSON_ADD_ITEM(j_data, jSetcpExplicitConfirms);
    
    Json::Value jSetcpImplicitConfirms(Json::arrayValue);
    for ( std::set<CConcept*>::const_iterator c_iter = setcpImplicitConfirms.begin();c_iter != setcpImplicitConfirms.end(); ++c_iter) {
        jSetcpImplicitConfirms.append( (*c_iter)->GetAgentQualifiedName() );
    }
    JSON_ADD_ITEM(j_data, jSetcpImplicitConfirms);
    
    Json::Value jSetcpUnplannedImplicitConfimrs(Json::arrayValue);
    for ( std::set<CConcept*>::const_iterator c_iter = setcpUnplannedImplicitConfirms.begin();c_iter != setcpUnplannedImplicitConfirms.end(); ++c_iter) {
        jSetcpUnplannedImplicitConfimrs.append( (*c_iter)->GetAgentQualifiedName() );
    }
    JSON_ADD_ITEM(j_data, jSetcpUnplannedImplicitConfimrs);

    return j_data;
}

bool TSystemAction::FromJson(const Json::Value& j_data, CDialogAgent* root) {
    Json::Value jSetcpRequests;
    JSON_ITEM_TO_ARRAY(j_data, jSetcpRequests);
    setcpRequests.clear();
    for (size_t i = 0;i < jSetcpRequests.size(); ++i ) {
        if ( jSetcpRequests[i].isString() ) {
            setcpRequests.insert( &( root->C(jSetcpRequests[i].asString()) ) );
        }
    }
    
    Json::Value jSetcpExplicitConfirms;
    JSON_ITEM_TO_ARRAY(j_data, jSetcpExplicitConfirms);
    setcpExplicitConfirms.clear();
    for (size_t i = 0;i < jSetcpExplicitConfirms.size(); ++i ) {
        if ( jSetcpExplicitConfirms[i].isString() ) {
            setcpExplicitConfirms.insert( &( root->C(jSetcpExplicitConfirms[i].asString()) ) );
        }
    }

    Json::Value jSetcpImplicitConfirms;
    JSON_ITEM_TO_ARRAY(j_data, jSetcpImplicitConfirms);
    setcpImplicitConfirms.clear();
    for (size_t i = 0;i < jSetcpImplicitConfirms.size(); ++i ) {
        if ( jSetcpImplicitConfirms[i].isString() ) {
            setcpImplicitConfirms.insert( &( root->C(jSetcpImplicitConfirms[i].asString()) ) );
        }
    }
    
    Json::Value jSetcpUnplannedImplicitConfimrs;
    JSON_ITEM_TO_ARRAY(j_data, jSetcpUnplannedImplicitConfimrs);
    setcpUnplannedImplicitConfirms.clear();
    for (size_t i = 0;i < jSetcpUnplannedImplicitConfimrs.size(); ++i ) {
        if ( jSetcpUnplannedImplicitConfimrs[i].isString() ) {
            setcpUnplannedImplicitConfirms.insert( &( root->C(jSetcpUnplannedImplicitConfimrs[i].asString()) ) );
        }
    }

    return true;
}

CDMCoreAgent* CDMCoreAgent::s_instance = NULL;

CDMCoreAgent::CDMCoreAgent(const std::string& name, const std::string& configuration, const std::string& type)
    : CAgent(name, configuration, type) {
        _root = NULL;
        _output_manager_agent = NULL;
        _dtt_manager_agent = NULL;
        _registry_agent = NULL;
        _mobile_session = NULL;
        _gain_concept_api = NULL;
        _execute_api = NULL;
        _is_focus_claims_phase_flag = false;
        _is_agenda_modified_flag = false;
        _floor_status = fsSystem;
        _turn_number = 0;
        _timeout_period = 0;
        _start_over_funct = NULL;
        _is_need_update_state = false;
        _is_trigger_new_business = false;
        _nonunderstanding_threshold = 0.0;
        _default_timeout_period = 0;
        _default_nonunderstanding_threshold = 0.0;
        
}

CDMCoreAgent::~CDMCoreAgent() {
}

void CDMCoreAgent::Initialize() {
ST_TIME 
    VLOG(DATA) << __FUNCTION__;
    if ( CDTTManagerAgent::Instance() == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", CDTTManagerAgent::Instance() is NULL, AgentName: " << _name;
        return ;
    }
    
    if ( CRegistryAgentType::Instance() == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", CRegistryAgentType::Instance is NULL, AgentName: " << _name;
        return ;
    }
    
    if ( _registry_agent != NULL ) {
        delete _registry_agent;
        _registry_agent = NULL;
    }
    _registry_agent = new CRegistryAgent();

    _output_manager_agent = (COutputManagerAgent*)CRegistryAgentType::Instance()->CreateAgent("COutputManagerAgent", "OutputManagerAgent." + _session);
    _output_manager_agent->SetOwnerCoreAgent( this );
    _output_manager_agent->Register();
    
    _root_name = GenRootName( _session );
    _root = CDTTManagerAgent::Instance()->GetDialogTaskTreeRoot(_root_name, this);
    if ( _root == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", Can not Get root.";
        return ;
    }
    
    if ( _mobile_session != NULL ) {
        delete _mobile_session;
        _mobile_session = NULL;
    }
    _mobile_session = new ChinaMobileProvince::ChinaMobileSession();
    
    if ( _gain_concept_api != NULL ) {
        delete _gain_concept_api;
        _gain_concept_api = NULL;
    }

    _gain_concept_api = new CGainConceptApi( _mobile_session );

    if ( _execute_api != NULL ) {
        delete _execute_api;
        _execute_api = NULL;
    }
    _execute_api = new CExecuteApi( _mobile_session );
    _owner_core_agent = this;
CAL_TIME
}

bool CDMCoreAgent::InitDialogState(const std::string& session, const std::string& telephone) {
    VLOG(DATA) << __FUNCTION__ << ", session: " << session << ", telephone: " << telephone;
    _session = session;
    _telephone = telephone;

    if ( _session.empty() ) {
        VLOG(FATAL) << __FUNCTION__ << ", session can not is empty.";
        return false;
    }
    Reset();

    SetFloorStatus(fsFree);
    ContinueWith(this, _root);

    if ( _mobile_session != NULL ) {
        _mobile_session->Init(session, telephone);
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _mobile_session is NULL.";
        return false;
    }
   
    return true;
}

bool CDMCoreAgent::DeclareSaveData(std::map<std::string, std::string>& agents) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;
    Json::Value j_data;

    JSON_ADD_ITEM(j_data, _timeout_period);
    JSON_ADD_ITEM(j_data, _turn_number);
    JSON_ADD_ITEM(j_data, _is_focus_claims_phase_flag);
    JSON_ADD_ITEM(j_data, _is_agenda_modified_flag);
    JSON_ADD_ITEM(j_data, _is_trigger_new_business);
    JSON_ADD_ITEM(j_data, _nonunderstanding_threshold);
    JSON_ADD_ITEM(j_data, _telephone);
    JSON_ADD_ITEM(j_data, _floor_status);
    Json::Value j_execution_stack = ExecutionStackToJson(_execution_stack);
    JSON_ADD_ITEM(j_data, j_execution_stack);
    
    Json::Value j_execution_history = ExecutionHistoryToJson(_execution_history);
    JSON_ADD_ITEM(j_data, j_execution_history);
    
    Json::Value j_binding_history = BindingHistoryToJson(_binding_history);
    JSON_ADD_ITEM(j_data, j_binding_history);
    
    Json::Value j_system_action = _system_action.ToJson();
    JSON_ADD_ITEM(j_data, j_system_action);
   
    Json::FastWriter fast_writer;
    agents[_name] = fast_writer.write( j_data );

    if ( _output_manager_agent != NULL ) {
        if ( !_output_manager_agent->DeclareSaveData( agents) ) {
            VLOG(WARNING) << __FUNCTION__ << ", _output_manager_agent DeclareSaveData Fail, AgentName: " << _name;
            return false;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _output_manager_agent is NULL, AgentName: " << _name;
    }

    return true;
}

bool CDMCoreAgent::Load(const std::map<std::string, std::string>& agents) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << _name;

    std::map<std::string, std::string>::const_iterator c_iter = agents.find( _name );
    if ( c_iter == agents.end() ) {
        VLOG(WARNING) << __FUNCTION__ << ", Not Find Agent, AgentName: " << _name;
        return false;
    }
    Json::Reader reader;
    Json::Value j_data;
    VLOG(DEBUG) << __FUNCTION__ << ", JsonString:\n" << c_iter->second;
    if ( !reader.parse(c_iter->second, j_data) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Parse Json Fail, strg: " << c_iter->second;
        return false;
    }

    JSON_ITEM_TO_INT(j_data, _timeout_period);
    JSON_ITEM_TO_INT(j_data, _turn_number);
    JSON_ITEM_TO_BOOL(j_data, _is_focus_claims_phase_flag);
    JSON_ITEM_TO_BOOL(j_data, _is_agenda_modified_flag);
    JSON_ITEM_TO_BOOL(j_data, _is_trigger_new_business);
    JSON_ITEM_TO_DOUBLE(j_data, _nonunderstanding_threshold);
    JSON_ITEM_TO_STRING(j_data, _telephone);
    JSON_ITEM_TO_ENUM(j_data, _floor_status, TFloorStatus);
    
    Json::Value j_execution_stack;
    JSON_ITEM_TO_ARRAY(j_data, j_execution_stack);
    // TODO
    // JSON_ITEM_TO_OBJECT(j_data, j_execution_stack);
    if ( !ExecutionStackFromJson(j_execution_stack, _execution_stack, _registry_agent) ) {
        VLOG(WARNING) << __FUNCTION__ << ", ExecutionStackFromJson Fail.";
        return false;
    }
    
    Json::Value j_execution_history;
    JSON_ITEM_TO_ARRAY(j_data, j_execution_history);
    if ( !ExecutionHistoryFromJson(j_execution_history, _execution_history) ) {
        VLOG(WARNING) << __FUNCTION__ << ", ExecutionHistoryFromJson Fail.";
        return false;

    }
    
    Json::Value j_binding_history;
    JSON_ITEM_TO_ARRAY(j_data, j_binding_history);
    if ( !BindingHistoryFromJson(j_binding_history, _binding_history) ) {
        VLOG(WARNING) << __FUNCTION__ << ", BindingHistoryFromJson Fail.";
        return false;
    }
    
    Json::Value j_system_action;
    JSON_ITEM_TO_OBJECT(j_data, j_system_action);
    if ( !_system_action.FromJson(j_system_action, _root) ) {
        VLOG(WARNING) << __FUNCTION__ << ", _system_action.FromJson Fail.";
        return false;
    }
    
    if ( _output_manager_agent != NULL ) {
        if ( !_output_manager_agent->Load( agents) ) {
            VLOG(WARNING) << __FUNCTION__ << ", _output_manager_agent Load Fail, AgentName: " << _name;
            return false;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _output_manager_agent is NULL, AgentName: " << _name;
    }

    if ( _mobile_session != NULL ) {
        _mobile_session->Init(_session, _telephone);
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _mobile_session is NULL.";
        return false;
    }

    return true;
}

void CDMCoreAgent::OnDestruction() {

    if ( !DestroyDialogTree() ) {
        VLOG(WARNING) << __FUNCTION__ << ", DestroyDialogTree Fail, AgentName: " << _name;
    }

    if ( _output_manager_agent != NULL ) {
        delete _output_manager_agent;
        _output_manager_agent = NULL;
    }

    if ( _mobile_session != NULL ) {
        delete _mobile_session;
        _mobile_session = NULL;
    }

    if ( _gain_concept_api != NULL ) {
        delete _gain_concept_api;
        _gain_concept_api = NULL;
    }

    if ( _execute_api != NULL ) {
        delete _execute_api;
        _execute_api = NULL;
    }

    if ( _registry_agent != NULL ) {
        delete _registry_agent;
        _registry_agent = NULL;
    }
}

CAgent* CDMCoreAgent::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CDMCoreAgent(name, configuration);
}

CDMCoreAgent* CDMCoreAgent::Instance() {
    if ( s_instance == NULL && CRegistryAgentType::Instance() != NULL ) {
        s_instance = (CDMCoreAgent*)CRegistryAgentType::Instance()->CreateAgent("CDMCoreAgent", "DMCoreAgent");
    }
    return s_instance;
}

void CDMCoreAgent::Reset() {
    _execution_stack.clear();
    _execution_history.clear();
    _binding_history.clear();
    _agenda.celSystemExpectations.clear();
    _agenda.vCompiledExpectations.clear();
    _is_trigger_new_business = false;
    if ( _root != NULL ) {
        _root->Reset();
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _root is NULL.";
    }
    if ( _output_manager_agent != NULL ) {
        _output_manager_agent->Reset();
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _output_manager_agent is NULL.";
    }
}

void CDMCoreAgent::SetSession(const std::string& session) {
    _session = session;
}

std::string CDMCoreAgent::GetSession() {
    return _session;
}

std::string CDMCoreAgent::GetTelephone() {
    return _telephone;
}

CDialogAgent* CDMCoreAgent::GetTaskRoot() {
    return _root;
}

COutputManagerAgent* CDMCoreAgent::GetOutputManagerAgent() {
    return _output_manager_agent;
}

CRegistryAgent* CDMCoreAgent::GetRegistryAgent() {
    return _registry_agent;
}

CGainConceptApi* CDMCoreAgent::GetGainConceptApi() {
    return _gain_concept_api;
}

CExecuteApi* CDMCoreAgent::GetExecuteApi() {
    return _execute_api;
}

bool CDMCoreAgent::IsTriggerNewBusiness() {
    return _is_trigger_new_business;
}

bool CDMCoreAgent::SaveSession() {
    if ( _mobile_session != NULL ) {
        return _mobile_session->Save();
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _mobile_session is NULL.";
        return false;
    }
}

bool CDMCoreAgent::LoadSession(const std::string& inter_idx) {
    if ( _mobile_session != NULL ) {
        return _mobile_session->Load(inter_idx);
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _mobile_session is NULL.";
        return false;
    }
}

std::string CDMCoreAgent::GenRootName(const std::string& session) {
    return "ROOT." + session;
}

/*
TDialogExecuteReturnCode CDMCoreAgent::Execute(std::vector<COutput>& outputs) {
    TDialogExecuteReturnCode return_code = ActualExecute(outputs);
    // TODO 更新数据前 完成出栈
    // PopCompletedFromExecutionStack();
    return return_code;
}
*/

TDialogExecuteReturnCode CDMCoreAgent::Execute(std::vector<COutput>& outputs) {
    while ( !_execution_stack.empty() ) {
        VLOG(DEBUG) << __FUNCTION__ <<  ", Execute next step.";
        PopCompletedFromExecutionStack();
        if ( _execution_stack.empty() ) {
            VLOG(DATA) << __FUNCTION__ << ", stop execute, _execution_stack is empyt.";
            return dercError;
        }
        if ( GetFloorStatus() == fsFree ) {
            VLOG(DEBUG) << __FUNCTION__ << ", GetFloorStatus() == fsFree..";
            if ( CGroundingManagerAgent::Instance()->HasPendingRequests() ||
                    CGroundingManagerAgent::Instance()->HasScheduledConceptGroundingRequests() ) {
                 CGroundingManagerAgent::Instance()->Run();
            }
            int popped = PopCompletedFromExecutionStack();
            while ( popped > 0 && CGroundingManagerAgent::Instance()->HasUnprocessedConceptGroundingRequests() ) {
                CGroundingManagerAgent::Instance()->Run();
                popped = PopCompletedFromExecutionStack();
            }
        }
        if ( _is_focus_claims_phase_flag ) {
            if ( AssembleFocusClaims() ) {
                ResolveFocusShift();
            }
            _is_focus_claims_phase_flag = false;
        }
        if ( _execution_stack.empty() ) {
            VLOG(DATA) << __FUNCTION__ << ", stop execute, _execution_stack is empyt.";
            return dercError;
        }
        CDialogAgent* agent = GetAgentInFocus();
        if ( agent == NULL ) {
            VLOG(DATA) << __FUNCTION__ << ", stop execute, agent is NULL.";
            return dercError;
        }
        VLOG(DATA) << __FUNCTION__ << ", ExecuteAgentName: " << agent->GetName();
        _execution_history[_execution_stack.front().iEHIndex].vtExecutionTimes.push_back(GetTime());
        COutput output;
        TDialogExecuteReturnCode result = agent->Execute(&output);
        if ( _execution_stack.empty() ) {
            VLOG(DATA) << __FUNCTION__ << ", stop execute, _execution_stack is empyt.";
            return dercError;
        }
        if ( output.IsOutput() && result != dercError ) {
            outputs.push_back(output);
        }
        _execution_history[_execution_stack.front().iEHIndex].bExecuted = true;

        switch ( result ) {
            case dercContinueExecution:
                break;
            case dercFinishDialogAndCloseSession:
                return dercFinishDialogAndCloseSession;
            case dercRestartDialog:
                StartOver();
                break;
            case dercYieldFloor:
                SetFloorStatus(fsUser);
                return dercYieldFloor;
            case dercTakeFloor:
                SetFloorStatus(fsSystem);
                return dercYieldFloor;
            case dercWaitForEvent:
                return dercYieldFloor;
            case dercError: {
                    VLOG(DEBUG) << __FUNCTION__ << ", Execute get dercError... AgentName: " << agent->GetName();
                    agent->OnExecuteFail();
                }
                break;
            default:
                VLOG(WARNING) << __FUNCTION__ << ", agent execute error, AgentName: " << agent->GetName();
                return dercError;
        }
    }
    VLOG(DATA) << __FUNCTION__ << ", over, _execution_stack is empty.";
    return dercError;
}

void CDMCoreAgent::AcquireNextEvent(const CInteractionEvent& event) {
    VLOG(DATA) << __FUNCTION__ << ", new event, type: " << event.GetType();
    _is_trigger_new_business = false;
    SignalFocusClaimsPhase();
    CDialogAgent* cur_agent = GetCurrentMainTopicAgent();
    if ( cur_agent != NULL ) {
        AssembleExpectationAgenda( cur_agent );
    } else {
        VLOG(FATAL) << __FUNCTION__ << ", CurrentMainTopicAgent is NULL.";
        return ;
    }
    std::string dman = cur_agent->GetDialogAgentName();
    TBindingsDescr bindings;
    BindConcepts(dman, bindings, event);
    _binding_history.push_back(bindings);

    CDialogAgent* agent = GetAgentInFocus();
    if ( agent == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", GetAgentInFocus is NULL.";
        return ;
    }
    agent->SetLastBindingsIndex( _binding_history.size()-1 );

    if ( event.HasProperty(dman, "[floor_status]") ) {
        SetFloorStatus(StringToFloorStatus(event.GetStringProperty(dman, "[floor_status]")));
    }
    if ( event.GetType() == IET_USER_UTT_START ) {
        ;
    } else if ( event.GetType() == IET_USER_UTT_END ||
            event.GetType() == IET_GUI ) {
        if ( event.IsComplete() ) {
            agent->SetLastInputIndex(_turn_number);
            agent->IncrementTurnsInFocusCounter();
            _turn_number += 1;
            CGroundingManagerAgent::Instance()->RequestTurnGrounding();
            return ;
        }
    } else if ( event.GetType() == IET_SYSTEM_UTT_START ) {
        /*
        COutputManagerAgent::Instance()->PreliminaryNotify(event.GetIntProperty("[utt_count]"),
                event.GetStringProperty("[tagged_prompt]"));
        */
        ;
    } else if ( event.GetType() == IET_SYSTEM_UTT_END ) {
        // TODO
        /*
        pOutputManager->Notify(pieEvent->GetIntProperty("[utt_count]"), 
			pieEvent->GetIntProperty("[bargein_pos]"),
			pieEvent->GetStringProperty("[conveyance]"),
			pieEvent->GetStringProperty("[tagged_prompt]"));

		Log(DMCORE_STREAM, "Processed output notification.");
        */
    } else if ( event.GetType() == IET_DIALOG_STATE_CHANGE ) {
        // TODO
        /*
        pStateManager->UpdateState();
		pStateManager->BroadcastState();
        */
    } else {
    }
}

void CDMCoreAgent::UpdateInputLineConfiguration() {
    /*
    // grab the first dialog agent from the stack
    CDialogAgent* pdaCurrent = _execution_stack.executionStack.front().pdaAgent;

    // obtain the hash describing the entire configuration
    STRING2STRING s2sConfig = pdaCurrent->GetInputLineConfiguration();
    
    // log it
    VLOG(DEBUG) << "Input line configuration dumped below: " << S2SHashToString(s2sConfig, "\n");
    
    if (s2sConfig.size() > 0) {

        //ZW : TODO: Dynamic NLU configuration
            
    }
    */
}

void CDMCoreAgent::RegisterBindingFilter(const std::string& name, TBindingFilterFunct bff_filter) {
    if ( _str2bff_filters.find(name) != _str2bff_filters.end() ) {
        VLOG(WARNING) << "Could not add binding filter " << name << ", it already exists.";
    } else {
        _str2bff_filters.insert(std::make_pair(name, bff_filter));
    }
}

int CDMCoreAgent::GetIntSessionID() {
    return 0;
}

void CDMCoreAgent::ClearCurrentSystemAction() {
    _system_action.setcpRequests.clear();
    _system_action.setcpExplicitConfirms.clear();
    _system_action.setcpImplicitConfirms.clear();
    _system_action.setcpUnplannedImplicitConfirms.clear();
}

void CDMCoreAgent::ComputeCurrentSystemAction() {
    VLOG(DEBUG) << __FUNCTION__;
    if ( _root == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _root is NULL, Name: " << _name;
        return ;
    }
    bool bFoundFocus = false;
    for( size_t l = 0;
            !bFoundFocus && (l < _agenda.vCompiledExpectations.size());
            l++ ) {
        bFoundFocus = true;

        std::map<std::string, std::vector<int> >::iterator iPtr;
        // iterate through the compiled expectations from that level
        for( iPtr = _agenda.vCompiledExpectations[l].mapCE.begin();
                iPtr != _agenda.vCompiledExpectations[l].mapCE.end();
                iPtr++ ) {
            std::string sSlotExpected = iPtr->first;
            std::vector<int>& rvIndices = iPtr->second;

            // go through the indices
            for(size_t i = 0; i < rvIndices.size(); i++) {
                TConceptExpectation& rceExpectation = 
                    _agenda.celSystemExpectations[rvIndices[i]];
                CConcept* pConcept = &(rceExpectation.pDialogAgent->
                    C(rceExpectation.sConceptName));
                std::string sAgentName = rceExpectation.pDialogAgent->GetName();
                if( sAgentName.find("_ImplicitConfirmExpect") != -1 ) {
                    // find the name of the confirmed concept
                    std::string sFoo, sConfirmedConcept;
                    SplitOnFirst(sAgentName, "[", sFoo, sConfirmedConcept);
                    sConfirmedConcept = TrimRight(sConfirmedConcept, "]");
                    SignalImplicitConfirmOnConcept( &(_root->C(sConfirmedConcept)));

                    bFoundFocus = false;
                    continue;
                }
                // now check if the agent is an explicit confirmation
                if(sAgentName.find("_ExplicitConfirm") != -1) {
                    // find the name of the confirmed concept
                    std::string sFoo, sConfirmedConcept;
                    SplitOnFirst(sAgentName, "[", sFoo, sConfirmedConcept);
                    sConfirmedConcept = TrimRight(sConfirmedConcept, "]");
                    SignalExplicitConfirmOnConcept( &(_root->C(sConfirmedConcept)) );
                }

                if( !rceExpectation.bDisabled ) {
                    _system_action.setcpRequests.insert(pConcept);
                    for(size_t i = 0; i < rceExpectation.vsOtherConceptNames.size(); i++) {
                        _system_action.setcpRequests.insert(&(rceExpectation.pDialogAgent->
                            C(rceExpectation.vsOtherConceptNames[i])));
                    }                    
                }                
            }
        }
    }

    VLOG(DEBUG) << __FUNCTION__ << "System action dumped below " << CurrentSystemActionToString();
    
}

std::string CDMCoreAgent::CurrentSystemActionToString() {
    return SystemActionToString(_system_action);
}

std::string CDMCoreAgent::SystemActionToString(const TSystemAction& system_action) {
    // now log the current system action
    std::string sResult = "REQUEST(";
    std::set<CConcept *>::iterator iPtr;
    for(iPtr = system_action.setcpRequests.begin(); 
        iPtr != system_action.setcpRequests.end();
        iPtr++) {
        sResult += FormatString("%s,", (*iPtr)->GetAgentQualifiedName().c_str());
    }
    sResult = TrimRight(sResult, ",");
    sResult += ")\nEXPL_CONF(";
    for(iPtr = system_action.setcpExplicitConfirms.begin(); 
        iPtr != system_action.setcpExplicitConfirms.end();
        iPtr++) {
        sResult += FormatString("%s,", (*iPtr)->GetAgentQualifiedName().c_str());
    }
    sResult = TrimRight(sResult, ",");
    sResult += ")\nIMPL_CONF(";
    for(iPtr = system_action.setcpImplicitConfirms.begin(); 
        iPtr != system_action.setcpImplicitConfirms.end();
        iPtr++) {
        sResult += FormatString("%s,", (*iPtr)->GetAgentQualifiedName().c_str());
    }
    sResult = TrimRight(sResult, ",");
    sResult += ")\nUNPLANNED_IMPL_CONF(";
    for(iPtr = system_action.setcpUnplannedImplicitConfirms.begin(); 
        iPtr != system_action.setcpUnplannedImplicitConfirms.end();
        iPtr++) {
        sResult += FormatString("%s,", (*iPtr)->GetAgentQualifiedName().c_str());
    }
    sResult = TrimRight(sResult, ",");
    sResult += ")";
    
    return sResult;
}

void CDMCoreAgent::AssembleExpectationAgenda() {
    VLOG(DEBUG) << "Expectation Agenda Assembly Phase initiated.";
    CompileExpectationAgenda();
    EnforceBindingPolicies();
    VLOG(DEBUG) << "Concept expectation agenda dumped below:" << ExpectationAgendaToString() ;
    VLOG(DEBUG) << "Expectation Agenda Assembly Phase completed ("  << _agenda.vCompiledExpectations.size() << " size)" ;
}

void CDMCoreAgent::AssembleExpectationAgenda(CDialogAgent* agent) {
    VLOG(DEBUG) << __FUNCTION__ << ", Expectation Agenda Assembly Phase initiated.";
    CompileExpectationAgenda(agent);
    EnforceBindingPolicies();
    VLOG(DEBUG) << __FUNCTION__ << ", Concept expectation agenda dumped below:" << ExpectationAgendaToString() ;
    VLOG(DEBUG) << __FUNCTION__ << ", Expectation Agenda Assembly Phase completed ("  << _agenda.vCompiledExpectations.size() << " size)" ;
}

void CDMCoreAgent::CompileExpectationAgenda() {
    VLOG(DEBUG) << __FUNCTION__ << ", Compiling Expectation Agenda ...";

    _agenda.celSystemExpectations.clear();
    _agenda.vCompiledExpectations.clear();

    int iLevel = 0;
    std::set<CDialogAgent*> setPreviouslySeenAgents;    // the set of agents already seen on the previous levels
    std::set<CDialogAgent*> setCurrentlySeenAgents;     // the set of agents seen on the current level
                                    
    TExecutionStack::iterator iPtr;
    for(TExecutionStack::iterator iPtr = _execution_stack.begin(); iPtr != _execution_stack.end(); ++iPtr ) {
        int iStartIndex = _agenda.celSystemExpectations.size();
        if ( iPtr->pdaAgent != NULL ) { 
            iPtr->pdaAgent->DeclareExpectations(_agenda.celSystemExpectations);
            TCompiledExpectationLevel celLevel;
            celLevel.pdaGenerator = iPtr->pdaAgent;

            for(size_t i = iStartIndex; i < _agenda.celSystemExpectations.size(); ++i) {
                if(setPreviouslySeenAgents.find(_agenda.celSystemExpectations[i].pDialogAgent) != setPreviouslySeenAgents.end()) {
                    continue;
                }

                setCurrentlySeenAgents.insert( _agenda.celSystemExpectations[i].pDialogAgent );

                string sSlotExpected = _agenda.celSystemExpectations[i].sGrammarExpectation;
                std::map<std::string, std::vector<int> >::iterator iPtr2;
                if( (iPtr2 = celLevel.mapCE.find(sSlotExpected)) != celLevel.mapCE.end() ) {
                    std::vector<int>& rvIndices = (*iPtr2).second;
                    rvIndices.push_back(i);
                } else {
                    std::vector<int> ivTemp;
                    ivTemp.push_back(i);
                    celLevel.mapCE.insert(
                        std::map<std::string, std::vector<int> >::value_type(sSlotExpected, ivTemp));
                } 
            }
            _agenda.vCompiledExpectations.push_back(celLevel);
            setPreviouslySeenAgents.insert(setCurrentlySeenAgents.begin(), setCurrentlySeenAgents.end());
            iLevel++;    
        }
    }

    VLOG(DEBUG) << __FUNCTION__ << ", Compiling Expectation Agenda completed.";
}

void CDMCoreAgent::CompileExpectationAgenda(CDialogAgent* agent) {
    if ( agent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", agent is NULL, Name: " << _name;
        return ;
    }
    VLOG(DEBUG) << __FUNCTION__ <<  ", AgentName: " << agent->GetName();

    if ( _root == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _root is NULL, Name: " << _name;
        return ;
    }

    _agenda.celSystemExpectations.clear();
    _agenda.vCompiledExpectations.clear();

    agent->DeclareExpectations(_agenda.celSystemExpectations);
    TCompiledExpectationLevel celLevel;
    celLevel.pdaGenerator = agent;

    for(size_t i = 0; i < _agenda.celSystemExpectations.size(); ++i) {
        string sSlotExpected = _agenda.celSystemExpectations[i].sGrammarExpectation;
        std::map<std::string, std::vector<int> >::iterator iPtr2;
            
        if( (iPtr2 = celLevel.mapCE.find(sSlotExpected)) != celLevel.mapCE.end() ) {
            std::vector<int>& rvIndices = (*iPtr2).second;
            rvIndices.push_back(i);
        } else {
            std::vector<int> ivTemp;
            ivTemp.push_back(i);
            celLevel.mapCE.insert(
                std::map<std::string, std::vector<int> >::value_type(sSlotExpected, ivTemp));
        }
    }
    _agenda.vCompiledExpectations.push_back(celLevel);

    // root 下有 所有业务流公共使用的 处理流程 ，  帮助 ， 错误， 重听 ....
    if ( agent != _root ) {
        // 只收集不是 DTS 的 Agent
        size_t st_pos = _agenda.celSystemExpectations.size();
        _root->DeclareExpectations(_agenda.celSystemExpectations, true);
        TCompiledExpectationLevel celLevel;
        celLevel.pdaGenerator = _root;
        
        for(size_t i = st_pos; i < _agenda.celSystemExpectations.size(); ++i) {
            string sSlotExpected = _agenda.celSystemExpectations[i].sGrammarExpectation;
            std::map<std::string, std::vector<int> >::iterator iPtr2;
            
            if( (iPtr2 = celLevel.mapCE.find(sSlotExpected)) != celLevel.mapCE.end() ) {
                std::vector<int>& rvIndices = (*iPtr2).second;
                rvIndices.push_back(i);
            } else {
                std::vector<int> ivTemp;
                ivTemp.push_back(i);
                celLevel.mapCE.insert(
                    std::map<std::string, std::vector<int> >::value_type(sSlotExpected, ivTemp));
            }
        }
        _agenda.vCompiledExpectations.push_back(celLevel);
    } else {
        VLOG(FATAL) << __FUNCTION__ << ", agent can not is root.";
    }
}

void CDMCoreAgent::EnforceBindingPolicies() {
    VLOG(DEBUG) << __FUNCTION__;

    for(size_t i = 0; i < _agenda.vCompiledExpectations.size(); i++) {
        std::string bind_policy = _agenda.vCompiledExpectations[i].pdaGenerator->DeclareBindingPolicy();
        if( bind_policy == WITHIN_TOPIC_ONLY ) {
            for(size_t l = i+1;l < _agenda.vCompiledExpectations.size(); ++l ) {
                for(std::map<std::string, std::vector<int> >::iterator iter = _agenda.vCompiledExpectations[l].mapCE.begin();
                        iter != _agenda.vCompiledExpectations[l].mapCE.end(); ++iter ) {
                    std::vector<int>& indexs = iter->second;
                    for(size_t ii = 0; ii < indexs.size(); ii++) {
                        int index = indexs[ii];
                        if(_agenda.celSystemExpectations[index].sExpectationType != "*") {
                            _agenda.celSystemExpectations[index].bDisabled = true;
                            _agenda.celSystemExpectations[index].sReasonDisabled = "within-topic binding policy";
                        }
                    }
                }
            }
            break;
        }
    }
    VLOG(DEBUG) <<  "Enforcing binding policies completed.";
}

std::string CDMCoreAgent::ExpectationAgendaToString() {
    std::string sResult;
    // go through all the levels of the agenda
    for(size_t l = 0; l < _agenda.vCompiledExpectations.size(); l++) {
        sResult += FormatString("\n Level %lu generated by %s", l, _agenda.vCompiledExpectations[l].pdaGenerator->GetName().c_str());
        std::map<std::string, std::vector<int> >::iterator iPtr;
        // iterate through the compiled expectations from that level
        for(iPtr = _agenda.vCompiledExpectations[l].mapCE.begin(); 
            iPtr != _agenda.vCompiledExpectations[l].mapCE.end(); 
            iPtr++) {
            string sSlotExpected = iPtr->first;
            std::vector<int>& rvIndices = iPtr->second;
            // convert expectations to string description
            for(size_t i = 0; i < rvIndices.size(); i++) {
                TConceptExpectation& rceExpectation =  _agenda.celSystemExpectations[rvIndices[i]];
                sResult += (rceExpectation.bDisabled)?"\n  X ":"\n  O ";
                sResult += rceExpectation.sGrammarExpectation + " -> (" + 
                           rceExpectation.pDialogAgent->GetName() + ")" +
                           rceExpectation.sConceptName;
                if( rceExpectation.bDisabled ) {
                    sResult += " [X-" + rceExpectation.sReasonDisabled + "]";
                }
            }
        }
    }
    // finally, return the string
    return sResult;
}

std::string CDMCoreAgent::ExpectationAgendaToBroadcastString() {
    return "";
}

std::string CDMCoreAgent::ExpectationAgendaToBroadcastString(const TExpectationAgenda& agenda) {
    return "";
}

std::string CDMCoreAgent::BindingsDescrToString(TBindingsDescr& bindings) {
    std::string sResult = "";
    // go through all the attempted bindings
    for(size_t i = 0; i < bindings.vbBindings.size(); i++) {
        if(bindings.vbBindings[i].bBlocked)
            sResult += FormatString("Fail:    Level.%d\t%s->(%s)%s [%s]\n", 
                bindings.vbBindings[i].iLevel, 
                bindings.vbBindings[i].sGrammarExpectation.c_str(), 
                bindings.vbBindings[i].sAgentName.c_str(), 
                bindings.vbBindings[i].sConceptName.c_str(), 
                bindings.vbBindings[i].sReasonDisabled.c_str());
        else 
            sResult += FormatString("Success: Level.%d\t%s(%s%s%f)->(%s)%s\n", 
                bindings.vbBindings[i].iLevel, 
                bindings.vbBindings[i].sGrammarExpectation.c_str(), 
                bindings.vbBindings[i].sValue.c_str(), 
                VAL_CONF_SEPARATOR, 
                bindings.vbBindings[i].fConfidence, 
                bindings.vbBindings[i].sAgentName.c_str(), 
                bindings.vbBindings[i].sConceptName.c_str());
    }
    // go through all the forced updates
    for(size_t i = 0; i < bindings.vfcuForcedUpdates.size(); i++) {
        if(bindings.vfcuForcedUpdates[i].iType == FCU_EXPLICIT_CONFIRM) 
            sResult += FormatString("Forced update [explicit_confirm] on %s: %s", 
                bindings.vfcuForcedUpdates[i].sConceptName.c_str(), 
                bindings.vfcuForcedUpdates[i].bUnderstanding?
                    "understanding":"non-understanding");
    }

    // finally, return
    return sResult;
}

void CDMCoreAgent::BindConcepts(const std::string& dman, TBindingsDescr& bindings, const CInteractionEvent& event) {
    VLOG(DEBUG) << __FUNCTION__;

    bindings.sEventType = event.GetType();
    bindings.iConceptsBlocked = 0;
    bindings.iConceptsBound = 0;
    bindings.iSlotsMatched = 0;
    bindings.iSlotsBlocked = 0;

    std::map<std::string, int> msiSlotsMatched;
    std::map<std::string, int> msiSlotsBlocked;
    
    for(size_t iLevel = 0; 
        iLevel < _agenda.vCompiledExpectations.size(); 
        iLevel++) {

        std::map<std::string, std::vector<int> >::iterator iPtr;
        for(iPtr = _agenda.vCompiledExpectations[iLevel].mapCE.begin();
            iPtr != _agenda.vCompiledExpectations[iLevel].mapCE.end();
            iPtr++) {
        
            std::string sSlotExpected = iPtr->first;
            std::vector<int>& rvIndices = iPtr->second;

            if( event.Matches(dman, sSlotExpected) ) {
                VLOG(DEBUG) << __FUNCTION__ << ", Event matches " << sSlotExpected;
                std::vector<int> vOpenIndices;
                std::set<CConcept *> scpOpenConcepts;

                std::vector<int> vClosedIndices;
                std::set<CConcept *> scpClosedConcepts;

                for(size_t i = 0; i < rvIndices.size(); i++) {
                    CConcept* pConcept = 
                        &(_agenda.celSystemExpectations[rvIndices[i]].pDialogAgent->C(
                            _agenda.celSystemExpectations[rvIndices[i]].sConceptName));
                    if( !_agenda.celSystemExpectations[rvIndices[i]].bDisabled ) {
                        if( scpOpenConcepts.find(pConcept) == scpOpenConcepts.end() ) {
                            vOpenIndices.push_back(rvIndices[i]);
                            scpOpenConcepts.insert(pConcept);
                            std::set<CConcept *>::iterator iPtr;
                            if((iPtr = scpClosedConcepts.find(pConcept)) !=
                                scpClosedConcepts.end()) {
                                scpClosedConcepts.erase(iPtr);
                            }
                        }
                    } else {
                        if( (scpClosedConcepts.find(pConcept) == scpClosedConcepts.end()) &&
                           (scpOpenConcepts.find(pConcept) == scpOpenConcepts.end()) ) {
                            vClosedIndices.push_back(rvIndices[i]);
                            scpClosedConcepts.insert(pConcept);
                        }
                    }
                }

                std::string sSlotValue = "";
                float fConfidence = event.GetConfidence();

                if( vOpenIndices.size() > 0 ) {
                    if( fConfidence > _nonunderstanding_threshold ) {
                        sSlotValue = event.GetValueForExpectation(dman, sSlotExpected);
                        // 多个的时候也是仅绑定第一个
                        PerformConceptBinding(sSlotExpected, sSlotValue, 
                            fConfidence, vOpenIndices[0], rvIndices, event.IsComplete());
                        // 保证一个事件中的槽位仅为一个 Concept 赋值
                        for(size_t iOtherLevel = iLevel + 1; 
                            iOtherLevel < _agenda.vCompiledExpectations.size(); 
                            iOtherLevel++) {
                            _agenda.vCompiledExpectations[iOtherLevel].\
                                mapCE.erase(sSlotExpected);
                        }
                    } else {
                        VLOG(DATA) << __FUNCTION__ << ", NonUnderstanding Slot: " << sSlotExpected;
                    }
                }

                for(size_t i = 0; i < vOpenIndices.size(); ++i ) {
                    if(i == 0) {
                        TBinding bBinding;
                        bBinding.iLevel = iLevel;
                        bBinding.sValue = sSlotValue;
                        bBinding.fConfidence = fConfidence;
                        bBinding.sAgentName = 
                            _agenda.celSystemExpectations[vOpenIndices[i]].pDialogAgent->GetName();
                        bBinding.sConceptName = 
                                _agenda.celSystemExpectations[vOpenIndices[i]].sConceptName;
                        bBinding.sGrammarExpectation = 
                            _agenda.celSystemExpectations[vOpenIndices[i]].sGrammarExpectation;
                        
                        msiSlotsMatched[bBinding.sGrammarExpectation] = 1;

                        if( fConfidence > _nonunderstanding_threshold ) {
                            bBinding.bBlocked = false;
                            
                            std::map<std::string, int>::iterator iPtr;
                            if((iPtr = msiSlotsBlocked.find(bBinding.sGrammarExpectation)) !=
                                msiSlotsBlocked.end()) {
                                msiSlotsBlocked.erase(iPtr);
                            }
                        } else {
                            bBinding.bBlocked = true;
                            bBinding.sReasonDisabled = "confidence below nonunderstanding threshold";
                            std::map<std::string, int>::iterator iPtr;
                            if((iPtr = msiSlotsBlocked.find(bBinding.sGrammarExpectation)) !=
                                msiSlotsBlocked.end()) {
                                msiSlotsBlocked.erase(iPtr);
                            }
                        }
                        bindings.vbBindings.push_back(bBinding);
                        bindings.iConceptsBound++;
                    }
                }

                // write the blocked bindings description
                for(size_t i = 0; i < vClosedIndices.size(); ++i ) {
                    TBinding bBlockedBinding;
                    bBlockedBinding.bBlocked = true;
                    bBlockedBinding.iLevel = iLevel;
                    bBlockedBinding.fConfidence = fConfidence;
                    bBlockedBinding.sAgentName = 
                        _agenda.celSystemExpectations[vClosedIndices[i]].pDialogAgent->GetName();
                    bBlockedBinding.sConceptName = 
                        _agenda.celSystemExpectations[vClosedIndices[i]].sConceptName;
                    bBlockedBinding.sGrammarExpectation = 
                        _agenda.celSystemExpectations[vClosedIndices[i]].sGrammarExpectation;
                    bBlockedBinding.sReasonDisabled = 
                        _agenda.celSystemExpectations[vClosedIndices[i]].sReasonDisabled;
                    bBlockedBinding.sValue = sSlotValue;
                    bindings.vbBindings.push_back(bBlockedBinding);
                    bindings.iConceptsBlocked++;

                    if(msiSlotsMatched.find(bBlockedBinding.sGrammarExpectation) ==
                        msiSlotsMatched.end()) {
                        msiSlotsBlocked[bBlockedBinding.sGrammarExpectation] = 1;
                    }
                }
            }
        }
    }

    // for user inputs, update the non-understanding flag
    if (event.GetType() == IET_USER_UTT_END ||
        event.GetType() == IET_GUI) {
        bindings.bNonUnderstanding = (bindings.iConceptsBound == 0);
    } else {
        bindings.bNonUnderstanding = false;
    }

    // update the slots matched and blocked information
    bindings.iSlotsMatched = msiSlotsMatched.size();
    bindings.iSlotsBlocked = msiSlotsBlocked.size();
    // finally, for user inputs, check if the statistics match what helios 
    // predicted would happen (the helios binding features)
    if (event.GetType() == IET_USER_UTT_END) {
        if( event.Matches(dman, "[slots_blocked]") && event.Matches(dman, "[slots_matched]")) {
            bool bHeliosMatch = true;
            string sH4SlotsBlocked = event.GetValueForExpectation(dman, "[slots_blocked]");
            string sH4SlotsMatched = event.GetValueForExpectation(dman, "[slots_matched]");
            if((sH4SlotsBlocked != "N/A") && (atoi(sH4SlotsBlocked.c_str()) != bindings.iSlotsBlocked) ) {
                bHeliosMatch = false;
            }
            if((sH4SlotsMatched != "N/A") && (atoi(sH4SlotsMatched.c_str()) != bindings.iSlotsMatched)) {
                bHeliosMatch = false;
            }
            if(!bHeliosMatch) {
                VLOG(WARNING) << "Helios binding features are different from RavenClaw obtained values.";
            }
        }
        PerformForcedConceptUpdates(bindings);
    } else if (event.GetType() == IET_GUI) {
        PerformForcedConceptUpdates(bindings);
    }
    VLOG(DEBUG) << __FUNCTION__ << ", bindings dumped: " << BindingsDescrToString(bindings);
}

void CDMCoreAgent::PerformConceptBinding(const std::string& name, const std::string& slot_value, float confidence, int index, const std::vector<int>& indices, bool is_complete) {
    VLOG(DEBUG) << __FUNCTION__ << ", slot: " << name << ", value:" << slot_value << 
        ", confidence: " << confidence << ", index: " << index << ", is_complete: " << is_complete;

    TConceptExpectation& expectation = _agenda.celSystemExpectations[index];

    std::string sValueToBind; 
    if(expectation.bmBindMethod == bmSlotValue) {
        sValueToBind = slot_value;
    } else if(expectation.bmBindMethod == bmExplicitValue) {
        sValueToBind = expectation.sExplicitValue;
    } else {
        std::map<std::string, TBindingFilterFunct>::iterator iPtr;
        if( (iPtr = _str2bff_filters.find(expectation.sBindingFilterName)) == _str2bff_filters.end() ) {
            VLOG(FATAL) << __FUNCTION__ << ", Can not find binding filter, FilterName: " 
                << expectation.sBindingFilterName;
            return ;
        }
        sValueToBind = (*(iPtr->second))(name, slot_value);
    }

    #ifdef ALWAYS_CONFIDENT
        confidence = 1.0;
    #endif

    std::string sBindingString = FormatString("%s%s%f", sValueToBind.c_str(), VAL_CONF_SEPARATOR, confidence);

    VLOG(DEBUG) << __FUNCTION__ <<  ", sBindingString: " << sBindingString 
        << "\tsConceptName: " << expectation.sConceptName;
    if (is_complete) {
        // TODO  CU_UPDATE_WITH_CONCEPT  Not Need....
        /*
        CConcept *pTempConcept = expectation.pDialogAgent->C(expectation.sConceptName).EmptyClone();
        pTempConcept->Update(CU_ASSIGN_FROM_STRING, &sBindingString);
        CConcept &c = expectation.pDialogAgent->C(expectation.sConceptName);

        // first if the concept has an undergoing grounding request, remove it
        //if (c.IsUndergoingGrounding())
        //    pGroundingManager->RemoveConceptGroundingRequest(&c);
        c.Update(CU_UPDATE_WITH_CONCEPT, pTempConcept);
        delete pTempConcept;
        */
        CConcept& c = expectation.pDialogAgent->C(expectation.sConceptName);
        if ( c.GetName() != "NULL") {
            c.Clear();
            c.Update(CU_ASSIGN_FROM_STRING, &sBindingString);

            for(size_t i = 0; i < indices.size(); i++) {
                CConcept* pConcept = 
                    &(_agenda.celSystemExpectations[indices[i]].pDialogAgent->C(
                        _agenda.celSystemExpectations[indices[i]].sConceptName));
                if( !_agenda.celSystemExpectations[indices[i]].bDisabled ) {
                    if( pConcept == &c ) {
                        VLOG(DEBUG) << __FUNCTION__ << ", OnConceptUpdated On AgentName: " << _agenda.celSystemExpectations[indices[i]].pDialogAgent->GetName();
                        _agenda.celSystemExpectations[indices[i]].pDialogAgent->OnConceptUpdated();
                    }
                }
            }
        } else {
            VLOG(FATAL) << __FUNCTION__ << ", concept is NULL.";
        }
    } else {
        expectation.pDialogAgent->C(expectation.sConceptName).Update(CU_PARTIAL_FROM_STRING, &sBindingString);
    }
}

void CDMCoreAgent::PerformCustomConceptBinding(int index) {

}

//    processes non-understandings
void CDMCoreAgent::ProcessNonUnderstanding() {
    /*
    VLOG(DEBUG) << "Process Non-Understanding Phase initiated.";

    // sets meta information on the input, specifying it's a non-understanding
    VLOG(DEBUG) << "Non-understanding " << NON_UNDERSTANDING << " detected.";
    pInteractionEventManager->GetLastInput()->SetProperty("["NON_UNDERSTANDING"]", "true");

    VLOG(DEBUG) << "Process Non-Understanding Phase completed.";
    */
}

void CDMCoreAgent::PerformForcedConceptUpdates(TBindingsDescr& bindings) {
    VLOG(DEBUG) << __FUNCTION__;

    std::set<CConcept *>::iterator iPtr;
    for( iPtr = _system_action.setcpExplicitConfirms.begin(); 
            iPtr != _system_action.setcpExplicitConfirms.end(); 
            iPtr++ ) {
        if( (*iPtr)->IsSealed() ) {
            TForcedConceptUpdate fcu;
            fcu.sConceptName = (*iPtr)->GetName();
            fcu.iType = FCU_EXPLICIT_CONFIRM;
            fcu.bUnderstanding = false;
            CHyp *phOldTopHyp = (*iPtr)->GetTopHyp();
            ((void) phOldTopHyp);

            VLOG(DEBUG) << __FUNCTION__ << ", Performing forced concept update on " << (*iPtr)->GetName();
            (*iPtr)->Update(CU_UPDATE_WITH_CONCEPT, NULL);

            /*
            if(!((*iPtr)->IsSealed()) && 
                (pGroundingManager->GetConceptGroundingRequestStatus(*iPtr) != GRS_EXECUTING)) {
                // now schedule grounding on this concept
                //string sAction = pGroundingManager->ScheduleConceptGrounding(*iPtr);
                // if the action scheduled is still explicit confirm
                //if(sAction != "EXPL_CONF") {
                    // then mark that we have an understanding
                //    fcu.bUnderstanding = true;
                //    bindings.bNonUnderstanding = false;
                //} else if ((*iPtr)->GetTopHyp() == phOldTopHyp) {
                    // if we are still on an explicit confirm on the same hypothesis, 
                    // seal it back
                    (*iPtr)->Seal();
                //}
            }
            */
            bindings.vfcuForcedUpdates.push_back(fcu);
        }
    }

    for( iPtr = _system_action.setcpImplicitConfirms.begin(); 
            iPtr != _system_action.setcpImplicitConfirms.end(); 
            iPtr++ ) {
        if( (*iPtr)->IsSealed() ) {
            // then perform an forced update            
            TForcedConceptUpdate fcu;
            fcu.sConceptName = (*iPtr)->GetName();
            fcu.iType = FCU_IMPLICIT_CONFIRM;
            fcu.bUnderstanding = false;

            VLOG(DEBUG) << __FUNCTION__ << ", Performing forced concept update on " << (*iPtr)->GetName();
            (*iPtr)->Update(CU_UPDATE_WITH_CONCEPT, NULL);

            /*
            if(!((*iPtr)->IsSealed()) && 
                (pGroundingManager->GetConceptGroundingRequestStatus(*iPtr) != GRS_EXECUTING)) {
                // first check that it was not already scheduled
                if(pGroundingManager->GetScheduledGroundingActionOnConcept(*iPtr) == "")                    
                    // if not scheduled already, schedule it now                
                    pGroundingManager->ScheduleConceptGrounding(*iPtr);
                // then mark that we have an understanding
                fcu.bUnderstanding = true;
                bindings.bNonUnderstanding = false;
            }
            */
            bindings.vfcuForcedUpdates.push_back(fcu);
        }
    }

    // finally, go through the concepts that have unplanned implicit confirmations
    // on them and perform the corresponding updates on them
    for( iPtr = _system_action.setcpUnplannedImplicitConfirms.begin(); 
            iPtr != _system_action.setcpUnplannedImplicitConfirms.end(); 
            iPtr++ ) {
        if( (*iPtr)->IsSealed() ) {
            TForcedConceptUpdate fcu;
            fcu.sConceptName = (*iPtr)->GetName();
            fcu.iType = FCU_UNPLANNED_IMPLICIT_CONFIRM;
            fcu.bUnderstanding = false;

            VLOG(DEBUG) << __FUNCTION__ << "Performing forced concept update on " << (*iPtr)->GetName();
            (*iPtr)->Update(CU_UPDATE_WITH_CONCEPT, NULL);
            /*
            // now, if this update has desealed the concept, then we need
            // to run grounding on this concept 
            if(!((*iPtr)->IsSealed())) {
                // first check that it was not already scheduled
                if(pGroundingManager->GetScheduledGroundingActionOnConcept(*iPtr) == "")                    
                    // if not scheduled already, schedule it now                
                    pGroundingManager->ScheduleConceptGrounding(*iPtr);
                // then mark that we have an understanding
                fcu.bUnderstanding = true;
                bindings.bNonUnderstanding = false;
            }
            */
            // finally, push this into the bindings
            bindings.vfcuForcedUpdates.push_back(fcu);
        }
    }

    VLOG(DEBUG) << __FUNCTION__ << "Forced concept updates completed.";
}

int CDMCoreAgent::AssembleFocusClaims() {
    VLOG(DEBUG) << __FUNCTION__ <<  ", Focus Claims Assembly Phase initiated.";
    if ( _root == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _root is NULL, Name: " << _name;
        return 0;
    }

    
    if ( _registry_agent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _registry_agent is NULL, AgentName: " << _name;
        return 0;
    }
    
    std::vector<TFocusClaim> fclTempFocusClaims;
    int iClaims = _root->DeclareFocusClaims(fclTempFocusClaims);

    VLOG(DEBUG) << __FUNCTION__ << ", fclTempFocusClaims.size(): " << fclTempFocusClaims.size();
    for(size_t i = 0; i < fclTempFocusClaims.size(); i++) {
        VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << fclTempFocusClaims[i].sAgentName;
    }

    int iClaimsEliminated = 0;
    _focus_claims.clear();
    for(size_t i = 0; i < fclTempFocusClaims.size(); i++) {
        CDialogAgent* pdaFocusClaimingAgent =
            (CDialogAgent *)_registry_agent->operator[](fclTempFocusClaims[i].sAgentName);
        if(pdaFocusClaimingAgent->SuccessCriteriaSatisfied() || 
           pdaFocusClaimingAgent->FailureCriteriaSatisfied() /*||
           AgentIsActive(pdaFocusClaimingAgent) ||
           (!fclTempFocusClaims[i].bClaimDuringGrounding && bDuringGrounding)*/) {
            VLOG(DEBUG) << __FUNCTION__ << ", Can not trigger, " << fclTempFocusClaims[i].sAgentName << "\tSuccessCriteriaSatisfied: " <<
                pdaFocusClaimingAgent->SuccessCriteriaSatisfied() << "\tFailureCriteriaSatisfied: " << pdaFocusClaimingAgent->FailureCriteriaSatisfied();
            iClaimsEliminated++;
        } else {
            _focus_claims.push_back(fclTempFocusClaims[i]);
        }
    }
    VLOG(DEBUG) << __FUNCTION__ << ", Can not trigger num: " << iClaimsEliminated << ", can trigger num: " << _focus_claims.size();
    return iClaims;
}

void CDMCoreAgent::ResolveFocusShift() {    
    // send out a warning if we have a multiple focus shift
    if ( _registry_agent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _registry_agent is NULL, AgentName: " << _name;
        return ;
    }
    if(_focus_claims.size() > 1) {
        std::string sMessage = "Ambiguous focus shift (claiming agents dump below).\n";
        for(size_t i = 0; i < _focus_claims.size(); i++) 
            sMessage += _focus_claims[i].sAgentName + "\n";
        VLOG(WARNING) << sMessage;
    };

    // put the agents on the stack 
    for(size_t i = 0; i < _focus_claims.size(); i++) {
        std::string sClaimingAgent = _focus_claims[i].sAgentName;
        VLOG(DEBUG) << "Adding focus-claiming agent " << sClaimingAgent << " on the execution stack.";
        ContinueWith(this, (CDialogAgent *)_registry_agent->operator[](sClaimingAgent));
    }
}

void CDMCoreAgent::SetTimeoutPeriod(int value) {
    _timeout_period = value;
}

int CDMCoreAgent::GetTimeoutPeriod() {
    return _timeout_period;
}

void CDMCoreAgent::SetDefaultTimeoutPeriod(int value) {
    _default_timeout_period = value;
}

int CDMCoreAgent::GetDefaultTimeoutPeriod() {
    return _default_timeout_period;
}

void CDMCoreAgent::SetNonunderstandingThreshold(float value) {
    _nonunderstanding_threshold = value;
}

float CDMCoreAgent::GetNonunderstandingThreshold() {
    return _nonunderstanding_threshold;
}

void CDMCoreAgent::SetDefaultNonunderstandingThreshold(float value) {
    _default_nonunderstanding_threshold = value;
}

float CDMCoreAgent::GetDefaultNonunderstandingThreshold() {
    return _default_nonunderstanding_threshold;
}

void CDMCoreAgent::SetFloorStatus(TFloorStatus status) {
    _floor_status = status;
}

void CDMCoreAgent::SetFloorStatus(const std::string& status) {
    SetFloorStatus(StringToFloorStatus(status));
}

TFloorStatus CDMCoreAgent::GetFloorStatus() {
    return _floor_status;
}

std::string CDMCoreAgent::FloorStatusToString(TFloorStatus floor) {
    return s_floor_status_lables[floor];
}

TFloorStatus CDMCoreAgent::StringToFloorStatus(const std::string& floor) {   
    for (size_t i=0; i < s_floor_status_lables.size(); i++) {
        if ( s_floor_status_lables[i] == floor)
            return (TFloorStatus)i;
    }
    return fsUnknown;
}

void CDMCoreAgent::SignalFocusClaimsPhase(bool flag) {
    VLOG(DEBUG) << __FUNCTION__ << ", flag: " << flag;
    _is_focus_claims_phase_flag = flag;
}

int CDMCoreAgent::LastTurnGetConceptsBound() {
    if(_binding_history.size() == 0) 
        return -1;
    else 
        return _binding_history.back().iConceptsBound;
}

bool CDMCoreAgent::LastTurnNonUnderstanding() {
    for (int i = _binding_history.size()-1; i >= 0; i--) {
        if (_binding_history[i].sEventType == IET_USER_UTT_END ||
            _binding_history[i].sEventType == IET_GUI) {
            if (_binding_history[i].bNonUnderstanding)
                return true;
            else
                return false;
        }
    }
    return false;
}

int CDMCoreAgent::GetNumberNonUnderstandings() {
    int iNumNonunderstandings = 0;
    for (int i = _binding_history.size()-1; i >= 0; i--) {
        if (_binding_history[i].sEventType == IET_USER_UTT_END ||
            _binding_history[i].sEventType == IET_GUI) {
            if (_binding_history[i].bNonUnderstanding) {
                iNumNonunderstandings++;
            }
            else {
                break;
            }
        }
    }
    return iNumNonunderstandings;
}

int CDMCoreAgent::GetTotalNumberNonUnderstandings() {
    int iNumNonunderstandings = 0;
    for (int i = 0; i < (int)_binding_history.size(); i++) {
        if (_binding_history[i].bNonUnderstanding) {
            iNumNonunderstandings++;
        }
    }
    return iNumNonunderstandings;
}

void CDMCoreAgent::ContinueWith(CAgent* pusher, CDialogAgent* agent) {
    VLOG(DEBUG) << "ContinueWith, pusher: " <<  pusher->GetName() << "\tagent: " << agent->GetName();
    if ( agent->IsABusiness() ) {
        // TODO
        _is_trigger_new_business = false;
        ResetExecutionStack();
    }
    if ( !_execution_stack.empty() && (_execution_stack.begin()->pdaAgent == agent) ) {
        VLOG(WARNING) << "Not ContinueWith, ";
        return ;
    }
    TExecutionHistoryItem ehi;
    ehi.sCurrentAgent = agent->GetName();
    ehi.sCurrentAgentType = agent->GetType();
    ehi.bScheduled = true;
    ehi.sScheduledBy = pusher->GetName();
    ehi.timeScheduled = GetTime();
    ehi.bExecuted = false;
    ehi.bCommitted = false;
    ehi.bCanceled = false;
    ehi.iStateHistoryIndex = -1;
    _execution_history.push_back(ehi);

    TExecutionStackItem esi;
    esi.pdaAgent = agent;
    esi.iEHIndex = _execution_history.size() - 1;
    _execution_stack.push_front(esi);
    agent->SetLastExecutionIndex(esi.iEHIndex);
    agent->IncomingStack();
    _is_agenda_modified_flag = true;

    MODIFY_NEED_UPDATE_STATE(agent);
}

void CDMCoreAgent::RestartTopic(CDialogAgent* agent) {
    
    if ( _registry_agent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _registry_agent is NULL, AgentName: " << _name;
        return ;
    }
    TExecutionStack::iterator iter;
    for ( iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
        if ( iter->pdaAgent == agent ) { break; }
    }
    if ( iter == _execution_stack.end() ) {
        VLOG(WARNING) << "Cannot restart the " << agent->GetName() << " agent. Agent not found on execution stack.";
        return ;
    }
    CDialogAgent* scheduler = (CDialogAgent*)_registry_agent->operator[](_execution_history[iter->iEHIndex].sScheduledBy);
    PopTopicFromExecutionStack(agent);
    agent->ReOpen();
    ContinueWith(scheduler, agent);
}

void CDMCoreAgent::RegisterCustomStartOver(TCustomStartOverFunct funct) {
    _start_over_funct = funct;
}

void CDMCoreAgent::StartOver() {
    if ( _start_over_funct == NULL ) {
        _execution_stack.clear();
        DestroyDialogTree();
        CDTTManagerAgent::Instance()->ReCreateDialogTree( _root_name, this );
        ContinueWith(this, _root);
    } else {
        (*_start_over_funct)();
    }
}

void CDMCoreAgent::ResetExecutionStack() {
    VLOG(DATA) << __FUNCTION__ << ", _execution_stack.size(): " << _execution_stack.size();
    while ( _execution_stack.size() > 1 ) {
        PopTopicFromExecutionStack(_execution_stack.begin()->pdaAgent);
    }
}

int CDMCoreAgent::PopCompletedFromExecutionStack() {
    VLOG(DEBUG) << __FUNCTION__;
    bool is_found_complete = false;
    std::vector<std::string> agents_eliminated;
    do {
        is_found_complete = false;
        for ( TExecutionStack::iterator iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
            if ( iter->pdaAgent != NULL && iter->pdaAgent->HasCompleted() ) {
                PopTopicFromExecutionStack(iter->pdaAgent, agents_eliminated);
                is_found_complete = true;
                break;
            }
        }
    } while (is_found_complete);
    return agents_eliminated.size();
}

void CDMCoreAgent::PopAgentFromExecutionStack(CDialogAgent* agent, std::vector<std::string>& agents_eliminated) {
    TExecutionStack::iterator iter;
    for ( iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
        if ( iter->pdaAgent == agent ) { break; }
    }

    if ( iter == _execution_stack.end() ) {
        VLOG(WARNING) << "Cannot pop the " << agent->GetName() << " agent off the execution stack. Agent not found.";
        return ;
    }
    _execution_history[iter->iEHIndex].timeTerminated = GetTime();
    if ( iter->pdaAgent != NULL ) {
        iter->pdaAgent->OnCompletion();
        agents_eliminated.push_back(iter->pdaAgent->GetName());
    }

    MODIFY_NEED_UPDATE_STATE(iter->pdaAgent);
    _execution_stack.erase(iter);
    _is_agenda_modified_flag = true;
}

void CDMCoreAgent::PopTopicFromExecutionStack(CDialogAgent* agent, std::vector<std::string>& agents_eliminated) {
    VLOG(DEBUG) << "private:PopTopicFromExecutionStack, agent: " << agent->GetName(); 
    
    if ( _registry_agent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _registry_agent is NULL, AgentName: " << _name;
        return ;
    }
    TExecutionStack::iterator iter;
    for ( iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
        if ( iter->pdaAgent == agent ) { break; }
    }

    if ( iter == _execution_stack.end() ) {
        VLOG(WARNING) << "Cannot pop the " << agent->GetName() << " agent off the execution stack. agent not found.";
        return ;
    }
    std::set<CDialogAgent*> agents;
    agents.insert(iter->pdaAgent);
    _execution_history[iter->iEHIndex].timeTerminated = GetTime();
    if ( iter->pdaAgent != NULL ) {
        iter->pdaAgent->OnCompletion();
       agents_eliminated.push_back(iter->pdaAgent->GetName());
    }

    MODIFY_NEED_UPDATE_STATE(iter->pdaAgent);
    _execution_stack.erase(iter);
    bool is_found_agent_to_remove = true;
    while ( is_found_agent_to_remove ) {
        is_found_agent_to_remove = false;
        for (iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
            CDialogAgent* scheduler = (CDialogAgent*)_registry_agent->operator[](_execution_history[iter->iEHIndex].sScheduledBy);
            if ( agents.find(scheduler) != agents.end() ) {
                agents.insert(iter->pdaAgent);
                _execution_history[iter->iEHIndex].timeTerminated = GetTime();
                if ( iter->pdaAgent != NULL ) {
                    iter->pdaAgent->OnCompletion();
                    agents_eliminated.push_back(iter->pdaAgent->GetName());
                }
                MODIFY_NEED_UPDATE_STATE(iter->pdaAgent);
                _execution_stack.erase(iter);
                is_found_agent_to_remove = true;
                break;
            }
        }
    }
    _is_agenda_modified_flag = true;
}

void CDMCoreAgent::PopGroundingAgentsFromExecutionStack(std::vector<std::string>& agents_eliminated) {
    if ( _registry_agent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", _registry_agent is NULL, AgentName: " << _name;
        return ;
    }
    std::set<CAgent*> agents;
    agents.insert(CGroundingManagerAgent::Instance());
    bool is_found_agent_to_remove = true;

    while ( is_found_agent_to_remove ) {
        is_found_agent_to_remove = false;

        for ( TExecutionStack::iterator iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
            CDialogAgent* scheduler = (CDialogAgent*)_registry_agent->operator[](_execution_history[iter->iEHIndex].sScheduledBy);
            if ( agents.find(scheduler) != agents.end() ) {
                agents.insert(iter->pdaAgent);
                _execution_history[iter->iEHIndex].timeTerminated = GetTime();
                if ( iter->pdaAgent != NULL ) {
                    iter->pdaAgent->OnCompletion();
                    agents_eliminated.push_back(iter->pdaAgent->GetName());
                }

                MODIFY_NEED_UPDATE_STATE(iter->pdaAgent);
                _execution_stack.erase(iter);
                is_found_agent_to_remove = true;
                break;
            }
        }
    }
    _is_agenda_modified_flag = true;
}

void CDMCoreAgent::RollBackDialogState(int state) {
    VLOG(DEBUG) << __FUNCTION__ << ", state: " << state;
    int index = (*CStateManagerAgent::Instance())[state].iEHIndex;
    for (int i = _execution_history.size()-1;i > index; --i ) {
        TExecutionHistoryItem ehi = _execution_history[i];
        if ( ehi.bExecuted && !ehi.bCanceled && _registry_agent != NULL ) {
            CDialogAgent* agent = (CDialogAgent*)_registry_agent->operator[](ehi.sCurrentAgent);
            agent->Undo();
            ehi.bCanceled = true;
        }
    }
    TDialogState current_state = CStateManagerAgent::Instance()->operator[](state);
    _floor_status = current_state.fsFloorStatus;
    _execution_stack = current_state.esExecutionStack;
    _agenda = current_state.eaAgenda;
    _system_action = current_state.saSystemAction;
    _is_agenda_modified_flag = false;
}

bool CDMCoreAgent::DestroyDialogTree() {
    if ( CDTTManagerAgent::Instance() != NULL ) {
        CDTTManagerAgent::Instance()->DestroyDialogTree(_root_name);
        _root = NULL;
        return true;
    }
    return false;
}

int CDMCoreAgent::GetBindingHistorySize() {
    return _binding_history.size();
}

const TBindingsDescr& CDMCoreAgent::GetBindingResult(int index) {
    if ( index >= 0 || index < -(int)_binding_history.size() ) {
        VLOG(WARNING) << "Out of bounds index on access to binding history.";
        return _binding_history[0];
    } else {
        return _binding_history[_binding_history.size()+index];
    }
}

TSystemActionOnConcept CDMCoreAgent::GetSystemActionOnConcept(CConcept* concept) {
    TSystemActionOnConcept saoc;

    if ( _system_action.setcpRequests.find(concept) != _system_action.setcpRequests.end() ) {
        saoc.sSystemAction = SA_REQUEST;
    } else if ( _system_action.setcpExplicitConfirms.find(concept) != _system_action.setcpExplicitConfirms.end() ) {
        saoc.sSystemAction = SA_EXPL_CONF;
    } else if ( _system_action.setcpImplicitConfirms.find(concept) !=  _system_action.setcpImplicitConfirms.end() ) {
        saoc.sSystemAction = SA_IMPL_CONF;
    } else if ( _system_action.setcpUnplannedImplicitConfirms.find(concept) != _system_action.setcpUnplannedImplicitConfirms.end() ) {
        saoc.sSystemAction = SA_UNPLANNED_IMPL_CONF;
    } else {
        saoc.sSystemAction = SA_OTHER;
    }
    return saoc;
}

void CDMCoreAgent::SignalExplicitConfirmOnConcept(CConcept* concept) {

    if ( _system_action.setcpUnplannedImplicitConfirms.find(concept) != 
            _system_action.setcpUnplannedImplicitConfirms.end() ) {
        _system_action.setcpUnplannedImplicitConfirms.erase(concept);
    }
    _system_action.setcpExplicitConfirms.insert(concept);
}

void CDMCoreAgent::SignalImplicitConfirmOnConcept(CConcept* concept) {
    if ( _system_action.setcpUnplannedImplicitConfirms.find(concept) != _system_action.setcpUnplannedImplicitConfirms.end() ) {
        _system_action.setcpUnplannedImplicitConfirms.erase(concept);
    }
    _system_action.setcpImplicitConfirms.insert(concept);
}

void CDMCoreAgent::SignalUnplannedImplicitConfirmOnConcept(int state, CConcept* concept) {
    VLOG(DEBUG) << __FUNCTION__ << ", state: " << state;
	if ( state >= 0 ) {
		if (((*CStateManagerAgent::Instance())[state].saSystemAction.setcpImplicitConfirms.find(concept) == 
			 (*CStateManagerAgent::Instance())[state].saSystemAction.setcpImplicitConfirms.end()) &&
			((*CStateManagerAgent::Instance())[state].saSystemAction.setcpExplicitConfirms.find(concept) == 
			 (*CStateManagerAgent::Instance())[state].saSystemAction.setcpExplicitConfirms.end())) {
			(*CStateManagerAgent::Instance())[state].saSystemAction.setcpUnplannedImplicitConfirms.insert(concept);
		}
	} else {
		if ((_system_action.setcpImplicitConfirms.find(concept) == 
			 _system_action.setcpImplicitConfirms.end()) &&
			(_system_action.setcpExplicitConfirms.find(concept) == 
			 _system_action.setcpExplicitConfirms.end())) {
			_system_action.setcpUnplannedImplicitConfirms.insert(concept);
		}
	}

}

void CDMCoreAgent::SignalAcceptOnConcept(CConcept* concept) {
    _system_action.setcpExplicitConfirms.erase(concept);
}

CDialogAgent* CDMCoreAgent::GetAgentInFocus() {
    // TODO 是否应该做容错
    if ( _execution_stack.empty() ) {
        VLOG(FATAL) << __FUNCTION__ << "_execution_stack is empty.";
    }
    for ( TExecutionStack::iterator iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
        if ( iter->pdaAgent != NULL ) {
            if ( iter->pdaAgent->IsExecutable() ) {
                return iter->pdaAgent;
            } else {
                VLOG(WARNING) << "Can not Execute, agent: " << iter->pdaAgent->GetName();
            }
        }
    }
    VLOG(FATAL) << __FUNCTION__ << ", Not Executable Agent, _execution_stack.size(): " << _execution_stack.size();
    return NULL;
}

CDialogAgent* CDMCoreAgent::GetDTSAgentInFocus() {
    for ( TExecutionStack::iterator iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
        if ( iter->pdaAgent != NULL && iter->pdaAgent->IsDTSAgent() && iter->pdaAgent->IsExecutable() ) {
            return iter->pdaAgent;
        }
    }
    return NULL;
}

bool CDMCoreAgent::AgentIsInFocus(CDialogAgent* agent) {
    if ( !agent->IsExecutable() ) {
        return false;
    }

    if ( !agent->IsDTSAgent() ) {
        return GetAgentInFocus() == agent;
    } else {
        return GetDTSAgentInFocus() == agent;
    }
}

CDialogAgent* CDMCoreAgent::GetAgentPreviouslyInFocus(CDialogAgent* agent) {
    TExecutionStack::iterator iter = _execution_stack.begin();
    if ( agent == NULL ) {
        agent = GetAgentInFocus();
    }
    while( iter != _execution_stack.end() && iter->pdaAgent != agent) {
        iter++;
    }
    if ( iter == _execution_stack.end() ) {
        return NULL;
    }
    iter++;
    while (iter != _execution_stack.end() && (iter->pdaAgent != NULL) && !iter->pdaAgent->IsExecutable() ) {
        iter++;
    }
    if ( iter == _execution_stack.end() ) {
        return NULL;
    } else {
        return iter->pdaAgent;
    }
}

CDialogAgent* CDMCoreAgent::GetDTSAgentPreviouslyInFocus(CDialogAgent* agent) {
    TExecutionStack::iterator iter = _execution_stack.begin();
    if ( agent == NULL ) {
        agent = GetAgentInFocus();
    }
    while ( iter != _execution_stack.end() && iter->pdaAgent != agent) { iter++; }
    if ( iter == _execution_stack.end() ) {
        return NULL;
    }
    while ( iter != _execution_stack.end() && (iter->pdaAgent != NULL) && !iter->pdaAgent->IsDTSAgent() ) { iter++; }
    if ( iter == _execution_stack.end() ) {
        return NULL;
    } else {
        return iter->pdaAgent;
    }
}

CDialogAgent* CDMCoreAgent::GetCurrentMainTopicAgent() {
    for ( TExecutionStack::iterator iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
        if ( iter->pdaAgent != NULL && iter->pdaAgent->IsAMainTopic() ) {
            return iter->pdaAgent;
        }
    }
    return NULL;
}

CDialogAgent* CDMCoreAgent::GetCurrentBusinessAgent() {
    for ( TExecutionStack::iterator iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
        if ( iter->pdaAgent != NULL && iter->pdaAgent->IsABusiness() ) {
            return iter->pdaAgent;
        }
    }
    return NULL;
}

bool CDMCoreAgent::AgentIsActive(CDialogAgent* agent) {
    for ( TExecutionStack::iterator iter = _execution_stack.begin();iter != _execution_stack.end(); ++iter ) {
        if ( iter->pdaAgent == agent ) {
            return true;
        }
    }
    return false;
}

void CDMCoreAgent::PopAgentFromExecutionStack(CDialogAgent* agent) {
    std::vector<std::string> agents_eliminated;
    PopAgentFromExecutionStack(agent, agents_eliminated);
}

void CDMCoreAgent::PopTopicFromExecutionStack(CDialogAgent* agent) {
    std::vector<std::string> agents_eliminated;
    PopTopicFromExecutionStack(agent, agents_eliminated);
}

void CDMCoreAgent::PopGroundingAgentsFromExecutionStack() {
    std::vector<std::string> agents_eliminated;
    PopGroundingAgentsFromExecutionStack(agents_eliminated);
}

int CDMCoreAgent::GetLastInputTurnNumber() {
    VLOG(DEBUG) << __FUNCTION__ << ", turn_number: " << _turn_number;
    return _turn_number;
}

}
