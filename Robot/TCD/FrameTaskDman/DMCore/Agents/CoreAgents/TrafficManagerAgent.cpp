#include "TrafficManagerAgent.h"

namespace TrioTDM {

CTrafficManagerAgent* CTrafficManagerAgent::s_instance = NULL;

CTrafficManagerAgent::CTrafficManagerAgent(const std::string& name, const std::string& configuration, const std::string& type) :
    CAgent(name, configuration, type) {
}

CTrafficManagerAgent::~CTrafficManagerAgent() {
}

CAgent* CTrafficManagerAgent::AgentFactory(const std::string&  name, const std::string& configuration) {
    return new CTrafficManagerAgent(name, configuration);
}

CTrafficManagerAgent* CTrafficManagerAgent::Instance() {
    if ( s_instance == NULL && CRegistryAgentType::Instance() != NULL ) {
        s_instance = (CTrafficManagerAgent*)CRegistryAgentType::Instance()->CreateAgent("CTrafficManagerAgent" ,"TrafficManagerAgent");
    }
    return s_instance;
}

void CTrafficManagerAgent::Call(CDialogAgent* agent, const std::string& call) {
}

void CTrafficManagerAgent::GalaxyCall(const TExternalCallSpec& call) {
}


void CTrafficManagerAgent::OaaCall(const TExternalCallSpec& call) {
}


}
