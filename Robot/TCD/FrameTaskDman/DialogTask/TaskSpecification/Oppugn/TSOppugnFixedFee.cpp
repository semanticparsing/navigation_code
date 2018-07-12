#include "TSOppugnFixedFee.h"
#include "../TSCommonAgent.h"
#include "../TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

// 主资费套餐接口MaintarOutService返回结果
DEFINE_FRAME_CONCEPT_TYPE( COppugnFixedFeeMaintarItem,
    ITEMS(
        STRING_ITEM(prod_prc_name_array)  // 固定费业务
    )
)

// 固定费业务数组
DEFINE_ARRAY_CONCEPT_TYPE( COppugnFixedFeeMaintarItem, COppugnFixedFeeMaintarItems )

// 主资费套餐接口MaintarOutService返回结果
DEFINE_FRAME_CONCEPT_TYPE( COppugnFixedFeeMaintarResult,
    ITEMS(
        BOOL_ITEM(has_fixed_fee_business)
        CUSTOM_ITEM(business_list, COppugnFixedFeeMaintarItems)
    )
)

// 入口节点
DEFINE_AGENCY( COppugnFixedFeeAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
        CUSTOM_SYSTEM_CONCEPT(result, COppugnFixedFeeMaintarResult)
    )

    DEFINE_SUBAGENTS(
        SUBAGENT(GainGetOppugnFixedFeeMaintar, CGainGetOppugnFixedFeeMaintar, "none")
        SUBAGENT(OppugnFixedFeeDealHasFixedFeeBusiness, COppugnFixedFeeDealHasFixedFeeBusinessAgency, "none")
    )
)

// 判断是否有固定费业务的情况
DEFINE_AGENCY( COppugnFixedFeeDealHasFixedFeeBusinessAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnFixedFeeHasFixedFeeBusinessAgency, COppugnFixedFeeHasFixedFeeBusinessAgency, "none")
        SUBAGENT(OppugnFixedFeeNoFixedFeeBusinessAgency, COppugnFixedFeeNoFixedFeeBusinessAgency, "none")
    )
)

// 没有固定费业务的情况
DEFINE_AGENCY( COppugnFixedFeeNoFixedFeeBusinessAgency,
    PRECONDITION(
        C("result")["has_fixed_fee_business"].IsUpdated() && (bool)C("result")["has_fixed_fee_business"] == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnFixedFeeNoFixedFeeBusinessInform, CTaskPromptInform, "none")
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 有固定费业务的情况
DEFINE_AGENCY( COppugnFixedFeeHasFixedFeeBusinessAgency,
    PRECONDITION(
        C("result")["has_fixed_fee_business"].IsUpdated() && (bool)C("result")["has_fixed_fee_business"] == true
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnFixedFeeHasFixedFeeBusinessInform, CTaskPromptInform, "none")
        SUBAGENT(OppugnFixedFeeSMSInform, CTaskPromptInform, "none")
        SUBAGENT(ExecuteSendSMS, CExecuteSendSMS, "none")
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        SUBAGENT(OppugnFixedFeeDealIsOutOfServiceAgency, COppugnFixedFeeDealIsOutOfServiceAgency, "none")
    )
)

// 调用MaintarOutService接口获取月租费
DEFINE_GAIN_CONCEPT_AGENT( CGainGetOppugnFixedFeeMaintar,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetMaintarInfo_FixedFee)
)
 
// 判断是否停机的情况
DEFINE_AGENCY( COppugnFixedFeeDealIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnFixedFeeNotOutOfServiceAgency, COppugnFixedFeeNotOutOfServiceAgency, "none")
        SUBAGENT(OppugnFixedFeeOutOfServiceAgency, COppugnFixedFeeOutOfServiceAgency, "none")
    )
)

// 未停机
DEFINE_AGENCY( COppugnFixedFeeNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 已停机
DEFINE_AGENCY( COppugnFixedFeeOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipOutOfServiceJudge, CSkipOutOfServiceJudge, "none")
    )
)

void RegisterOppugnFixedFee() {
    DECLARE_AGENT(COppugnFixedFeeAgency)
    DECLARE_AGENT(COppugnFixedFeeDealHasFixedFeeBusinessAgency)
    DECLARE_AGENT(COppugnFixedFeeNoFixedFeeBusinessAgency)
    DECLARE_AGENT(COppugnFixedFeeHasFixedFeeBusinessAgency)
    DECLARE_AGENT(CGainGetOppugnFixedFeeMaintar)
    DECLARE_AGENT(COppugnFixedFeeDealIsOutOfServiceAgency)
    DECLARE_AGENT(COppugnFixedFeeNotOutOfServiceAgency)
    DECLARE_AGENT(COppugnFixedFeeOutOfServiceAgency)
}

}
