#include "TSPasswordDoubleConfirm.h"

#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

/* 密码二次确认 */
DEFINE_AGENCY( CPasswordDoubleConfirmAgency,
    IS_MAIN_TOPIC()
    ERROR_HANDLE( THIRD_TO_NT_MAIN_2 )

    DEFINE_SUBAGENTS(
        /* 语音播报：jl0060301[请您再次输入6位数新密码] */
        SUBAGENT( TaskPromptReqUser, CRequestPasswordInput, "none")
        /* 处理用户二次输入 */
        SUBAGENT( DealPasswordDoubleConfirmAgency, CDealPasswordDoubleConfirmAgency, "none")
    )
)

/* 处理用户二次输入 */
DEFINE_AGENCY( CDealPasswordDoubleConfirmAgency,
    EXECUTION_POLICY( START_AGAIN_LEFT_TO_RIGHT )
    SUCCEEDS_WHEN(
        C("is_check_success").IsUpdated()
    )
    /*
     * 变量定义：
     * is_legal_input：输入是否合法
    */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_legal_input )
    )
    DEFINE_SUBAGENTS(
        /* 用户二次输入 */
        SUBAGENT( PasswordDoubleConfirmUserInputAgency, CPasswordDoubleConfirmUserInputAgency, "none")
        /* 判断输入是否合法：[合法：不合法] */
        SUBAGENT( GainIsLegalInput, CPasswordDoubleConfirmGainIsLegalInput, "none")
        /* 处理输入不同状态 */
        SUBAGENT( DealIsLegalInputAgency, CPasswordDoubleConfirmDealIsLegalInputAgency, "none")
    )
)

/* 用户二次输入 */
DEFINE_AGENCY( CPasswordDoubleConfirmUserInputAgency,
    SUCCESS_POLICY_IS_SELECT()

    ON_EXECUTE_FAIL(
        TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT
    )
    DEFINE_SUBAGENTS(
        /* 用户二次输入密码 */
        SUBAGENT( RequestDoubleConfirm, CPasswordDoubleConfirmRequestNewPassword, "none")
        SUBAGENT( SpecificBusinessAgency, CSpecificBusinessAgency, "none")
    )
)

/* 用户二次输入密码 */
DEFINE_REQUEST_AGENT( CPasswordDoubleConfirmRequestNewPassword,
    PRECONDITION( false )
    REQUEST_CONCEPT( confirm_password )
    GRAMMAR_MAPPING( "[dtmf]" )
)

/* 判断输入是否合法：[合法：不合法] */
DEFINE_GAIN_CONCEPT_AGENT( CPasswordDoubleConfirmGainIsLegalInput,
    CONCEPT_NAME( is_legal_input )
    PARAM_NAME( confirm_password )
    GAIN_CONCEPT_PARAM_FUNC( &CGainConceptApi::IsLegalPasswordInput )
)

/* 处理输入不同状态 */
DEFINE_AGENCY( CPasswordDoubleConfirmDealIsLegalInputAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        /* 不合法 */
        SUBAGENT( NotLegalInputAgency, CPasswordDoubleConfirmNotLegalInputAgency, "none")
        /* 合法 */
        SUBAGENT( LegalInputAgency, CPasswordDoubleConfirmLegalInputAgency, "none")
    )
)

/* 不合法 */
DEFINE_AGENCY( CPasswordDoubleConfirmNotLegalInputAgency,
    PRECONDITION(
        C("is_legal_input").IsUpdated() && (bool)C("is_legal_input") == false
    )
    //返回，用户重新输入
    ON_COMPLETION(
        SetCompleted( ctStartAgain);
    )
    //无法识别
    DEFINE_SUBAGENTS(
        SUBAGENT( TriggerNoUnderstand, CTriggerNoUnderstand, "none")
    )
)

/* 合法 */
DEFINE_AGENCY( CPasswordDoubleConfirmLegalInputAgency,
    PRECONDITION(
        C("is_legal_input").IsUpdated() && (bool)C("is_legal_input")
    )
    /* 
     * 变量定义：
     * is_return_primary_menu：是否返回主菜单 */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_return_primary_menu )
    )
    DEFINE_SUBAGENTS(
        /* 返回主菜单状态判断：[返回主菜单：不返回主菜单] */
        SUBAGENT( GainIsReturnPrimaryMenu, CPasswordDoubleConfirmGainIsReturnPrimaryMenu, "none")
        /* 不同返回状态处理 */
        SUBAGENT( DealIsReturnPrimaryMenuAgency, CPasswordDoubleConfirmDealIsReturnPrimaryMenuAgency, "none")
    )
)

