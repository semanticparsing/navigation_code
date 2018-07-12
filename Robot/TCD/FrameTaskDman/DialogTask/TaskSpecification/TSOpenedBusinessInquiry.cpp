#include "TSOpenedBusinessInquiry.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM{
// 已开通并收固定费的业务
DEFINE_FRAME_CONCEPT_TYPE(CPROD_PRC_NAMEArrayItem,
    ITEMS(
        STRING_ITEM(PROD_NAME)
    )
)

// 结构体数组
DEFINE_ARRAY_CONCEPT_TYPE( CPROD_PRC_NAMEArrayItem, CPROD_PRC_NAMEArrayItems )

// 查询结果
DEFINE_FRAME_CONCEPT_TYPE(CMaintarObject,
    ITEMS(
        STRING_ITEM(PROD_PRC_NAME)
        CUSTOM_ITEM(PROD_PRC_NAMEarry1, CPROD_PRC_NAMEArrayItems)
    )
)

// 业务流程根节点
DEFINE_AGENCY(COpenedBusinessInquiryAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION
    
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
        BOOL_SYSTEM_CONCEPT(is_success)
        CUSTOM_SYSTEM_CONCEPT(result, CMaintarObject)
    )
    
    DEFINE_SUBAGENTS(
        // jl008_lb
        SUBAGENT(TaskPromptInform, CTaskPromptInform, "none")
        SUBAGENT(GainOpenedBusiness, CGainOpenedBusiness, "none")
        // jl008_tts01(需要重听)
        SUBAGENT(OpenedBusinessBroadcast, CTaskPromptInform, "none")
        // 下发短信jl_008sms01(Boss平台自动下发短信)
        SUBAGENT(GainAndExecuteSendSMS, CGainAndExecuteSendSMS, "none")
        // 停机判断（通用接口需要实现）
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService,"none")
        // 停机判断结果处理
        SUBAGENT(OpenedBusinessDealIsOutOfSeverice, COpenedBusinessDealIsOutOfSeverice,"none")
    )
)

DEFINE_GAIN_CONCEPT_AGENT(CGainAndExecuteSendSMS,
    CONCEPT_NAME(is_success)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::SendAlreadyInfoSMS)
)

// 获取已开业务列表
DEFINE_GAIN_CONCEPT_AGENT(CGainOpenedBusiness,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GainOpenedBusinessList)
)

// 停机判断结果处理
DEFINE_AGENCY(COpenedBusinessDealIsOutOfSeverice,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OpenedBusinessDealNotOutOfSeverice,COpenedBusinessDealNotOutOfSeverice, "none")
        SUBAGENT(OpenedBusinessDealOutOfSeverice, COpenedBusinessDealOutOfSeverice, "none")
    )
)

// 停机处理
DEFINE_AGENCY(COpenedBusinessDealOutOfSeverice,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == true
    )
    DEFINE_SUBAGENTS(
        // 进入停机判断
        SUBAGENT(OutOfServiceJudgeAgency, COutOfServiceAgency, "none")
    )
)

// 未停机处理
DEFINE_AGENCY(COpenedBusinessDealNotOutOfSeverice,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    // 接ntmain3
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

void RegisterOpenedBusinessInquiry() {
    DECLARE_AGENT(COpenedBusinessInquiryAgency)
    DECLARE_AGENT(CGainAndExecuteSendSMS)
    DECLARE_AGENT(CGainOpenedBusiness)
    DECLARE_AGENT(COpenedBusinessDealIsOutOfSeverice)
    DECLARE_AGENT(COpenedBusinessDealOutOfSeverice)
    DECLARE_AGENT(COpenedBusinessDealNotOutOfSeverice)
}

}
