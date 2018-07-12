#include "DialogTask.h"
#include "TaskSpecification/AllTaskSpecifications.h"

namespace TrioTDM {

DECLARE_DIALOG_TASK_ROOT(CTaskFlowRoot, "")
SYSTEM_EXPECT_DIALOG( SystemExpection )

DECLARE_AGENTS(
    RegisterAllTaskSpecificationAgents();
)

bool InitDialogTaskOnBeginSession() {
    VLOG(DEBUG) << __FUNCTION__;

    VLOG(DATA) << "Init FrameTaskDman start.";
    if ( TrioTDM::CRegistryAgentType::Instance() == NULL ) {
        VLOG(FATAL) << "CRegistryAgentType::Instance() is NULL.";
        return false;
    }
    TrioTDM::CRegistryAgentType::Instance()->Clear();

    // CDMCoreAgent
    TrioTDM::CRegistryAgentType::Instance()->RegisterAgentType("CDMCoreAgent", TrioTDM::CDMCoreAgent::AgentFactory);

    // CDTTManagerAgent
    TrioTDM::CRegistryAgentType::Instance()->RegisterAgentType("CDTTManagerAgent", TrioTDM::CDTTManagerAgent::AgentFactory);
    if ( TrioTDM::CDTTManagerAgent::Instance() == NULL ) {
        VLOG(FATAL) << "TrioTDM::CDTTManagerAgent::Instance() is NULL.";
        return false;
    }
    USE_LIBRARY_AGENTS(
        // 帮助
        LIBRARY_AGENT( CHelpAgency, Help, RegisterHelpAgent, "none")
        // 重听机制
        LIBRARY_AGENT( CRepeatAgency, Repeat, RegisterRepeatAgent, "none")
        // 统一的错误处理
        LIBRARY_AGENT( CErrorHandleAgency, ErrorHandle, RegisterErrorHandle, "none")
        // 系统异常
        LIBRARY_AGENT( CSystemExpectionAgency, SystemExpection, RegisterSystemException, "none")
        // 集外说法
        LIBRARY_AGENT( COtherMeanAgency, OtherMean, RegisterOtherMean, "none")
    )
    TrioTDM::CDTTManagerAgent::Instance()->Initialize();
    // TrioTDM::CDTTManagerAgent::Instance()->Register();

    // CGroundingManagerAgent
    TrioTDM::CRegistryAgentType::Instance()->RegisterAgentType("CGroundingManagerAgent", TrioTDM::CGroundingManagerAgent::AgentFactory);
    if ( TrioTDM::CGroundingManagerAgent::Instance() == NULL ) {
        VLOG(FATAL) << "TrioTDM::CGroundingManagerAgent::Instance() is NULL.";
        return false;
    }
    TrioTDM::CGroundingManagerAgent::Instance()->Initialize();
    // TrioTDM::CGroundingManagerAgent::Instance()->Register();

    // CTrafficManagerAgent
    TrioTDM::CRegistryAgentType::Instance()->RegisterAgentType("CTrafficManagerAgent", TrioTDM::CTrafficManagerAgent::AgentFactory);
    if ( TrioTDM::CTrafficManagerAgent::Instance() == NULL ) {
        VLOG(FATAL) << "TrioTDM::CTrafficManagerAgent::Instance() is NULL.";
        return false;
    }
    TrioTDM::CTrafficManagerAgent::Instance()->Initialize();
    // TrioTDM::CTrafficManagerAgent::Instance()->Register();
    VLOG(DATA) << "Init FrameTaskDman Over.";

    TrioTDM::CRegistryAgentType::Instance()->RegisterAgentType("CStateManagerAgent", TrioTDM::CStateManagerAgent::AgentFactory);
    if ( TrioTDM::CStateManagerAgent::Instance() == NULL ) {
        VLOG(FATAL) << "TrioTDM::CStateManagerAgent::Instance() is NULL.";
        return false;
    }
    TrioTDM::CStateManagerAgent::Instance()->Initialize();
    // TrioTDM::CStateManagerAgent::Instance()->Register();

    TrioTDM::CRegistryAgentType::Instance()->RegisterAgentType("CInteractionEventManagerAgent", TrioTDM::CInteractionEventManagerAgent::AgentFactory);
    if ( TrioTDM::CInteractionEventManagerAgent::Instance() == NULL ) {
        VLOG(FATAL) << "TrioTDM::CInteractionEventManagerAgent::Instance() is NULL.";
        return false;
    }
    TrioTDM::CInteractionEventManagerAgent::Instance()->Initialize();
    // TrioTDM::CInteractionEventManagerAgent::Instance()->Register();

    TrioTDM::CRegistryAgentType::Instance()->RegisterAgentType("COutputManagerAgent", TrioTDM::COutputManagerAgent::AgentFactory);
    /*
    if ( TrioTDM::COutputManagerAgent::Instance() == NULL ) {
        VLOG(FATAL) << "TrioTDM::COutputManagerAgent::Instance() is NULL.";
        return false;
    }
    TrioTDM::COutputManagerAgent::Instance()->Initialize();
    TrioTDM::COutputManagerAgent::Instance()->Register();
    */
/*
    TrioTDM::CDMCoreAgent::Instance()->Set(
            TrioTDM::CDTTManagerAgent::Instance(),
            TrioTDM::CStateManagerAgent::Instance(),
            TrioTDM::CInteractionEventManagerAgent::Instance(),
            TrioTDM::COutputManagerAgent::Instance(),
            TrioTDM::CRegistry::Instance()
            );
    // Register Grounding model
*/
    USE_ALL_GROUNDING_MODEL_TYPES

    return true;
}

}
