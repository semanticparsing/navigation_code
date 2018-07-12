#include "InteractionEventManagerAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

namespace TrioTDM {

CInteractionEventManagerAgent* CInteractionEventManagerAgent::s_instance = NULL;

CInteractionEventManagerAgent::CInteractionEventManagerAgent(const std::string& name,
									   const std::string& configuration, 
									   const std::string& type) : 
	CAgent(name, configuration, type) {
}

CInteractionEventManagerAgent::~CInteractionEventManagerAgent() {
}

CAgent* CInteractionEventManagerAgent::AgentFactory(const std::string& name, 
													const std::string& configuration) {
	return new CInteractionEventManagerAgent(name, configuration);
}

CInteractionEventManagerAgent* CInteractionEventManagerAgent::Instance() {
    if ( s_instance == NULL && CRegistryAgentType::Instance() != NULL ) {
        s_instance = (CInteractionEventManagerAgent*)CRegistryAgentType::Instance()->CreateAgent("CInteractionEventManagerAgent", "InteractionEventManagerAgent");
    }
    return s_instance;
}

void CInteractionEventManagerAgent::Reset() {
}

void CInteractionEventManagerAgent::Initialize() {
	pieLastEvent = NULL;
	pieLastInput = NULL;
}

bool CInteractionEventManagerAgent::HasEvent() {
	return !qpieEventQueue.empty();
}

CInteractionEvent *CInteractionEventManagerAgent::GetNextEvent() {
	if ( !qpieEventQueue.empty() ) {
		CInteractionEvent *pieNext = qpieEventQueue.front();
		qpieEventQueue.pop();
		pieLastEvent = pieNext;
		if (pieNext->GetType() == IET_USER_UTT_END) {
			pieLastInput = pieNext;
		}
		vpieEventHistory.push_back(pieNext);
		return pieNext;
	} else {
		return NULL;
	}
}

CInteractionEvent *CInteractionEventManagerAgent::GetLastEvent() {
	return pieLastEvent;
}

CInteractionEvent *CInteractionEventManagerAgent::GetLastInput() {
	return pieLastInput;
}
	
bool CInteractionEventManagerAgent::LastEventMatches(const std::string& sGrammarExpectation) {
	// delegate it to the InteractionEvent class
	return pieLastEvent->Matches(sGrammarExpectation);
}

bool CInteractionEventManagerAgent::LastInputMatches(const std::string& sGrammarExpectation) {
	// delegate it to the InteractionEvent class
	return pieLastInput->Matches(sGrammarExpectation);
}

bool CInteractionEventManagerAgent::LastEventIsComplete() {
	return pieLastEvent->IsComplete();
}

float CInteractionEventManagerAgent::GetLastEventConfidence() {
    // delegate it to the Input class
    return pieLastEvent->GetConfidence();
}

string CInteractionEventManagerAgent::GetValueForExpectation(const string& sGrammarExpectation) {
	// delegate it to the Input class
	return pieLastEvent->GetValueForExpectation(sGrammarExpectation);
}

void CInteractionEventManagerAgent::WaitForEvent() {

}

void CInteractionEventManagerAgent::SignalInteractionEventArrived() {
}

}

