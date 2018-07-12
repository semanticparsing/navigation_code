#include "RegistryAgentType.h"
#include "Agent.h"

namespace TrioTDM {

CRegistryAgentType* CRegistryAgentType::s_instance = NULL;

CRegistryAgentType::CRegistryAgentType() {
    ;
}

CRegistryAgentType::~CRegistryAgentType() {
    ;
}

CRegistryAgentType* CRegistryAgentType::Instance() {
    if ( s_instance == NULL ) {
        s_instance = new CRegistryAgentType();
    }
    return s_instance;
}

void CRegistryAgentType::Clear() {
    _agents_type_hash.clear();
}

bool CRegistryAgentType::RegisterAgentType(const std::string& agent_type_name, FCreateAgent fct_create_agent) {
    std::pair<std::map<std::string, FCreateAgent>::iterator, bool > ret = 
        _agents_type_hash.insert( std::make_pair(agent_type_name, fct_create_agent) );

    if ( !ret.second ) {
        VLOG(WARNING) << __FUNCTION__ << ", Insert Fail, AgentTypeName: " << agent_type_name;
        return false;
    }
    return true;
}

bool CRegistryAgentType::UnRegisterAgentType(const std::string& agent_type_name) {
    if ( !_agents_type_hash.erase( agent_type_name ) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Erase Fail, AgentTypeName: " << agent_type_name;
        return false;
    }
    return true;
}

bool CRegistryAgentType::IsRegisteredAgentType(const std::string& agent_type_name) const {
    if ( _agents_type_hash.find( agent_type_name ) != _agents_type_hash.end() ) {
        return true;
    }
    return false;
}

CAgent* CRegistryAgentType::CreateAgent(const std::string& agent_type_name, const std::string& agent_name, 
        const std::string& agent_configuration) const {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentTypeName: " << agent_type_name << ", AgentName: " << agent_name;
    std::map<std::string, FCreateAgent>::const_iterator c_iter = _agents_type_hash.find( agent_type_name );

    if ( c_iter == _agents_type_hash.end() ) {
        VLOG(WARNING) << __FUNCTION__ << ", Not Find FCreateAgent, AgentTypeName: " << agent_type_name;
        return NULL;
    } else {
        CAgent* agent = (c_iter->second)(agent_name, agent_configuration);
        if ( agent != NULL ) {
            agent->Create();
            return agent;
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", CarateAgent agent is NULL, AgentTypeName: " << agent_type_name;
            return NULL;
        }
    }
}

}
