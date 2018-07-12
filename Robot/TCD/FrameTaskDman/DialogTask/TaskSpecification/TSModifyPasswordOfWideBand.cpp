#include "TSModifyPasswordOfWideBand.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM{
// 宽带密码修改业务流程根节点
DEFINE_AGENCY(CModifyPasswordOfWideBand,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION
    
    DEFINE_SUBAGENTS(
        // jl007_01
        SUBAGENT(TaskPromptInform, CTaskPromptInform, "none")
        SUBAGENT( SkipToIvrNodeOfModifyPasswordOfWideBand, CSkipToIvrNodeOfModifyPasswordOfWideBand, "none")
    )
)

DEFINE_AGENCY(CSkipToIvrNodeOfModifyPasswordOfWideBand,
    DEFINE_SUBAGENTS(
        // 跳转IVR服务
        SUBAGENT(SkipTrasferIVRWideBandPasswordModify, CSkipTrasferIVRWideBandPasswordModify,"none")
    )
)

void RegisterModifyPasswordOfWideBand() {
    DECLARE_AGENT(CModifyPasswordOfWideBand)
    DECLARE_AGENT(CSkipToIvrNodeOfModifyPasswordOfWideBand)
}

}
