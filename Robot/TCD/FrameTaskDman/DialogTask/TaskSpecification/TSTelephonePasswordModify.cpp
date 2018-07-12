#include "TSTelephonePasswordModify.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

DEFINE_string(simple_password, "10011103111415017", "");

namespace TrioTDM {

/* 密码修改结构体 */
DEFINE_FRAME_CONCEPT_TYPE( CTelephonePasswordModifyInfo,
    ITEMS(
        /* 旧密码 */
        STRING_ITEM(old_password)
        /* 新密码 */
        STRING_ITEM(new_password)
    )
)

/* 手机密码修改节点 */
DEFINE_AGENCY( CTelephonePasswordModifyAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION 

    /*
     * 变量定义：
     * is_out_of_service：是否停机
     */
    DEFINE_CONCEPTS(
        //停机原因
        STRING_SYSTEM_CONCEPT( out_of_service_reason )
    )

    DEFINE_SUBAGENTS(
        /* 获取停机原因 */
        SUBAGENT( GainOutOfServiceReason, CGainOutOfServiceReason, "none" )
        /* 处理不同停机原因 */
        SUBAGENT( TelephonePasswordModifyDealIsOutOfServiceAgency, CTelephonePasswordModifyDealIsOutOfServiceAgency, "none")
    )
)

/* 不同停机状态处理 */
DEFINE_AGENCY( CTelephonePasswordModifyDealIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        /* 报停 */
        SUBAGENT( TelephonePasswordModifyAskForOutOfServiceAgency, CTelephonePasswordModifyAskForOutOfServiceAgency, "none")
        /* 欠停 */
        SUBAGENT( TelephonePasswordModifyArrearsOutOfServiceAgency, CTelephonePasswordModifyArrearsOutOfServiceAgency, "none")
        /* 其他原因停机 */
        SUBAGENT( TelephonePasswordModifyOtherReasonOutOfServiceAgency, CTelephonePasswordModifyOtherReasonOutOfServiceAgency, "none")
        /* 未停机 */
        SUBAGENT( TelephonePasswordModifyNotOutOfServiceAgency, CTelephonePasswordModifyNotOutOfServiceAgency, "none")
    )
)

/* 报停 */
DEFINE_AGENCY( CTelephonePasswordModifyAskForOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") == "7"
    )
    DEFINE_SUBAGENTS(
        //跳转到nt_main7
        SUBAGENT(SkipNtMain_5, CSkipNtMain_5, "none")
    )
)

/* 欠停 */
DEFINE_AGENCY( CTelephonePasswordModifyArrearsOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") == "6"
    )
    DEFINE_SUBAGENTS(
        //跳转到nt_main4
        SUBAGENT(SkipNtMain_4, CSkipNtMain_4, "none")
    )
)

/* 其他原因停机 */
DEFINE_AGENCY( CTelephonePasswordModifyOtherReasonOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") != "0" \
        && (std::string)C("out_of_service_reason") != "7" && (std::string)C("out_of_service_reason") != "6"
    )
    DEFINE_SUBAGENTS(
        //语音播报：jl006_01
        SUBAGENT(OtherReasonOutOfServiceInform, CTaskPromptInform, "none")
        //跳转到nt_main3
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

/* 未停机 */
DEFINE_AGENCY( CTelephonePasswordModifyNotOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") == "0"
    )

    /*
     * 变量定义:
     * is_real_name：是否为实名制
     */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_real_name)
    )

    DEFINE_SUBAGENTS(
        /* 获取实名制的状态：[实名制：非实名制] */
        SUBAGENT(GainIsRealName, CGainIsRealName, "none")
        /* 处理实名制不同状态 */
        SUBAGENT(TelephonePasswordModifyDealIsRealNameAgency, CTelephonePasswordModifyDealIsRealNameAgency, "none")
    )
)

/* 处理实名制不同流程 */
DEFINE_AGENCY( CTelephonePasswordModifyDealIsRealNameAgency,
    SUCCESS_POLICY_IS_SELECT()

    DEFINE_SUBAGENTS(
        /* 未实名 */
        SUBAGENT(TelephonePasswordModifyNoRealNameAgency, CTelephonePasswordModifyNoRealNameAgency, "none")
        /* 已实名 */
        SUBAGENT(TelephonePasswordModifyRealNameAgency, CTelephonePasswordModifyRealNameAgency, "none")
    )
)

