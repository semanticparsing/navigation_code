#include "TSNonOnlineBusiness.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

/* 定义未上线业务节点 */
DEFINE_AGENCY( CNonOnlineBusinessAgency,
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    PRECONDITION( false )
    RESET_ON_COMPLETION
    
    DEFINE_SUBAGENTS(
        /* 通知语音播报：nt_main_noservice */
        SUBAGENT(NoService, CTaskPromptInform, "none")
        /* 发送短信：nt_main_noservice_sms */
        SUBAGENT(ExecuteSendSMS, CExecuteSendSMS, "none")
        /* 跳转到IVR首层 */
        SUBAGENT(RequestTrasferIVRFloor, CRequestTrasferIVRFloor, "none")
    )
)

/* 声明节点 */
void RegisterNonOnlineBusiness() {
    DECLARE_AGENT( CNonOnlineBusinessAgency)
}

}
