#include "StateManagerAgent.h"


namespace TrioTDM {

CStateManagerAgent* CStateManagerAgent::s_instance = NULL;

CStateManagerAgent::CStateManagerAgent(const std::string& name, 
									   const std::string& configuration,
									   const std::string& type):
	CAgent(name, configuration, type) {
}

CStateManagerAgent::~CStateManagerAgent() {

}

CAgent* CStateManagerAgent::AgentFactory(const std::string& sAName, const std::string& sAConfiguration) {
	return new CStateManagerAgent(sAName, sAConfiguration);
}

CStateManagerAgent* CStateManagerAgent::Instance() {
    if ( s_instance == NULL ) {
        s_instance = new CStateManagerAgent("StateManagerAgent");
    }
    return s_instance;
}


void CStateManagerAgent::Reset() {
	_state_history.clear();
}

void CStateManagerAgent::LoadDialogStateNames(const std::string& sFileName) {
}

void CStateManagerAgent::BroadcastState() {

}

void CStateManagerAgent::UpdateState(CDMCoreAgent* core_agent) {
    VLOG(DEBUG) << __FUNCTION__;
    if ( core_agent == NULL ) {
        VLOG(FATAL) << "core_agent is NULL.";
        return ;
    }
    if ( core_agent->_is_agenda_modified_flag) {
        VLOG(DATA) << "CStateManagerAgent call AssembleExpectationAgenda.";
        core_agent->AssembleExpectationAgenda();
    }
    /*
    // construct the dialog state
    TDialogState dsDialogState;
	dsDialogState.fsFloorStatus = pDMCore->fsFloorStatus;
    dsDialogState.sFocusedAgentName = pDMCore->GetAgentInFocus()->GetName();
    dsDialogState.esExecutionStack = pDMCore->esExecutionStack;
    dsDialogState.eaAgenda = pDMCore->eaAgenda;
	dsDialogState.saSystemAction = pDMCore->saSystemAction;
    dsDialogState.iTurnNumber = pDMCore->iTurnNumber;
	dsDialogState.iEHIndex = pDMCore->esExecutionStack.front().iEHIndex;

	// compute the dialog state 
	dsDialogState.sStateName = "";
	if(s2sDialogStateNames.empty()) { 
		dsDialogState.sStateName = dsDialogState.sFocusedAgentName;
	} else {
		// go through the mapping and find something that matches the focus
		STRING2STRING::iterator iPtr;
		for(iPtr = s2sDialogStateNames.begin(); 
			iPtr != s2sDialogStateNames.end();
			iPtr++) {
			if(dsDialogState.sFocusedAgentName.find(iPtr->first) != -1) {
				dsDialogState.sStateName = iPtr->second;
				break;
			}
		}	
		// if we couldn't find anything in the mapping, then set it to 
		// _unknown_
		if(dsDialogState.sStateName == "") 
			dsDialogState.sStateName = "_unknown_";
	}

	// adds the input line configuration as part of the state
	STRING2STRING s2sInputLineConfiguration = 
		pDMCore->GetAgentInFocus()->GetInputLineConfiguration();
	dsDialogState.sInputLineConfiguration = 
	    S2SHashToString(s2sInputLineConfiguration);

    // and push the state in history
    vStateHistory.push_back(dsDialogState);

	TDialogState ds = GetLastState();

    // log the finish
	Log(STATEMANAGER_STREAM, "Dialog state update completed: %s at %d "
		"(iEHIndex=%d):\n%s",
		dsDialogState.sFocusedAgentName.c_str(), vStateHistory.size()-1,
		dsDialogState.iEHIndex, GetStateAsString().c_str());
    */
}

std::string CStateManagerAgent::GetStateAsString(TDialogState dsState) {
    return "";
}

std::string CStateManagerAgent::GetStateAsString() {
	return GetStateAsString(_state_history.back());
}

int CStateManagerAgent::GetStateHistoryLength() {
	return _state_history.size();
}

TDialogState &CStateManagerAgent::GetLastState() {
    return _state_history.back();
}

TDialogState &CStateManagerAgent::operator[](size_t i) {
	return _state_history[i];
}

}