/* 未实名 */
DEFINE_AGENCY( CTelephonePasswordModifyNoRealNameAgency,
    PRECONDITION(
        C("is_real_name").IsUpdated() && (bool)C("is_real_name") == false
    )
    DEFINE_SUBAGENTS(
        //语音播报：jl006_04
        SUBAGENT(NoRealNameInform, CTaskPromptInform, "none")
        //发送短信
        SUBAGENT(NoRealName, CExecuteSendSMS, "none")
        //跳转到nt_main3
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

/* 已实名 */
DEFINE_AGENCY( CTelephonePasswordModifyRealNameAgency,
    PRECONDITION(
        C("is_real_name").IsUpdated() && (bool)C("is_real_name") == true
    )

    /*
     * 变量定义:
     * original_password: 原始密码
     * new_password: 新密码
     * confirm_password: 密码确认
     * is_success：是否成功
     */
    DEFINE_CONCEPTS(
        STRING_SYSTEM_CONCEPT( original_password )
        STRING_SYSTEM_CONCEPT( new_password )
        STRING_SYSTEM_CONCEPT( confirm_password )
    )
    DEFINE_SUBAGENTS(
        /* 语音播报：jl006_lb */
        SUBAGENT(ReadyModifyInform, CTaskPromptInform, "none")
        /* 进入原始密码验证节点 */
        SUBAGENT( OriginalPasswordVerification, COriginalPasswordVerificationAgency, "none")
        /* 输入新密码及后续后台修改密码 */
        SUBAGENT( PasswordInputAndModifyAgency, CPasswordInputAndModifyAgency, "none")
        /* 跳转到nt_main3 */
        SUBAGENT( SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

/* 输入新密码及后续后台修改密码 */
DEFINE_AGENCY( CPasswordInputAndModifyAgency,
    EXECUTION_POLICY( START_AGAIN_LEFT_TO_RIGHT )

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_process_success )
        STRING_SYSTEM_CONCEPT( is_success )
    )
    SUCCEEDS_WHEN(
        C("is_process_success").IsUpdated() && (bool)C("is_process_success") == true
    )

    DEFINE_SUBAGENTS( 
        /* 进入新密码两次验证节点 */
        SUBAGENT( NewPasswordTwoTimesCheckAgency, CNewPasswordTwoTimesCheckAgency, "none")
        SUBAGENT( DealTelephonePasswordModifyAgency, CDealTelephonePasswordModifyAgency, "none")
        /* 修改密码[后台处理] */
        //SUBAGENT( ModifyTelephonePassword, CModifyTelephonePassword, "none")
        /* 对密码修改不同结果进行处理 */
        //SUBAGENT( TelephonePasswordModifyDealResult, CTelephonePasswordModifyDealResultAgency, "none")
    )
)

DEFINE_AGENCY( CDealTelephonePasswordModifyAgency,
    DEFINE_SUBAGENTS(
        /* 修改密码[后台处理] */
        SUBAGENT( ModifyTelephonePassword, CModifyTelephonePassword, "none")
        /* 对密码修改不同结果进行处理 */
        SUBAGENT( TelephonePasswordModifyDealResult, CTelephonePasswordModifyDealResultAgency, "none")
    )
)

/* 新密码两次验证 */
DEFINE_AGENCY( CNewPasswordTwoTimesCheckAgency,
    EXECUTION_POLICY( START_AGAIN_LEFT_TO_RIGHT )
    /*
     * 变量定义：
     * is_check_success：验证是否成功
     */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_check_success )
    )
    SUCCEEDS_WHEN(
        C("is_check_success").IsUpdated() && (bool)C("is_check_success")
    )
    DEFINE_SUBAGENTS(
        /* 进入新密码输入节点 */
        SUBAGENT( NewPasswordInput, CNewPasswordInputAgency, "none")
        /* 进入新密码二次确认节点 */
        SUBAGENT( PasswordDoubleConfirm, CPasswordDoubleConfirmAgency, "none")
    )
)

/* 修改密码[后台处理] */
DEFINE_GAIN_CONCEPT_AGENT( CModifyTelephonePassword,
    DEFINE_CONCEPTS(
        CUSTOM_SYSTEM_CONCEPT(password_info, CTelephonePasswordModifyInfo)
    )
    /* 结构体类型输入参数 */
    EXECUTE_PARAM(
        C("password_info")["old_password"] = C("original_password");
        C("password_info")["new_password"] = C("confirm_password");
        return C("password_info");
    )
    /* 输出参数 */
    CONCEPT_NAME( is_success )
    GAIN_CONCEPT_PARAM_FUNC(&CGainConceptApi::ModifyTelephonePassword)
)

/* 对密码修改不同结果进行处理 */
DEFINE_AGENCY( CTelephonePasswordModifyDealResultAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        /* 新密码修改成功 */
        SUBAGENT(TelephonePasswordModifySuccessAgency, CTelephonePasswordModifySuccessAgency, "none")
        /* 新密码修改失败 */
        SUBAGENT(TelephonePasswordModifyFailAgency, CTelephonePasswordModifyFailAgency, "none")
        /* 密码过于简单 */
        SUBAGENT(TelephonePasswordModifySimpleAgency, CTelephonePasswordModifySimpleAgency, "none")
    )
)

