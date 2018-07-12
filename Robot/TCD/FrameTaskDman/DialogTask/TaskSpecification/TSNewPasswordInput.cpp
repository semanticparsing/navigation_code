#include "TSNewPasswordInput.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

/* 新密码修改节点 */
DEFINE_AGENCY( CNewPasswordInputAgency,
    IS_MAIN_TOPIC()
    ERROR_HANDLE( THIRD_TO_NT_MAIN_2 )

    DEFINE_SUBAGENTS(
        /* 语音播报：jl0060201 */
        SUBAGENT( TaskPromptReqUser, CRequestPasswordInput, "none")
        /* 处理新密码输入 */
        SUBAGENT( DealNewPasswordInputAgency, CDealNewPasswordInputAgency, "none")
    )
)

/* 处理新密码输入 */
DEFINE_AGENCY( CDealNewPasswordInputAgency,
    EXECUTION_POLICY( START_AGAIN_LEFT_TO_RIGHT )
    SUCCEEDS_WHEN(
        C("is_simple_password").IsUpdated() && (bool)C("is_simple_password") == false
    )
    /*
        变量定义：
    is_legal_input：输入是否合法
    is_simple_password：是否为简单密码
    */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_legal_input )
        BOOL_SYSTEM_CONCEPT( is_simple_password )
    )
    DEFINE_SUBAGENTS(
        /* 新密码输入 */
        SUBAGENT( NewPasswordInputUserInputAgency, CNewPasswordInputUserInputAgency, "none")
        /* 新密码合法性判断：[合法：不合法] */
        SUBAGENT( GainIsLegalInput, CNewPasswordInputGainIsLegalInput, "none")
        /* 处理新密码不同的状态 */
        SUBAGENT( DealIsLegalInputAgency, CNewPasswordInputDealIsLegalInputAgency, "none")
    )
)
/* 新密码输入 */
DEFINE_AGENCY( CNewPasswordInputUserInputAgency,
    SUCCESS_POLICY_IS_SELECT()

    ON_EXECUTE_FAIL(
        TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT
    )
    DEFINE_SUBAGENTS(
        /* 新密码输入响应 */
        SUBAGENT( RequestNewPassword, CNewPasswordInputRequestNewPassword, "none")
        /* 其他业务流程处理 */
        SUBAGENT( SpecificBusinessAgency, CSpecificBusinessAgency, "none")
    )
)
/* 新密码输入响应 */
DEFINE_REQUEST_AGENT( CNewPasswordInputRequestNewPassword,
    PRECONDITION( false )
    REQUEST_CONCEPT( new_password )
    GRAMMAR_MAPPING( "[dtmf]" )
)

/* 新密码合法性判断：[合法：不合法] */
DEFINE_GAIN_CONCEPT_AGENT( CNewPasswordInputGainIsLegalInput,
    CONCEPT_NAME( is_legal_input )
    PARAM_NAME( new_password )
    GAIN_CONCEPT_PARAM_FUNC( &CGainConceptApi::IsLegalPasswordInput )
)

/* 处理新密码不同的状态 */
DEFINE_AGENCY( CNewPasswordInputDealIsLegalInputAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        /* 新密码不合法 */
        SUBAGENT( NotLegalInputAgency, CNewPasswordInputNotLegalInputAgency, "none")
        /* 新密码合法 */
        SUBAGENT( LegalInputAgency, CNewPasswordInputLegalInputAgency, "none")
    )
)

/* 新密码不合法 */
DEFINE_AGENCY( CNewPasswordInputNotLegalInputAgency,
    PRECONDITION(
        C("is_legal_input").IsUpdated() && (bool)C("is_legal_input") == false
    )
    /* 重新输入 */
    ON_COMPLETION(
        SetCompleted( ctStartAgain);
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( TriggerNoUnderstand, CTriggerNoUnderstand, "none")
    )
)

/* 新密码合法 */
DEFINE_AGENCY( CNewPasswordInputLegalInputAgency,
    PRECONDITION(
        C("is_legal_input").IsUpdated() && (bool)C("is_legal_input")
    )
    /* 
       变量定义：
    is_return_primary_menu：是否返回上级菜单
    */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_return_primary_menu )
    )
    DEFINE_SUBAGENTS(
        /* 识别结果判断，上级菜单返回状态:[返回：未返回] */
        SUBAGENT( GainIsReturnPrimaryMenu, CNewPasswordInputGainIsReturnPrimaryMenu, "none")
        /* 处理上级菜单返回状态 */
        SUBAGENT( DealIsReturnPrimaryMenuAgency, CNewPasswordInputDealIsReturnPrimaryMenuAgency, "none")
    )
)

/* 上级菜单返回状态:[返回：未返回] */
DEFINE_GAIN_CONCEPT_AGENT( CNewPasswordInputGainIsReturnPrimaryMenu,
    CONCEPT_NAME( is_return_primary_menu )
    PARAM_NAME( new_password )
    GAIN_CONCEPT_PARAM_FUNC( &CGainConceptApi::IsInputReturnPrimaryMenu)
)

