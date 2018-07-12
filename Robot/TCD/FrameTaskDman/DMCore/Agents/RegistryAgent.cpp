#include "RegistryAgent.h"
#include "Agent.h"

namespace TrioTDM {

CRegistryAgent::CRegistryAgent() {
    ;
}

CRegistryAgent::~CRegistryAgent() {
    ;
}

void CRegistryAgent::Clear() {
    _agents_hash.clear();
}

CAgent* CRegistryAgent::operator[](const std::string& agent_name) const {
    std::map<std::string, CAgent*>::const_iterator c_iter = _agents_hash.find( agent_name );

    if ( c_iter == _agents_hash.end() ) {
        VLOG(WARNING) << __FUNCTION__ << ", Not Agent, AgentName: " << agent_name;
        return NULL;
    } else {
        return c_iter->second;
    }
}

bool CRegistryAgent::RegisterAgent(const std::string& agent_name, CAgent* agent) {
    VLOG(DEBUG) << __FUNCTION__ << ", AgentName: " << agent_name;
    std::pair<std::map<std::string, CAgent*>::iterator, bool> ret = 
        _agents_hash.insert( std::make_pair(agent_name, agent) );

    if ( !ret.second ) {
        VLOG(WARNING) << __FUNCTION__ << ", AgentName: " << agent_name;
        return false;
    }
    return true;
}

bool CRegistryAgent::UnRegisterAgent(const std::string& agent_name) {
    if ( _agents_hash.erase(agent_name) ) {
        return true;
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", Erase Fail, AgentName: " << agent_name;
        return false;
    }
}

bool CRegistryAgent::IsRegisteredAgent(const std::string& agent_name) const {
    if ( _agents_hash.find(agent_name) != _agents_hash.end() ) {
        return true;
    } else {
        return false;
    }
}

}