/* 新密码修改成功 */
DEFINE_AGENCY( CTelephonePasswordModifySuccessAgency,
    PRECONDITION(
        C("is_success").IsUpdated() && (std::string)C("is_success") == "0"
    )

    DEFINE_SUBAGENTS(
        /* 语音播报：jl006_02[密码修改成功，请使用] */
        SUBAGENT( TelephonePasswordModifySuccessInform, CTaskPromptInform, "none")
        SUBAGENT( OperatePasswordModifySuccess, COperatePasswordModifySuccess, "none")
    )
)

DEFINE_OPERATE_AGENT( COperatePasswordModifySuccess,
    OPERATE(
        C("is_process_success") = true;
    )
)

DEFINE_OPERATE_AGENT( COperatePasswordModifyFail,
    OPERATE(
        C("is_process_success") = true;
    )
)

DEFINE_OPERATE_AGENT( COperatePasswordModifySimple,
    OPERATE(
        C("is_process_success") = false;
    )
)

/* 新密码修改失败 */
DEFINE_AGENCY( CTelephonePasswordModifyFailAgency,
    PRECONDITION(
        C("is_success").IsUpdated() && (std::string)C("is_success") != FLAGS_simple_password \
        && (std::string)C("is_success") != "0"
    )

    DEFINE_SUBAGENTS(
        /* 语音播报：jl006_03[密码修改失败，请稍后再试] */
        SUBAGENT( TelephonePasswordModifyFailInform, CTaskPromptInform, "none")
        SUBAGENT( OperatePasswordModifyFail, COperatePasswordModifyFail, "none")
    )
)

/* 密码过于简单 */
DEFINE_AGENCY( CTelephonePasswordModifySimpleAgency,
    PRECONDITION(
        C("is_success").IsUpdated() && (std::string)C("is_success") == FLAGS_simple_password
    )

    DEFINE_SUBAGENTS(
        /* 语音播报：jl006_05 */
        SUBAGENT( TelephonePasswordModifySimpleInform, CTaskPromptInform, "none")
        SUBAGENT( OperatePasswordModifySimple, COperatePasswordModifySimple, "none")
    )
)

/* 声明节点 */
void RegisterTelephonePasswordModify() {
    DECLARE_AGENT( CTelephonePasswordModifyAgency)
    DECLARE_AGENT( CTelephonePasswordModifyDealIsOutOfServiceAgency)
    DECLARE_AGENT( CTelephonePasswordModifyAskForOutOfServiceAgency)
    DECLARE_AGENT( CTelephonePasswordModifyArrearsOutOfServiceAgency)
    DECLARE_AGENT( CTelephonePasswordModifyOtherReasonOutOfServiceAgency)
    DECLARE_AGENT( CTelephonePasswordModifyNotOutOfServiceAgency)
    DECLARE_AGENT( CTelephonePasswordModifyDealIsRealNameAgency)
    DECLARE_AGENT( CTelephonePasswordModifyNoRealNameAgency)
    DECLARE_AGENT( CTelephonePasswordModifyRealNameAgency)
    DECLARE_AGENT( CPasswordInputAndModifyAgency)
    DECLARE_AGENT( CNewPasswordTwoTimesCheckAgency)
    DECLARE_AGENT( CModifyTelephonePassword)
    DECLARE_AGENT( CTelephonePasswordModifyDealResultAgency)
    DECLARE_AGENT( CTelephonePasswordModifySuccessAgency)
    DECLARE_AGENT( CTelephonePasswordModifyFailAgency)
    DECLARE_AGENT( CTelephonePasswordModifySimpleAgency)
    DECLARE_AGENT( COperatePasswordModifySuccess)
    DECLARE_AGENT( COperatePasswordModifyFail)
    DECLARE_AGENT( COperatePasswordModifySimple)
    DECLARE_AGENT( CDealTelephonePasswordModifyAgency)
}

}
