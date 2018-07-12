#include "TSSystemException.h"
#include "TSCommonAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

//系统异常节点    
DEFINE_AGENCY( CSystemExpectionAgency,
    PRECONDITION(false)
    IS_NOT_DTS_AGENT()
    RESET_ON_COMPLETION
    DEFINE_SUBAGENTS(
        //异常播报
        SUBAGENT( SystemExpectionBroadcastInform, CSystemExpectionBroadcastInform, "none")
        //跳转到nt_main3
        SUBAGENT( SkipNtMain_3, CSkipNtMain_3, "none")
    )
)
//异常语音播报
DEFINE_INFORM_AGENT( CSystemExpectionBroadcastInform,
    PROMPT("template SystemExpection.Broadcast")
)

//跳过系统异常
DEFINE_TASK_SKIP_AGENT( CSkipSystemExpection,
    DIALOG_TYPE( CSystemExpectionAgency)
    DIALOG_NAME( SystemExpectionAgency)
)

//节点声明
void RegisterSystemException() {
    DECLARE_AGENT( CSystemExpectionAgency)
    DECLARE_AGENT( CSystemExpectionBroadcastInform)
    DECLARE_AGENT( CSkipSystemExpection)
}

}
