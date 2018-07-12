
#include "StateManagerAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Registry.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

CDialogState::CDialogState() {
    sFocusedAgentName = "";
    sInputLineConfiguration = "";
    iTurnNumber = 0;
    iEHIndex = 0;
    sStateName = "";
}

CDialogState::~CDialogState() {

}

bool CDialogState::Load(const Json::Value& json) {
    if ( !json.isObject() || json.isNull()  ) {
        return false;
    }
    
    if ( ! fsFloorStatus.Load(json["FloorStatus"]) ) {
        VLOG(FATAL) << "Fail to load FloorStatus!";
        return false;
    }
    
    sFocusedAgentName = json["FocusedAgentName"].asString();
    if ( sFocusedAgentName.empty() ) {
        VLOG(DEBUG) << "FocusedAgentName is empty!";
    }
    
    if ( ! esExecutionStack.Load(json["ExecutionStack"]) ) {
        VLOG(FATAL) << "Fail to load ExecutionStack!";
        return false;
    }
    
    if ( ! eaAgenda.Load(json["Agenda"]) ) {
        VLOG(FATAL) << "Fail to load ExpectationAgenda!";
        return false;
    }
    
    if ( ! saSystemAction.Load(json["SystemAction"]) ) {
        VLOG(FATAL) << "Fail to load SystemAction!";
        return false;
    }

    sInputLineConfiguration = json["InputLineConfiguration"].asString();
    
    if ( !json["TurnNumber"].isInt() ) {
         VLOG(FATAL) << "Fail to load TurnNumber!";
        return false;
    }
    iTurnNumber = json["TurnNumber"].asInt();
    
    if ( !json["EHIndex"].isInt() ) {
         VLOG(FATAL) << "Fail to load EHIndex!";
        return false;
    }
    iEHIndex = json["EHIndex"].asInt();
    
    sStateName = json["StateName"].asString();
    
    return true;
    
}

Json::Value CDialogState::toJson() const {
    Json::Value json(Json::objectValue);
    json["FloorStatus"] = fsFloorStatus.toJson();
    json["FocusedAgentName"] = Json::Value(sFocusedAgentName);
    json["ExecutionStack"] = esExecutionStack.toJson();
    json["Agenda"] = eaAgenda.toJson();
    json["SystemAction"] = saSystemAction.toJson();
    json["InputLineConfiguration"] = Json::Value(sInputLineConfiguration);
    json["TurnNumber"] = Json::Value(iTurnNumber);
    json["EHIndex"] = Json::Value(iEHIndex);
    json["StateName"] = Json::Value(sStateName);
}

//-----------------------------------------------------------------------------
// Constructors and Destructors
//-----------------------------------------------------------------------------

CStateManagerAgent::CStateManagerAgent() :
pDMCore(NULL) {
   
}

//CStateManagerAgent::CStateManagerAgent(CDMCoreAgent * pDMCore) :
//pDMCore(pDMCore) {
   
//}

// Virtual destructor - does nothing at this point
CStateManagerAgent::~CStateManagerAgent() {

}

//-----------------------------------------------------------------------------
// CAgent class overwritten methods
//-----------------------------------------------------------------------------

// D: the overwritten Reset method
void CStateManagerAgent::Reset() {
    vStateHistory.clear();
}

void CStateManagerAgent::Set(CDMCoreAgent * myDMCore) {
    pDMCore = myDMCore;
}

//-----------------------------------------------------------------------------
//
// StateManagerAgent specific methods
//
//-----------------------------------------------------------------------------

// D: Loads the dialog state names from a file
/*
void CStateManagerAgent::LoadDialogStateNames(string sFileName) {
    // check if the filename is empty
    if(sFileName == "") return;

    // now, if the filename exists, try opening and reading from it
    // open the file
    FILE *fid;
    fopen_s(&fid, sFileName.c_str(), "r");
    if(!fid) {
        //Warning(FormatString(
        //    "Dialog states specification file (%s) could not be open for"\
        //    " reading.", sFileName.c_str()));
        VLOG(WARNING) << "Dialog states specification file " << sFileName << " could not be open for reading.";
        return;
    }

    // log
    //Log(STATEMANAGER_STREAM, "Loading dialog states specification ...");
    VLOG(DEBUG) << "Loading dialog states specification ...";

    // read the file line by line
    char lpszLine[STRING_MAX];
    while(fgets(lpszLine, STRING_MAX, fid) != NULL) {
        string sAgentName; 
        string sStateName;
        
        // check for comments
        if((lpszLine[0] == '#') || 
           ((lpszLine[0] == '/') && lpszLine[1] == '/'))
            continue;

        // check for empty lines
        if(lpszLine[0] == '\n')
            continue;

        // check for AgentName = StateName pair
        if(SplitOnFirst((string)lpszLine, "=", sAgentName, sStateName)) {
            // extract the model name and filename
            sAgentName = Trim(sAgentName);
            sStateName = Trim(sStateName);
            // then add it to the mapping
            s2sDialogStateNames.insert(
                STRING2STRING::value_type(sAgentName, sStateName));
        }
    }
    fclose(fid);

    // Log the states loaded
    //Log(STATEMANAGER_STREAM, "Dialog states specification loaded successfully.");
    VLOG(DEBUG) << "Dialog states specification loaded successfully.";
}
*/

