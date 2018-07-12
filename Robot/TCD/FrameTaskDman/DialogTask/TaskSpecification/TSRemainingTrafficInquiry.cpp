#include "TSRemainingTrafficInquiry.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM{

// 流量余量查询结构
DEFINE_FRAME_CONCEPT_TYPE(CRemainingTrafficInquiryObject,
    ITEMS(
        BOOL_ITEM(is_exist_record)
        STRING_ITEM(GPRS_TOTAL)
        STRING_ITEM(GPRS_FREE)
        STRING_ITEM(GPRS_USED)
        STRING_ITEM(AREA_GPRS_TOTAL)
        STRING_ITEM(AREA_GPRS_FREE)
        STRING_ITEM(AREA_GPRS_USED)
    )
)

// 业务流程根节点
DEFINE_AGENCY(CRemainingTrafficInquiryAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION
    
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_out_of_service )
        CUSTOM_SYSTEM_CONCEPT(result, CRemainingTrafficInquiryObject)
    )
    
    DEFINE_SUBAGENTS(
        //jl003_lb
        SUBAGENT(TaskPromptInform, CTaskPromptInform, "none")
        SUBAGENT(GetRemainingTrafficRecord, CGetRemainingTrafficRecord, "none")
        SUBAGENT(RemainingTrafficDealIsExistRecordAgency, CRemainingTrafficDealIsExistRecordAgency, "none")
    )
)

DEFINE_GAIN_CONCEPT_AGENT(CGetRemainingTrafficRecord,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GainRemainingTrafficPackage)
)

DEFINE_AGENCY(CRemainingTrafficDealIsExistRecordAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(RemainingTrafficNotExistRecordAgency, CRemainingTrafficNotExistRecordAgency, "none")
        SUBAGENT(RemainingTrafficExistRecordAgency, CRemainingTrafficExistRecordAgency, "none")
    )
)

// 停机判断结果处理
DEFINE_AGENCY(CRemainingTrafficDealIsOutOfSeverice,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(RemainingTrafficDealNotOutOfSeverice,CRemainingTrafficDealNotOutOfSeverice,"none")
        SUBAGENT(RemainingTrafficDealOutOfSeverice,CRemainingTrafficDealOutOfSeverice,"none")
    )
)

// 停机处理
DEFINE_AGENCY(CRemainingTrafficDealOutOfSeverice,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == true
    )
    DEFINE_SUBAGENTS(
        // 进入停机判断
        SUBAGENT(OutOfServiceAgency, COutOfServiceAgency, "none")
    )
)

DEFINE_AGENCY(CRemainingTrafficDealNotOutOfSeverice,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    // 接NtMain3
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

DEFINE_AGENCY(CRemainingTrafficExistRecordAgency,
    PRECONDITION(
        C("result")["is_exist_record"].IsUpdated() && (bool)C("result")["is_exist_record"] == true
    )
    
    DEFINE_SUBAGENTS(
        // jl003_tts01 (需要重听)
        SUBAGENT(RemainingTrafficBroadcast, CTaskPromptInform, "none")
        // jl003_02
        SUBAGENT(SendMsgInform, CTaskPromptInform, "none")
        // 下发短信jl_003sms01
        SUBAGENT(ExecuteSendSMS,CExecuteSendSMS, "none")
        // 判断停机
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        // 处理停机状态
        SUBAGENT(RemainingTrafficDealIsOutOfSeverice, CRemainingTrafficDealIsOutOfSeverice, "none")
    )
)

DEFINE_AGENCY(CRemainingTrafficNotExistRecordAgency,
    PRECONDITION(
        C("result")["is_exist_record"].IsUpdated() && (bool)C("result")["is_exist_record"] == false
    )
    DEFINE_SUBAGENTS(
        // jl003_01（需要重听）
        SUBAGENT(NoTrafficInform, CTaskPromptInform, "none")
        // 跳转到NtMain2
        SUBAGENT(SkipNtMain_2, CSkipNtMain_2, "none")
    )
)

void RegisterRemainingTrafficInquiry() {
    DECLARE_AGENT(CRemainingTrafficInquiryAgency)
    DECLARE_AGENT(CGetRemainingTrafficRecord)
    DECLARE_AGENT(CRemainingTrafficDealIsExistRecordAgency)
    DECLARE_AGENT(CRemainingTrafficDealIsOutOfSeverice)
    DECLARE_AGENT(CRemainingTrafficDealOutOfSeverice)
    DECLARE_AGENT(CRemainingTrafficDealNotOutOfSeverice)
    DECLARE_AGENT(CRemainingTrafficExistRecordAgency)
    DECLARE_AGENT(CRemainingTrafficNotExistRecordAgency)
}

}
