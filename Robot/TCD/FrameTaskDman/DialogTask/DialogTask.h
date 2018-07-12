// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-15 17:33
// Description: 

#ifndef _DIALOG_TASK_H__
#define _DIALOG_TASK_H__

#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DiscourseAgents/AllDiscourseAgents.h"

namespace TrioTDM {

bool InitDialogTaskOnBeginSession();

// 错误处理
#define THIRD_TO_NT_MAIN_2 "ThirdToNtMain2"
#define THIRD_TO_NT_MAIN_12 "ThirdToNtMain12"
#define THIRD_SEND_SMS_TO_NT_MAIN_3 "ThirdSMSPasswordChange"
#define SECOND_SKIP_NT_MAIN_3 "SecondSkipNtMain3"
#define FIFTH_SKIP_NT_MAIN_2 "FifthSkipNtMain2"

#define DMSERVER_CONFIGURATION(Name, Port) \
    extern "C" {\
        char* server_name = Name;\
        int server_port = Port;\
    }\

#define CORE_CONFIGURATION(Configuration) \
    void DialogTaskOnBeginSession() {\
        Configuration\
    }\

#define DEFINE_CONFIDENCE_SLOTNAME(Name) \
    ;\

#define USE_OUTPUT_DEVICES(Devices) \
    {\
        Devices \
    }\

#define OUTPUT_DEVICE(Name, ServerCall, Params) \
    COutputManager::Instance()->RegisterOutputDevice(Name, ServerCall, Params);\

#define DEFAULT_OUTPUT_DEVICE(Name, ServerCall, Params) \
    COutputManager::Instance()->RegisterOutputDevice(Name, ServerCall, Params);\
    COutputManager::Instance()->SetDefaultOutputDevice(Name);\

#define USE_OUTPUT_CLASS(OutputClass) \
    COutputManager::Instance()->SetOutputClass(#OutputClass);

#define USE_LIBRARY_AGENTS(Agents) \
    {\
        Agents\
    }\

// D: declare which library agents to be used
#define LIBRARY_AGENT(AgentType, AgentName, RegistrationFunction, ConfigurationString)\
    CDTTManagerAgent::Instance()->Use(#AgentType, #AgentName, &RegistrationFunction, ConfigurationString);

// D: define the grounding model types to use
#define USE_GROUNDING_MODEL_TYPES(ModelTypes)\
    {\
        ModelTypes\
    }\

// D: define the grounding actions to be used
#define USE_GROUNDING_ACTIONS(Actions)\
    {\
        Actions\
    }\

// D: define for using all the grounding model types
#define USE_ALL_GROUNDING_MODEL_TYPES\
    USE_GROUNDING_MODEL_TYPES(ALL_GROUNDING_MODEL_TYPES)

// D: declare which grounding models are used
#define GROUNDING_MODEL_TYPE(Name, FactoryMethod)\
    TrioTDM::CGroundingManagerAgent::Instance()->RegisterGroundingModelType(#Name, FactoryMethod);

// D: define for using all the grounding actions
#define USE_ALL_GROUNDING_ACTIONS(Configuration)\
    USE_GROUNDING_ACTIONS(ALL_GROUNDING_ACTIONS(Configuration))

// D: declare which grounding actions are to be used
#define GROUNDING_ACTION(Name, GAType, Configuration)\
    pGroundingManager->UseGroundingAction(#Name, new GAType((string)Configuration));

// D: declare the binding filters to be used
#define USE_BINDING_FILTERS(BindingFilters)\
    {\
        BindingFilters\
    }\

// D: declare customized binding filters to be user
#define BINDING_FILTER(Name, Filter)\
    pDMCore->RegisterBindingFilter(Name, Filter);\
    
// D: declare a customized start over routine
#define CUSTOM_START_OVER(Funct)\
    pDMCore->RegisterCustomStartOver(Funct);\


// D: shortcuts for definiting items in structures
#define INT_ITEM(Name) \
    ITEM(Name, CIntConcept)
#define BOOL_ITEM(Name) \
    ITEM(Name, CBoolConcept)
#define STRING_ITEM(Name) \
    ITEM(Name, CStringConcept)
#define FLOAT_ITEM(Name) \
    ITEM(Name, CFloatConcept)
#define CUSTOM_ITEM(Name, Type) \
    ITEM(Name, Type)

// D: shortcuts for defining concepts
#define INT_SYSTEM_CONCEPT(Name) SYSTEM_CONCEPT(Name, CIntConcept)
#define BOOL_SYSTEM_CONCEPT(Name) SYSTEM_CONCEPT(Name, CBoolConcept)
#define STRING_SYSTEM_CONCEPT(Name) SYSTEM_CONCEPT(Name, CStringConcept)
#define FLOAT_SYSTEM_CONCEPT(Name) SYSTEM_CONCEPT(Name, CFloatConcept)
#define CUSTOM_SYSTEM_CONCEPT(Name, Type) SYSTEM_CONCEPT(Name, Type)

#define TRIGGER_NO_UNDERSTAND \
    {\
        VLOG(DEBUG) << "TRIGGER_NO_UNDERSTAND";\
        if ( _owner_core_agent == NULL ) {\
            VLOG(FATAL) << __FUNCTION__ << ", _owner_core_agent is NULL, AgentName: " << _name;\
            return ;\
        }\
        C("action") = "拒识";\
        C("target") = "1";\
        _owner_core_agent->SignalFocusClaimsPhase();\
    }\

#define CLEAR_ROOT_LAST_UPDATE_CONCEPT \
    {\
        VLOG(DEBUG) << "CLEAR_ROOT_LAST_UPDATE_CONCEPT";\
        if ( _owner_core_agent != NULL ) {\
            CDialogAgent* root = _owner_core_agent->GetTaskRoot();\
            if ( root != NULL ) {\
                root->RollbackLastUpdateConcept(false);\
            }\
        }\
    }\

#define TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT \
    CLEAR_ROOT_LAST_UPDATE_CONCEPT \
    TRIGGER_NO_UNDERSTAND \

#define IS_ERROR() \
    {\
        if ( C("action").IsUpdated() && C("target").IsUpdated() ) { \
            if ( ((std::string)C("action") == "超时" || (std::string)C("action") == "拒识") && (std::string)C("target") == "1" ) {\
                C("action").Clear();\
                C("target").Clear();\
                return true;\
            }\
        }\
    }\

#define RESET_CLEAR_ROOT_CONCEPTS \
    {\
        RESET \
        \
        if ( IsABusiness() && _owner_core_agent != NULL ) {\
            CDialogAgent* root = _owner_core_agent->GetTaskRoot();\
            if ( root != NULL ) {\
                root->ClearCurrentConcepts( csUser );\
            }\
        }\
    }\

// D: shortcuts for various agent definitions

#define RESET_ON_COMPLETION ON_COMPLETION(RESET)

// #define RESET_ON_COMPLETION ON_COMPLETION( RESET_CLEAR_ROOT_CONCEPTS )

#define REOPEN_ON_COMPLETION ON_COMPLETION(REOPEN)

#define PARENT_COMPLETION_ON_COMPLETION ON_COMPLETION(\
            if ( _da_parent != NULL ) {\
                _da_parent->SetCompleted();\
            }\
        )\

#define FAILS_WHEN_NOT_CURRENT_MAIN_TOPIC \
    public:\
        virtual bool FailureCriteriaSatisfied() {\
            if ( CDialogAgent::FailureCriteriaSatisfied() ) {\
                return true;\
            }\
            if ( _execute_counter > 0 ) {\
                if ( _owner_core_agent != NULL ) {\
                    CDialogAgent* current_main_topic = _owner_core_agent->GetCurrentMainTopicAgent();\
                    if ( current_main_topic != NULL ) {\
                        return !CDTTManagerAgent::IsAncestorOrEqualOf(_name, current_main_topic->GetName());\
                    }\
                }\
            }\
            return false;\
        }\

#define INT_USER_CONCEPT(Name, GroundingModelSpec) \
    USER_CONCEPT(Name, CIntConcept, GroundingModelSpec)
#define BOOL_USER_CONCEPT(Name, GroundingModelSpec) \
    USER_CONCEPT(Name, CBoolConcept, GroundingModelSpec)
#define STRING_USER_CONCEPT(Name, GroundingModelSpec) \
    USER_CONCEPT(Name, CStringConcept, GroundingModelSpec)
#define FLOAT_USER_CONCEPT(Name, GroundingModelSpec) \
    USER_CONCEPT(Name, CFloatConcept, GroundingModelSpec)
#define CUSTOM_USER_CONCEPT(Name, Type, GroundingModelSpec) \
    USER_CONCEPT(Name, Type, GroundingModelSpec)

// D: shortcuts for various agent check functions
#define COMPLETED(Agent) (A(#Agent).HasCompleted())
#pragma warning (disable:4005)
#define FAILED(Agent) (A(#Agent).HasFailed())
#define SUCCEEDED(Agent) (A(#Agent).HasSucceeded())
#pragma warning (default:4005)
#define IS_ACTIVE_TOPIC(Agent) (pDMCore->AgentIsActive(&(A(#Agent))))
#define IS_FOCUSED(Agent) (pDMCore->IsFocused(&(A(#Agent))))

// D: shortcuts for commands on agents
#define SET_COMPLETED(Agent) {A(#Agent).SetCompleted();}
#define FINISH(Agent) {A(#Agent).SetCompleted(); \
    pDMCore->PopAgentFromExecutionStack(&A(#Agent));}
#define RESET_AGENT(Agent) {A(#Agent).Reset();}
#define REOPEN_AGENT(Agent) {A(#Agent).ReOpen();}
#define REOPEN_TOPIC(Agent) {A(#Agent).ReOpenTopic();}

// D: shortcuts for various concept check functions
#define AVAILABLE(Concept) (C(#Concept).IsAvailableAndGrounded())
#define UNAVAILABLE(Concept) (!(C(#Concept).IsAvailableAndGrounded()))
#define UPDATED(Concept) (C(#Concept).IsUpdatedAndGrounded())
#define NOT_UPDATED(Concept) (!(C(#Concept).IsUpdatedAndGrounded()))
#define INVALIDATED(Concept) (C(#Concept).IsInvalidated())
#define IS_TRUE(Concept) (AVAILABLE(Concept) && (bool)C(#Concept))
#define IS_FALSE(Concept) (AVAILABLE(Concept) && !(bool)C(#Concept))
#define HAS_HISTORY(Concept) (C(#Concept).GetHistorySize() > 0)
#define TURNS_SINCE_LAST_UPDATED(Concept) (C(#Concept).GetTurnsSinceLastUpdated())
#define UPDATED_IN_LAST_TURN(Concept) ((C(#Concept).GetTurnsSinceLastUpdated())==0)
#define UPDATED_IN_PENULTIMATE_TURN(Concept) ((C(#Concept).GetTurnsSinceLastUpdated())==1)

// D: shortcuts for methods on concepts
#define REOPEN_CONCEPT(Concept) {C(#Concept).ReOpen();}
#define RESTORE_CONCEPT(Concept) {C(#Concept).Restore();}
#define CLEAR(Concept) {C(#Concept).Clear();}
#define SIZE(Concept) (C(#Concept).GetSize())

}

#endif
