#include "TSModifyPassword.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM{
// 密码修改业务流程
DEFINE_AGENCY(CModifyPassword,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION
    
    ERROR_HANDLE(THIRD_TO_NT_MAIN_2)
    DEFINE_SUBAGENTS(
        //语音播报：sd005_lb[请问您是要修改手机密码还是宽带密码]
        SUBAGENT( TaskPromptInform, CTaskPromptReqUser, "none")
        //处理用户请求,识别结果判断
        SUBAGENT( PasswordChangePossibleUserReqAgency, CPasswordChangePossibleUserReqAgency, "none")
        SUBAGENT( TriggerSpecificBusinessAgency, CTriggerSpecificBusinessAgency, "none")
    )
)

//处理用户请求，识别结果判断
DEFINE_AGENCY( CPasswordChangePossibleUserReqAgency,
    SUCCESS_POLICY_IS_SELECT() 
    //拒识处理
    ON_EXECUTE_FAIL(
        TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT
    )
    DEFINE_SUBAGENTS(
        //密码修改类型选择
        SUBAGENT( PasswordChangeSelectRequestOperationType, CPasswordChangeSelectRequestOperationType, "none")
        SUBAGENT(SpecificBusinessAgency, CSpecificBusinessAgency, "none")
    )
)
DEFINE_REQUEST_AGENT( CPasswordChangeSelectRequestOperationType,
    PRECONDITION( false )
    /* 输入选择类型 */
    REQUEST_CONCEPT( entity_tag )
    ON_CONCEPT_UPDATED(
        FILL_TRIGGER_CONCEPT
    )
    GRAMMAR_MAPPING( "*[entity_tag]" )
)

void RegisterModifyPassword() {
    DECLARE_AGENT(CModifyPassword)
    DECLARE_AGENT(CPasswordChangePossibleUserReqAgency)
    DECLARE_AGENT(CPasswordChangeSelectRequestOperationType)
}

}