// D: Updates the state information
bool CStateManagerAgent::UpdateState() {
    // log the activity
    //Log(STATEMANAGER_STREAM, "Updating dialog state ...");
    
    if ( pDMCore == NULL ) {
        VLOG(FATAL) << "pDMCore pointer unspecified!";
        return false;
    }
    
    VLOG(DEBUG) << "Updating dialog state ...";

    // first, if necessary, assemble the agenda of concept expectations
    if (pDMCore->bAgendaModifiedFlag) {
        pDMCore->assembleExpectationAgenda();
    }
 
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
        STRING2STRING::const_iterator iPtr;
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
    //Log(STATEMANAGER_STREAM, "Dialog state update completed: %s at %d "
    //    "(iEHIndex=%d):\n%s",
    //    dsDialogState.sFocusedAgentName.c_str(), vStateHistory.size()-1,
    //    dsDialogState.iEHIndex, GetStateAsString().c_str());
    VLOG(DEBUG) << "Dialog state update completed: " << dsDialogState.sFocusedAgentName << " at " 
                << vStateHistory.size()-1 << " (iEHIndex=" << dsDialogState.iEHIndex << "): " << GetStateAsString();
    return true;
}

// A: Returns a string representing the state
std::string CStateManagerAgent::GetStateAsString(const TDialogState & dsState) const {

    if ( pDMCore == NULL ) {
        VLOG(FATAL) << "pDMCore pointer unspecified!";
        return "";
    }

    if ( pDMCore->_pOutputManager == NULL ) {
        VLOG(FATAL) << "pDMCore->_pOutputManager pointer unspecified!";
        return "";
    }
    
    std::string sDialogState = 
        FormatString("turn_number = %d\nnotify_prompts = %s\ndialog_state = "
            "%s\nnonu_threshold = %.4f\nstack = {\n%s\n}\nagenda = {\n%s\n}\n"
            "input_line_config = {\n%s\n}",
            dsState.iTurnNumber, 
            pDMCore->_pOutputManager->GetPromptsWaitingForNotification().c_str(),
            dsState.sStateName.c_str(),
            pDMCore->GetNonunderstandingThreshold(), 
            Trim(pDMCore->executionStackToString(
                dsState.esExecutionStack)).c_str(), 
            Trim(pDMCore->expectationAgendaToBroadcastString(
                dsState.eaAgenda)).c_str(),
            dsState.sInputLineConfiguration.c_str()
            );

    return sDialogState;
}
// 
std::string CStateManagerAgent::GetStateAsString() const {
    return GetStateAsString(vStateHistory.back());
}

Json::Value CStateManagerAgent::GetStatesAsJson() const {
    
    Json::Value json(Json::arrayJson);
    for ( size_t i = 0; i < vStateHistory.size(); ++i ) {
        json.append(vStateHistory[i].toJson());
    }
    return json;
}



// D: Access to the length of the history
int CStateManagerAgent::GetStateHistoryLength() const {
    return vStateHistory.size();
}

// D: Access the last state
TDialogState &CStateManagerAgent::GetLastState() {
    return vStateHistory.back();
}

// D: Indexing operator to access states 
TDialogState &CStateManagerAgent::operator[](unsigned int i) {
    return vStateHistory[i];
}

bool CStateManagerAgent::Load(const Json::Value & json) {
    VLOG(DEBUG) << "Loading state history from Json.";
    
    if ( !json.isArray() ) {
        VLOG(FATAL) << "Can't load state from non-array Json object!";
        return false;
    }
    vStateHistory.clear();
    for ( size_t i = 0; i < json.size(); ++i ) {
        TDialogState ds;
        if ( !ds.Load(json[i]) ) {
            VLOG(FATAL) << "Fail to load state at " << i ;
            return false;
        }
        vStateHistory.push_back(ds);
    }
    return true;
}

Json::Value CStateManagerAgent::toJson() const {
    Json::Value json(Json::arrayValue);
    for ( size_t i = 0; i < vStateHistory.size(); ++i ) {
        json.append(vStateHistory[i].toJson());
    } 
    return json;
}