/*处理上级菜单返回状态*/
DEFINE_AGENCY( CNewPasswordInputDealIsReturnPrimaryMenuAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        /* 按*号键，返回上级菜单 */
        SUBAGENT( NewPasswordReturnPrimaryMenuAgency, CNewPasswordReturnPrimaryMenuAgency, "none")
        /* 没有返回上级菜单 */
        SUBAGENT( NewPasswordNotReturnPrimeryMenuAgency, CNewPasswordNotReturnPrimeryMenuAgency, "none")
    )
)

/*处理上级菜单返回状态*/
DEFINE_AGENCY( CNewPasswordReturnPrimaryMenuAgency,
    PRECONDITION(
        C("is_return_primary_menu").IsUpdated() && (bool)C("is_return_primary_menu")
    )
    DEFINE_SUBAGENTS(
        /* 跳转到nt_main2 */
        SUBAGENT( SkipNtMain_2, CSkipNtMain_2, "none")
    )
)

//判断是否简单密码
DEFINE_AGENCY( CNewPasswordNotReturnPrimeryMenuAgency,
    PRECONDITION(
        C("is_return_primary_menu").IsUpdated() && (bool)C("is_return_primary_menu") == false
    )
    DEFINE_SUBAGENTS(
        /* 判断简单密码的状态:[是简单密码：不是简单密码] */
        SUBAGENT( GainIsSimplePassword, CGainIsSimplePassword, "none")
        /* 处理简单密码判断不同状态 */
        SUBAGENT( DealIsSimplePasswordAgency, CNewPasswordInputDealIsSimplePasswordAgency, "none")
    )
)
/* 判断简单密码的状态:[是简单密码：不是简单密码] */
DEFINE_GAIN_CONCEPT_AGENT( CGainIsSimplePassword,
    CONCEPT_NAME( is_simple_password )
    PARAM_NAME( new_password )
    GAIN_CONCEPT_PARAM_FUNC( &CGainConceptApi::IsSimplePassword)
)

/* 处理简单密码判断不同状态 */
DEFINE_AGENCY( CNewPasswordInputDealIsSimplePasswordAgency,
    SUCCESS_POLICY_IS_SELECT()

    DEFINE_SUBAGENTS(
        /* 不是简单密码 */
        SUBAGENT( ExecuteNotSimplePassword, CExecuteNotSimplePassword, "none")
        /* 是简单密码 */
        SUBAGENT( ExecuteIsSimplePasswordAgency, CExecuteIsSimplePasswordAgency, "none")
    )
)

//非简单密码
DEFINE_EXECUTE_AGENT( CExecuteNotSimplePassword,
    PRECONDITION( false )
    SUCCEEDS_WHEN(
        C("is_simple_password").IsUpdated() && (bool)C("is_simple_password") == false
    )
)

//简单密码
DEFINE_AGENCY( CExecuteIsSimplePasswordAgency,
    SUCCESS_POLICY_IS_SELECT()
    PRECONDITION(
        C("is_simple_password").IsUpdated() && (bool)C("is_simple_password")
    )
    ON_COMPLETION(
        SetCompleted( ctStartAgain);
    )
    DEFINE_SUBAGENTS(    
        /* 语音播报：jl0060202 */
        SUBAGENT( SimplePasswordReqUserInform, CRequestPasswordInput, "none")
    )
)

/* 声明节点 */
void RegisterNewPasswordInput() {
    DECLARE_AGENT( CNewPasswordInputAgency)
    DECLARE_AGENT( CDealNewPasswordInputAgency)
    DECLARE_AGENT( CNewPasswordInputUserInputAgency)
    DECLARE_AGENT( CNewPasswordInputRequestNewPassword)
    DECLARE_AGENT( CNewPasswordInputGainIsLegalInput)
    DECLARE_AGENT( CNewPasswordInputDealIsLegalInputAgency)
    DECLARE_AGENT( CNewPasswordInputNotLegalInputAgency)
    DECLARE_AGENT( CNewPasswordInputLegalInputAgency)
    DECLARE_AGENT( CNewPasswordInputGainIsReturnPrimaryMenu)
    DECLARE_AGENT( CNewPasswordInputDealIsReturnPrimaryMenuAgency)
    DECLARE_AGENT( CNewPasswordReturnPrimaryMenuAgency)
    DECLARE_AGENT( CNewPasswordNotReturnPrimeryMenuAgency)
    DECLARE_AGENT( CGainIsSimplePassword)
    DECLARE_AGENT( CNewPasswordInputDealIsSimplePasswordAgency)
    DECLARE_AGENT( CExecuteNotSimplePassword)
    DECLARE_AGENT( CExecuteIsSimplePasswordAgency)
}

}
