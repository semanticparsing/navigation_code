
#ifndef __STATEMANAGERAGENT_H__
#define __STATEMANAGERAGENT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Agent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DialogAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/CoreAgents/DMCoreAgent.h"

//-----------------------------------------------------------------------------
// CStateManagerAgent Class - 
//   This class is an agent which keeps track of state information in the 
//   dialog manager
//-----------------------------------------------------------------------------

namespace TrioTDM {

// D: structure describing the state of the dialog manager at some point
typedef struct CDialogState {
    
    CDialogState();
    //CDialogState(CDMCoreAgent * pDMCore);
    virtual ~CDialogState();
    
    TFloorStatus fsFloorStatus;             // who has the floor?
    std::string sFocusedAgentName;               // the name of the focused agent
    TExecutionStack esExecutionStack;       // the execution stack
    TExpectationAgenda eaAgenda;            // the expectation agenda
    TSystemAction saSystemAction;           // the current system action
    std::string sInputLineConfiguration;         // string representation of the input
                                            // line config at this state (lm, etc)
    int iTurnNumber;                        // the current turn number
    int iEHIndex;                           // the execution history index matching 
                                            // this dialog state
    std::string sStateName;                      // the name of the current dialog state
    
    //ZW new interface 
    bool Load(const Json::Value& json); 
    Json::Value toJson() const;
} TDialogState;

// D: the CStateManager class definition
class CStateManagerAgent /*: public CAgent*/ {

public:
    // dialog state name definitions
    std::map<std::string, std::string> s2sDialogStateNames;

    // private vector containing a history of the states that the DM went 
    // through
    std::vector<TDialogState, std::allocator<TDialogState> > vStateHistory;

    // variable containing the state broadcast address
    //std::string sStateBroadcastAddress;

public:

    //---------------------------------------------------------------------
    // Constructor and destructor
    //---------------------------------------------------------------------
    //
    // Default constructor
    CStateManagerAgent();

    // Virtual destructor
    virtual ~CStateManagerAgent();

    // static function for dynamic agent creation
    //static CAgent* AgentFactory(string sAName, string sAConfiguration);

    //---------------------------------------------------------------------
    // CAgent Class overwritten methods 
    //---------------------------------------------------------------------
    //
    virtual void Reset();    // overwritten Reset

public:

    //---------------------------------------------------------------------
    // StateManagerAgent class specific public methods
    //---------------------------------------------------------------------

    // Load the dialog state name mapping from a file
    //void LoadDialogStateNames(string sFileName);
    
    //ZW new interface
    bool Load(const Json::Value& json);
    Json::Value toJson() const;
    void Set(CDMCoreAgent * myDMCore);

    // Updates the state information
    bool UpdateState();

    // Returns a string representing the state
    std::string GetStateAsString(const TDialogState & dsState) const;
    std::string GetStateAsString() const;

    Json::Value GetStatesAsJson() const;

    // Access to the length of the history
    int GetStateHistoryLength() const;

    // Access the last state
    TDialogState &GetLastState();

    // Indexing operator to access states 
    TDialogState &operator[](unsigned int i);
    
private:
    CDMCoreAgent * pDMCore;
};

}
#endif // __STATEMANAGERAGENT_H__