/* 返回主菜单状态判断：[返回主菜单：不返回主菜单] */
DEFINE_GAIN_CONCEPT_AGENT( CPasswordDoubleConfirmGainIsReturnPrimaryMenu,
    CONCEPT_NAME( is_return_primary_menu )
    PARAM_NAME( confirm_password )
    GAIN_CONCEPT_PARAM_FUNC( &CGainConceptApi::IsInputReturnPrimaryMenu)
)

/* 不同返回状态处理 */
DEFINE_AGENCY( CPasswordDoubleConfirmDealIsReturnPrimaryMenuAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        /* 返回上级菜单 */
        SUBAGENT( DoubleConfirmReturnPrimaryMenuAgency, CPasswordDoubleConfirmDoubleConfirmReturnPrimaryMenuAgency, "none")
        /* 不返回上级菜单 */
        SUBAGENT( NotDoubleConfirmReturnPrimeryMenuAgency, CPasswordDoubleConfirmNotDoubleConfirmReturnPrimeryMenuAgency, "none")
    )
)

/* 按1#返回上级菜单 */
DEFINE_AGENCY( CPasswordDoubleConfirmDoubleConfirmReturnPrimaryMenuAgency,
    PRECONDITION(
        C("is_return_primary_menu").IsUpdated() && (bool)C("is_return_primary_menu")
    )
    DEFINE_SUBAGENTS(
        /* 跳转到nt_main2 */
        SUBAGENT( SkipNtMain_2, CSkipNtMain_2, "none")
    )
)

//判断两次是否相同
DEFINE_AGENCY( CPasswordDoubleConfirmNotDoubleConfirmReturnPrimeryMenuAgency,
    PRECONDITION(
        C("is_return_primary_menu").IsUpdated() && (bool)C("is_return_primary_menu") == false
    )
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        /* 两次密码输入相同 */
        SUBAGENT( ExecuteIsConfirmPassword, COperatorConfirmPasswordSuccess, "none")
        /* 两次密码不同 */
        SUBAGENT( ExecuteNotConfirmPasswordAgency, CPasswordDoubleConfirmExecuteNotConfirmPasswordAgency, "none")
    )
)

//两次密码输入相同，继续上次流程
DEFINE_OPERATE_AGENT( COperatorConfirmPasswordSuccess,
    PRECONDITION(
        C("confirm_password") == C("new_password")
    )
    OPERATE(
        C("is_check_success") = true;
    )
)

/* 两次密码输入不同 */
DEFINE_AGENCY( CPasswordDoubleConfirmExecuteNotConfirmPasswordAgency,
    PRECONDITION(
        C("confirm_password") != C("new_password")
    )

    DEFINE_SUBAGENTS(
        /* 语音播报：jl0060302 */
        SUBAGENT( NotConfirmPasswordInform, CTaskPromptInform, "none")
        /* 二次密码验证失败 */
        SUBAGENT( OperatorConfirmPasswordFail, COperatorConfirmPasswordFail, "none")
    )
)

/* 二次密码验证失败 */
DEFINE_OPERATE_AGENT( COperatorConfirmPasswordFail,
    OPERATE(
        C("is_check_success") = false;
    )
)

/* 声明节点 */
void RegisterPasswordDoubleConfirm() {
    DECLARE_AGENT( CPasswordDoubleConfirmAgency)
    DECLARE_AGENT( CDealPasswordDoubleConfirmAgency)
    DECLARE_AGENT( CPasswordDoubleConfirmUserInputAgency)
    DECLARE_AGENT( CPasswordDoubleConfirmRequestNewPassword)
    DECLARE_AGENT( CPasswordDoubleConfirmGainIsLegalInput)
    DECLARE_AGENT( CPasswordDoubleConfirmDealIsLegalInputAgency)
    DECLARE_AGENT( CPasswordDoubleConfirmNotLegalInputAgency)
    DECLARE_AGENT( CPasswordDoubleConfirmLegalInputAgency)
    DECLARE_AGENT( CPasswordDoubleConfirmGainIsReturnPrimaryMenu)
    DECLARE_AGENT( CPasswordDoubleConfirmDealIsReturnPrimaryMenuAgency)
    DECLARE_AGENT( CPasswordDoubleConfirmDoubleConfirmReturnPrimaryMenuAgency)
    DECLARE_AGENT( CPasswordDoubleConfirmNotDoubleConfirmReturnPrimeryMenuAgency)
    DECLARE_AGENT( COperatorConfirmPasswordSuccess)
    DECLARE_AGENT( CPasswordDoubleConfirmExecuteNotConfirmPasswordAgency)
    DECLARE_AGENT( COperatorConfirmPasswordFail)
}

}
