#include "TSRemainingTalkMinutesInquiry.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM{
// 剩余通话分钟数外层结构
DEFINE_FRAME_CONCEPT_TYPE(CRemainingTalkMinutesInquiryObject,
    ITEMS(
        BOOL_ITEM(is_exist_record)
        STRING_ITEM(TALK_TOTAL)
        STRING_ITEM(TALK_FREE)
        STRING_ITEM(TALK_USED)
        STRING_ITEM(FAMILY_TALK_TOTAL)
        STRING_ITEM(FAMILY_TALK_FREE)
        STRING_ITEM(FAMILY_TALK_USED)
    )
)

// 业务流程根节点
DEFINE_AGENCY(CRemainingTalkInquiryAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
        CUSTOM_SYSTEM_CONCEPT(result, CRemainingTalkMinutesInquiryObject)
    )

    DEFINE_SUBAGENTS(
        // jl002_lb
        SUBAGENT(TaskPromptInform, CTaskPromptInform, "none")
        SUBAGENT(GetRemainingTalkMinutes, CGetRemainingTalkMinutes, "none")
        SUBAGENT(RemainingTalkDealIsExistRecordAgency, CRemainingTalkDealIsExistRecordAgency, "none")
    )
)

// 取是否有语音套餐
DEFINE_GAIN_CONCEPT_AGENT(CGetRemainingTalkMinutes,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GainRemainingTalkMinutes)
)

// 处理不同返回结果
DEFINE_AGENCY(CRemainingTalkDealIsExistRecordAgency,
    SUCCESS_POLICY_IS_SELECT()

    DEFINE_SUBAGENTS(
        SUBAGENT(RemainingTalkIsHaveRecord, CRemainingTalkIsHaveRecord,"none")
        SUBAGENT(RemainingTalkNotHaveRecord, CRemainingTalkNotHaveRecord,"none")
    )
)

// 处理有通话语音记录
DEFINE_AGENCY(CRemainingTalkIsHaveRecord,
    PRECONDITION(
        C("result")["is_exist_record"].IsUpdated() && (bool)C("result")["is_exist_record"] == true
    )
    DEFINE_SUBAGENTS(
        // 已经缓存了用户信息，此处不需要再调接口取数据
        // 语音播报jl002_tts01(需要重听)
        SUBAGENT(RemainingTalkInquiryBroadcast, CRemainingTalkInquiryBroadcast, "none")
        // 下发短信提示音jl002_02
        SUBAGENT(SendMsgInform, CTaskPromptInform,"none")
        // 下发短信jl_002sms01
        SUBAGENT(ExecuteSendSMS,CExecuteSendSMS, "none")
        // 判断停机（共通函数，需要修改）
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        // 处理停机状态
        SUBAGENT(RemainingTalkDealIsOutOfSeverice, CRemainingTalkDealIsOutOfSeverice, "none")
    )
)

// 无操作节点
DEFINE_INFORM_AGENT( CRemainingTalkInquiryBroadcast,
    RETURN_CODE(dercContinueExecution)
)

// 停机判断结果处理
DEFINE_AGENCY(CRemainingTalkDealIsOutOfSeverice,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(RemainingTalkDealNotOutOfSeverice,CRemainingTalkDealNotOutOfSeverice,"none")
        SUBAGENT(RemainingTalkDealOutOfSeverice,CRemainingTalkDealOutOfSeverice,"none")
    )
)

// 停机处理
DEFINE_AGENCY(CRemainingTalkDealOutOfSeverice,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == true
    )
    DEFINE_SUBAGENTS(
        // 进入停机判断（共通函数，需要再确认）
        SUBAGENT( OutOfServiceAgency, COutOfServiceAgency, "none")
    )
)

DEFINE_AGENCY(CRemainingTalkDealNotOutOfSeverice,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    // 接NtMain3
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 处理无通话语音记录
DEFINE_AGENCY(CRemainingTalkNotHaveRecord,
    PRECONDITION(
        C("result")["is_exist_record"].IsUpdated() && (bool)C("result")["is_exist_record"] == false
    )
    DEFINE_SUBAGENTS(
        // jl002_01
        SUBAGENT(NoTalkInform,CTaskPromptInform,"none")
        // 跳转NtMain2
        SUBAGENT(SkipNtMain_2,CSkipNtMain_2,"none")
    )
)

void RegisterRemainingTalkMinutesInquiry() {
    DECLARE_AGENT(CRemainingTalkInquiryAgency)
    DECLARE_AGENT(CGetRemainingTalkMinutes)
    DECLARE_AGENT(CRemainingTalkDealIsExistRecordAgency)
    DECLARE_AGENT(CRemainingTalkIsHaveRecord)
    DECLARE_AGENT(CRemainingTalkInquiryBroadcast)
    DECLARE_AGENT(CRemainingTalkDealIsOutOfSeverice)
    DECLARE_AGENT(CRemainingTalkDealOutOfSeverice)
    DECLARE_AGENT(CRemainingTalkDealNotOutOfSeverice)
    DECLARE_AGENT(CRemainingTalkNotHaveRecord)
}

}
