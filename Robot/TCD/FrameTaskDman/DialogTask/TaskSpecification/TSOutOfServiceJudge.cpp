#include "TSOutOfServiceJudge.h"
#include "TSCommonAgent.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

//停机判断节点
DEFINE_AGENCY( COutOfServiceJudgeAgency,
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION
    PRECONDITION( false )
    DEFINE_CONCEPTS(
        //停机原因
        STRING_SYSTEM_CONCEPT( out_of_service_reason )
    )
    DEFINE_SUBAGENTS(
        //获取停机原因
        SUBAGENT( GainOutOfServiceReason, CGainOutOfServiceReason, "none" )
        //处理不同停机原因
        SUBAGENT( DealOutOfServiceReasonAgency, CDealOutOfServiceReasonAgency, "none")
    )
)

//获取停机原因
DEFINE_GAIN_CONCEPT_AGENT( CGainOutOfServiceReason,
    CONCEPT_NAME( out_of_service_reason )
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetOutOfServiceReason)
)

//处理不同停机原因
DEFINE_AGENCY( CDealOutOfServiceReasonAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        //办理停机
        SUBAGENT(AskForOutOfServiceAgency, CAskForOutOfServiceAgency, "none")
        //欠费停机
        SUBAGENT(ArrearsOutOfServiceAgency, CArrearsOutOfServiceAgency, "none")
        //未实名登记
        SUBAGENT(NonRealNameRegistrationOutOfServiceAgency, CNonRealNameRegistrationOutOfServiceAgency, "none")
        //其他停机原因
        SUBAGENT(OtherReasonOutofServiceAgency, COtherReasonOutofServiceAgency, "none")
    )
)

//办理停机
DEFINE_AGENCY( CAskForOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") == "7"
    )
    DEFINE_SUBAGENTS(
        //办理停机语音播报
        SUBAGENT(AskForOutOfServiceInform, CTaskPromptInform, "none")
        //跳转到nt_main8
        SUBAGENT(NtMainAgency_8, CSkipNtMain_8, "none")
    )
)

//欠费停机
DEFINE_AGENCY( CArrearsOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") == "6"
    )
    DEFINE_SUBAGENTS(
        //欠费停机语音播报
        SUBAGENT(ArrearsOutOfServiceInform, CTaskPromptInform, "none")
        //跳转到nt_main4
        SUBAGENT(NtMainAgency_4, CSkipNtMain_4, "none")
    )
)

//未实名登记
DEFINE_AGENCY( CNonRealNameRegistrationOutOfServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") == "5"
    )
    DEFINE_SUBAGENTS(
        //未实名登记语音播报
        SUBAGENT(NonRealNameRegistrationOutOfServiceInform, CTaskPromptInform, "none")
        //跳转到nt_main3
        SUBAGENT(NtMainAgency_3, CSkipNtMain_3, "none")
    )
)

//其他原因
DEFINE_AGENCY( COtherReasonOutofServiceAgency,
    PRECONDITION(
        C("out_of_service_reason").IsUpdated() && (std::string)C("out_of_service_reason") != "0" && \
        (std::string)C("out_of_service_reason") != "5" && (std::string)C("out_of_service_reason") != "6" && \
        (std::string)C("out_of_service_reason") != "7"
    )
    DEFINE_SUBAGENTS(
        //其他原因语音播报
        SUBAGENT(OtherReasonOutofServiceInform, CTaskPromptInform, "none" )
        //跳转到nt_main3
        SUBAGENT( NtMainAgency_3, CSkipNtMain_3, "none")
    )
)

//跳过节点
DEFINE_TASK_SKIP_AGENT( CSkipOutOfServiceJudge,
    DIALOG_TYPE( COutOfServiceJudgeAgency )
    DIALOG_NAME( OutOfServiceJudge )
)

//声明节点
void RegisterOutOfServiceJudge() {
    DECLARE_AGENT(COutOfServiceJudgeAgency)
    DECLARE_AGENT(CGainOutOfServiceReason)
    DECLARE_AGENT(CDealOutOfServiceReasonAgency)
    DECLARE_AGENT(CAskForOutOfServiceAgency)
    DECLARE_AGENT(CArrearsOutOfServiceAgency)
    DECLARE_AGENT(CNonRealNameRegistrationOutOfServiceAgency)
    DECLARE_AGENT(COtherReasonOutofServiceAgency)
    DECLARE_AGENT( CSkipOutOfServiceJudge )
}

}
