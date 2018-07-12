#include "TSOppugnMonthlyRent.h"
#include "../TSCommonAgent.h"
#include "../TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

// 主资费套餐接口MaintarOutService返回结果
DEFINE_FRAME_CONCEPT_TYPE( COppugnMonthlyRentMaintarResult,
    ITEMS(
        STRING_ITEM(monthly_rent_fee)  // 月租费
    )
)

// 入口节点
DEFINE_AGENCY( COppugnMonthlyRentAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
        CUSTOM_SYSTEM_CONCEPT(result, COppugnMonthlyRentMaintarResult)
    )

    DEFINE_SUBAGENTS(
        SUBAGENT(GainGetOppugnMonthlyRentMaintar, CGainGetOppugnMonthlyRentMaintar, "none")
        SUBAGENT(OppugnMonthlyRentMaintarInform, CTaskPromptInform, "none")
        SUBAGENT(OppugnMonthlyRentSMSInform, CTaskPromptInform, "none")
        SUBAGENT(ExecuteSendSMS, CExecuteSendSMS, "none")
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        SUBAGENT(OppugnMonthlyRentDealIsOutOfServiceAgency, COppugnMonthlyRentDealIsOutOfServiceAgency, "none")
    )
)

// 调用MaintarOutService接口获取月租费
DEFINE_GAIN_CONCEPT_AGENT( CGainGetOppugnMonthlyRentMaintar,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetMaintarInfo_MonthlyRentFee)
)
     
// 判断是否停机的情况
DEFINE_AGENCY( COppugnMonthlyRentDealIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnMonthlyRentNotOutOfServiceAgency, COppugnMonthlyRentNotOutOfServiceAgency, "none")
        SUBAGENT(OppugnMonthlyRentOutOfServiceAgency, COppugnMonthlyRentOutOfServiceAgency, "none")
    )
)

// 未停机
DEFINE_AGENCY( COppugnMonthlyRentNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 已停机
DEFINE_AGENCY( COppugnMonthlyRentOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipOutOfServiceJudge, CSkipOutOfServiceJudge, "none")
    )
)

void RegisterOppugnMonthlyRent() {
    DECLARE_AGENT(COppugnMonthlyRentAgency)
    DECLARE_AGENT(CGainGetOppugnMonthlyRentMaintar)
    DECLARE_AGENT(COppugnMonthlyRentDealIsOutOfServiceAgency)
    DECLARE_AGENT(COppugnMonthlyRentNotOutOfServiceAgency)
    DECLARE_AGENT(COppugnMonthlyRentOutOfServiceAgency)
}

}
