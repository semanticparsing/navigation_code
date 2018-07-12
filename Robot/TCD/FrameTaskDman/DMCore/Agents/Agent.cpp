#include "Agent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/CoreAgents/DMCoreAgent.h"

namespace TrioTDM {

CAgent::CAgent(const std::string& name, const std::string& configuration, const std::string& type) : 
    _name(name),
    _type(type) {
        _owner_core_agent = NULL;
        SetConfiguration(configuration);
}

CAgent::~CAgent() {
    if ( _name != "NULL" ) {
        UnRegister();
    }
}

CAgent* CAgent::AgentFactory(const std::string& name, const std::string& configuration) {
    VLOG(WARNING) << "AgentFactory called on Cagent (abstract) class.";
    return NULL;
}

void CAgent::Create() {
}

void CAgent::Initialize() {
}

void CAgent::Reset() {
}

void CAgent::SetOwnerCoreAgent(CDMCoreAgent* core_agent) {
    _owner_core_agent = core_agent;
}

void CAgent::SetConfiguration(const std::string& configuration) {
    std::map<std::string, std::string> lval = StringToS2SHash(configuration);
    AppendToS2S(_configuration, lval);
}

void CAgent::SetConfiguration(const std::map<std::string, std::string>& configuration) {
    AppendToS2S(_configuration, configuration);
}

void CAgent::SetParameter(const std::string& param, const std::string& value) {
    _configuration.insert(std::make_pair(param, value));
}

bool CAgent::HasParameter(const std::string& param) {
    return (_configuration.find(param) != _configuration.end());
}

std::string CAgent::GetParameterValue(const std::string& param) {
    std::map<std::string, std::string>::iterator f_iter = _configuration.find(param);
    if ( f_iter == _configuration.end() ) {
        return "";
    } else {
        return (string)(f_iter->second);
    }
}

std::string CAgent::GetName() {
    return _name;
}

std::string CAgent::GetType() {
    return _type;
}

void CAgent::Register() {
    if ( _owner_core_agent != NULL && _owner_core_agent->GetRegistryAgent() != NULL ) {
        _owner_core_agent->GetRegistryAgent()->RegisterAgent(_name, this);
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", Fail, _owner_core_agent is NULL: " << (_owner_core_agent == NULL) << ", AgentName: " << _name;
    }
}

void CAgent::UnRegister() {
    if ( _owner_core_agent != NULL && _owner_core_agent->GetRegistryAgent() != NULL ) {
        _owner_core_agent->GetRegistryAgent()->UnRegisterAgent(_name);
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", Fail, _owner_core_agent is NULL: " << (_owner_core_agent == NULL);
    }
}

}
