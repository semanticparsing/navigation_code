#include "Registry.h"
#include "Agent.h"

namespace TrioTDM {

CRegistry* CRegistry::s_instance = NULL;

CRegistry::CRegistry() {
    ;
}

CRegistry* CRegistry::Instance() {
    if ( s_instance == NULL ) {
        s_instance = new CRegistry();
    }
    return s_instance;
}

void CRegistry::Clear() {
    boost::recursive_mutex::scoped_lock lock(_mutex);

    VLOG(DEBUG) << __FUNCTION__;

    while ( !_agents_hash.empty() ) {
        delete _agents_hash.begin()->second;
    }
    _agents_hash.clear();
    _agents_type_hash.clear();
}

bool CRegistry::IsRegisteredAgent(const std::string& agent_name) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    return (_agents_hash.find(agent_name) != _agents_hash.end());
}

void CRegistry::RegisterAgent(const std::string& agent_name, CAgent* pagent) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << agent_name;

    if ( IsRegisteredAgent(agent_name) ) {
        VLOG(FATAL) << __FUNCTION__ << ", Agent is exist, AgentName: " << agent_name;
        return ;
    }
    _agents_hash.insert(std::make_pair(agent_name, pagent));
}

void CRegistry::UnRegisterAgent(const std::string& agent_name) {
    boost::recursive_mutex::scoped_lock lock(_mutex);

    if ( _agents_hash.erase(agent_name) == 0 ) {
        VLOG(FATAL) << __FUNCTION__ << ", Could not find agent, AgentName: " << agent_name;
        return ;
    }
}

CAgent* CRegistry::operator [](const std::string& agent_name) {
    boost::recursive_mutex::scoped_lock lock(_mutex);

    std::map<std::string, CAgent*>::iterator f_iter = _agents_hash.find(agent_name);
    if ( f_iter == _agents_hash.end() ) {
        VLOG(WARNING) << __FUNCTION__ << ", Can not find agent, AgentName: " << agent_name; 
        return NULL;
    } else {
        return f_iter->second;
    }
}

bool CRegistry::IsRegisteredAgentType(const std::string& agent_type_name) {
    return (_agents_type_hash.find(agent_type_name) != _agents_type_hash.end() );
}

void CRegistry::RegisterAgentType(const std::string& agent_type_name, FCreateAgent fct_create_agent) {
    boost::recursive_mutex::scoped_lock lock(_mutex);

    if ( IsRegisteredAgentType(agent_type_name) ) {
        VLOG(FATAL) << __FUNCTION__ << ", AgentTypeName is exist, " << agent_type_name;
        return ;
    }
    _agents_type_hash.insert(std::make_pair(agent_type_name, fct_create_agent));

    VLOG(DATA) << "Agent type " << agent_type_name << " registered successfully.";
}

void CRegistry::UnRegisterAgentType(const std::string& agent_type_name) {
    boost::recursive_mutex::scoped_lock lock(_mutex);

    if ( _agents_type_hash.erase(agent_type_name) == 0 ) {
        VLOG(FATAL) << __FUNCTION__ << ", Could not find agent type, AgentType: " << agent_type_name;
        return ;
    }
    VLOG(DATA) << __FUNCTION__ << ", AgentType: " << agent_type_name;
}

CAgent* CRegistry::CreateAgent(const std::string& agent_type_name, const std::string& agent_name, const std::string& agent_configuration) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentType: " << agent_type_name << ", AgentName: " << agent_name;
    std::map<std::string, FCreateAgent>::iterator f_iter = _agents_type_hash.find(agent_type_name);
    if ( f_iter == _agents_type_hash.end() ) {
        VLOG(WARNING) << __FUNCTION__ << "Not AgentType: " << agent_type_name;
        return NULL;
    } else {
        CAgent* pnew_agent = (*(f_iter->second))(agent_name, agent_configuration);
        if ( pnew_agent != NULL ) {
            pnew_agent->Create();
            return pnew_agent;
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", Errorcreating AgentType: " << agent_type_name;
            return NULL;
        }
    }
}

}
