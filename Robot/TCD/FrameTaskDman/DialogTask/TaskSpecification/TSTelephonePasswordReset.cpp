#include "TSTelephonePasswordReset.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

/* 手机密码重置节点 */
DEFINE_AGENCY( CTelephonePasswordResetAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION 

    /*
     * 变量定义：
     * is_real_name：是否为实名制
     * out_of_service_reason 停机原因
     */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_real_name)
        STRING_SYSTEM_CONCEPT( certificate )
        //停机原因
        STRING_SYSTEM_CONCEPT( out_of_service_reason )
    )

    DEFINE_SUBAGENTS(
        //获取实名制的状态：[实名制：非实名制]
        SUBAGENT(GainIsRealName, CGainIsRealName, "none")
        //处理实名制不同状态
        SUBAGENT(DealIsRealNameAgency, CDealIsRealNameAgency, "none")
    )
)

/* 获取实名制的状态：[实名制：非实名制] */
DEFINE_GAIN_CONCEPT_AGENT( CGainIsRealName,
    CONCEPT_NAME(is_real_name)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::IsRealName)
)

/* 处理实名制不同状态 */
DEFINE_AGENCY( CDealIsRealNameAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        //是实名制
        SUBAGENT(IsRealNameAgency, CIsRealNameAgency, "none")
        //没有实名制
        SUBAGENT(IsNotRealNameAgency, CIsNotRealNameAgency, "none")
    )
)

/* 没有实名制 */
DEFINE_AGENCY( CIsNotRealNameAgency,
    PRECONDITION(
        C("is_real_name").IsUpdated() && (bool)C("is_real_name") == false
    )

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_out_of_service )
    )

    DEFINE_SUBAGENTS(
        //语音播报：jl004_01
        SUBAGENT(NotRealNameInform, CTaskPromptInform, "none")
        //获取停机状态
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        //处理不同停机状态
        SUBAGENT(TelephonePasswordResetDealIsOutOfServiceAgency, CTelephonePasswordResetDealIsOutOfServiceAgency, "none")
        //跳转到nt_main3
        //SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

/* 处理不同停机状态（未实名） */
DEFINE_AGENCY( CTelephonePasswordResetDealIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        //未停机
        SUBAGENT( TelephonePasswordResetNotOutOfServiceAgency, CTelephonePasswordResetRealNameNotOutOfServiceAgency, "none")
        //停机进入停机判断 （COutOfServiceAgency在sendmsg，sendmsg需要改）
        SUBAGENT( OutOfServiceAgency, COutOfServiceAgency, "none")
    )
)

/* 未停机（未实名） */
DEFINE_AGENCY( CTelephonePasswordResetRealNameNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )

    DEFINE_SUBAGENTS(
        //下发短信 jl004_sms01
        SUBAGENT(NotOutOfServiceExecuteSendSMS, CExecuteSendSMS, "none")
        //跳转到nt_main3
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

/* 是实名制 */
DEFINE_AGENCY( CIsRealNameAgency,
    PRECONDITION(
        C("is_real_name").IsUpdated() && (bool)C("is_real_name") == true
    )

    DEFINE_SUBAGENTS(
        //获取停机原因
        SUBAGENT( GainOutOfServiceReason, CGainOutOfServiceReason, "none" )
        //处理不同停机原因
        SUBAGENT( TelephonePasswordResetDealIsOutOfServiceReasonAgency, CTelephonePasswordResetDealIsOutOfServiceReasonAgency, "none")
    )
)

/* 处理不同停机状态 */
DEFINE_AGENCY( CTelephonePasswordResetDealIsOutOfServiceReasonAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        //报停
        SUBAGENT( TelephonePasswordResetAskForOutOfServiceAgency, CTelephonePasswordResetAskForOutOfServiceAgency, "none")
        //欠停
        SUBAGENT( TelephonePasswordResetArrearsOutOfServiceAgency, CTelephonePasswordResetArrearsOutOfServiceAgency, "none")
        //其他原因停机
        SUBAGENT( TelephonePasswordResetOtherReasonOutOfServiceAgency, CTelephonePasswordResetOtherReasonOutOfServiceAgency, "none")
        //未停机
        SUBAGENT( TelephonePasswordResetNotOutOfServiceAgency, CTelephonePasswordResetNotOutOfServiceAgency, "none")
    )
)

