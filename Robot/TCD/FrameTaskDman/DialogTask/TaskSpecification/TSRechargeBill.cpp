#include "TSRechargeBill.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM{
// 手机充值业务流程根节点
DEFINE_AGENCY(CRechargeBill,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        // jl011_01
        SUBAGENT( TaskPromptInform, CTaskPromptInform, "none")
        SUBAGENT( SkipToIvrNodeOfRechargeBill, CSkipToIvrNodeOfRechargeBill, "none")
    )
)

DEFINE_AGENCY(CSkipToIvrNodeOfRechargeBill,
    DEFINE_SUBAGENTS(
        // 跳转IVR服务（共通函数，需要实现）
        SUBAGENT(SkipTrasferIVRRecharge, CSkipTrasferIVRRecharge,"none")
    )
)

void RegisterRechargeBill() {
    DECLARE_AGENT(CRechargeBill)
    DECLARE_AGENT(CSkipToIvrNodeOfRechargeBill)
}

}
