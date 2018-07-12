#include "TSOppugnLowestSpend.h"
#include "../TSCommonAgent.h"
#include "../TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

// 主资费套餐接口AlreadyOutService返回结果
DEFINE_FRAME_CONCEPT_TYPE( COppugnLowestSpendResult,
    ITEMS(
        STRING_ITEM(lowest_spend)  // 最低消费
    )
)

// 入口节点
DEFINE_AGENCY( COppugnLowestSpendAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
        CUSTOM_SYSTEM_CONCEPT(result, COppugnLowestSpendResult)
    )

    DEFINE_SUBAGENTS(
        SUBAGENT(GainGetOppugnLowestSpend, CGainGetOppugnLowestSpend, "none")
        SUBAGENT(OppugnLowestSpendInform, CTaskPromptInform, "none")
        SUBAGENT(OppugnLowestSpendSMSInform, CTaskPromptInform, "none")
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        SUBAGENT(OppugnLowestSpendDealIsOutOfServiceAgency, COppugnLowestSpendDealIsOutOfServiceAgency, "none")
    )
)

// 调用AlreadyOutService接口获取最低消费
DEFINE_GAIN_CONCEPT_AGENT( CGainGetOppugnLowestSpend,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetAlreadyInfo_Lowest)
)
     
// 判断是否停机的情况
DEFINE_AGENCY( COppugnLowestSpendDealIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnLowestSpendNotOutOfServiceAgency, COppugnLowestSpendNotOutOfServiceAgency, "none")
        SUBAGENT(OppugnLowestSpendOutOfServiceAgency, COppugnLowestSpendOutOfServiceAgency, "none")
    )
)

// 未停机
DEFINE_AGENCY( COppugnLowestSpendNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 已停机
DEFINE_AGENCY( COppugnLowestSpendOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipOutOfServiceJudge, CSkipOutOfServiceJudge, "none")
    )
)

void RegisterOppugnLowestSpend() {
    DECLARE_AGENT(COppugnLowestSpendAgency)
    DECLARE_AGENT(CGainGetOppugnLowestSpend)
    DECLARE_AGENT(COppugnLowestSpendDealIsOutOfServiceAgency)
    DECLARE_AGENT(COppugnLowestSpendNotOutOfServiceAgency)
    DECLARE_AGENT(COppugnLowestSpendOutOfServiceAgency)
}

}