/* 报停 */
DEFINE_AGENCY( CTelephonePasswordResetAskForOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") == "7"
    )
    DEFINE_SUBAGENTS(
        //跳转到nt_main7
        SUBAGENT(SkipNtMain_7, CSkipNtMain_7, "none")
    )
)

/* 欠停 */
DEFINE_AGENCY( CTelephonePasswordResetArrearsOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") == "6"
    )
    DEFINE_SUBAGENTS(
        //跳转到nt_main4
        SUBAGENT(SkipNtMain_4, CSkipNtMain_4, "none")
    )
)

/* 其他原因停机 */
DEFINE_AGENCY( CTelephonePasswordResetOtherReasonOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") != "0" \
        && (std::string)C("out_of_service_reason") != "7" && (std::string)C("out_of_service_reason") != "6"
    )
    DEFINE_SUBAGENTS(
        //语音播报：jl004_02
        SUBAGENT(OtherReasonOutOfServiceInform, CTaskPromptInform, "none")
        //跳转到nt_main3
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

/* 未停机 */
DEFINE_AGENCY( CTelephonePasswordResetNotOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") == "0"
    )
    /* 重置结果 */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(reset_result)
    )
    DEFINE_SUBAGENTS(
        //播报jl004_lb
        SUBAGENT( SetTelephonePassword, CTaskPromptInform, "none")
        //验证身份信息
        SUBAGENT( CertificateVerification, CCertificateVerificationAgency, "none")
        //密码重置（后台处理）
        SUBAGENT(GainResetTelephonePassword, CGainResetTelephonePassword, "none")
        //播报jl004_03
        SUBAGENT(AlreadyRandomPassword, CTaskPromptInform, "none")
        //跳转到nt_main3
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

DEFINE_GAIN_CONCEPT_AGENT( CGainResetTelephonePassword,
    CONCEPT_NAME( reset_result )
    GAIN_CONCEPT_FUNC(&CGainConceptApi::ResetTelephonePassword)
)

DEFINE_TASK_SKIP_AGENT( CSkipTelephonePasswordReset,
    DIALOG_TYPE( CTelephonePasswordResetAgency )
    DIALOG_NAME( TelephonePasswordReset )
)

//声明节点
void RegisterTelephonePasswordReset() {
    DECLARE_AGENT(CTelephonePasswordResetAgency)
    DECLARE_AGENT(CGainIsRealName)
    DECLARE_AGENT(CDealIsRealNameAgency)
    DECLARE_AGENT(CIsNotRealNameAgency)
    DECLARE_AGENT(CTelephonePasswordResetDealIsOutOfServiceAgency)
    DECLARE_AGENT(CTelephonePasswordResetRealNameNotOutOfServiceAgency)
    DECLARE_AGENT(CIsRealNameAgency)
    DECLARE_AGENT(CTelephonePasswordResetDealIsOutOfServiceReasonAgency)
    DECLARE_AGENT(CTelephonePasswordResetAskForOutOfServiceAgency)
    DECLARE_AGENT(CTelephonePasswordResetArrearsOutOfServiceAgency)
    DECLARE_AGENT(CTelephonePasswordResetOtherReasonOutOfServiceAgency)
    DECLARE_AGENT(CTelephonePasswordResetNotOutOfServiceAgency)
    DECLARE_AGENT(CGainResetTelephonePassword)
    DECLARE_AGENT(CSkipTelephonePasswordReset)
}
}
