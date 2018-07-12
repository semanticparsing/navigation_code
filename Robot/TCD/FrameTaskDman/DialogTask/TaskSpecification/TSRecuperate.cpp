#include "TSRecuperate.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM{
// 复机业务流程根节点
DEFINE_AGENCY(CRecuperate,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        // jl012_01
        SUBAGENT( TaskPromptInform, CTaskPromptInform, "none")
        SUBAGENT( SkipToIvrNodeOfRecuperate, CSkipToIvrNodeOfRecuperate, "none")
    )
)

DEFINE_AGENCY(CSkipToIvrNodeOfRecuperate,
    DEFINE_SUBAGENTS(
        // 跳转IVR服务（共通函数，需要实现）
        SUBAGENT(SkipTrasferIVRRecuperate,CSkipTrasferIVRRecuperate,"none")
    )
)

void RegisterRecuperate() {
    DECLARE_AGENT(CRecuperate)
    DECLARE_AGENT(CSkipToIvrNodeOfRecuperate)
}

}
