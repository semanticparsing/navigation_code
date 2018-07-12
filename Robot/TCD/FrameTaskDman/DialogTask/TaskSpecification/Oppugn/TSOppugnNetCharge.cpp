#include "TSOppugnNetCharge.h"
#include "../TSCommonAgent.h"
#include "../TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

// 流量余量查询结构
DEFINE_FRAME_CONCEPT_TYPE(CNetCharRemainingTrafficInquiryObject,
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

// 入口节点
DEFINE_AGENCY( COppugnNetChargeAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
        CUSTOM_SYSTEM_CONCEPT(result, CNetCharRemainingTrafficInquiryObject)
    )

    DEFINE_SUBAGENTS(
        SUBAGENT(GainGetRemainingTrafficRecord, CGetRemainingTrafficRecord, "none")
        SUBAGENT(OppugnNetChargeInform, CTaskPromptInform, "none")
        SUBAGENT(OppugnNetChargeSMSInform, CTaskPromptInform, "none")
        SUBAGENT(ExecuteSendSMS, CExecuteSendSMS, "none")
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        SUBAGENT(OppugnNetChargeDealIsOutOfServiceAgency, COppugnNetChargeDealIsOutOfServiceAgency, "none")
    )
)
     
// 判断是否停机的情况
DEFINE_AGENCY( COppugnNetChargeDealIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnNetChargeNotOutOfServiceAgency, COppugnNetChargeNotOutOfServiceAgency, "none")
        SUBAGENT(OppugnNetChargeOutOfServiceAgency, COppugnNetChargeOutOfServiceAgency, "none")
    )
)

// 未停机
DEFINE_AGENCY( COppugnNetChargeNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 已停机
DEFINE_AGENCY( COppugnNetChargeOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipOutOfServiceJudge, CSkipOutOfServiceJudge, "none")
    )
)

void RegisterOppugnNetCharge() {
    DECLARE_AGENT(COppugnNetChargeAgency)
    DECLARE_AGENT(COppugnNetChargeDealIsOutOfServiceAgency)
    DECLARE_AGENT(COppugnNetChargeNotOutOfServiceAgency)
    DECLARE_AGENT(COppugnNetChargeOutOfServiceAgency)
}

}
