#include "TSOppugnTrafficFee.h"
#include "../TSCommonAgent.h"
#include "../TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

// 套餐外流量费接口billMsgOutService返回结果
//DEFINE_FRAME_CONCEPT_TYPE( CExcessTrafficFeeResult,
//    ITEMS(
//        STRING_ITEM(excess_traffic_fee)  // 套餐外流量费
//    )
//)

// 4G专网接口FourGonlyAndNetOutService返回结果
DEFINE_FRAME_CONCEPT_TYPE( CFourGExclusiveNetResult,
    ITEMS(
        STRING_ITEM(net_fee)  // 套餐外流量费
        STRING_ITEM(four_g_traffic)  // 4G专网剩余流量
        STRING_ITEM(area_traffic)  // 省内剩余流量
    )
)

// 剩余流量播报元素
//DEFINE_FRAME_CONCEPT_TYPE( CRemainingTrafficResultItem,
//    ITEMS(
//        STRING_ITEM(traffic_type)
//        STRING_ITEM(traffic_free)
//    )
//)

// 剩余流量播报数组
//DEFINE_ARRAY_CONCEPT_TYPE( CRemainingTrafficResultItem, CRemainingTrafficResultItems )

// 剩余流量接口NumRemainOutServicer返回结果
DEFINE_FRAME_CONCEPT_TYPE( CRemainingTrafficResult,
    ITEMS(
        BOOL_ITEM(is_exist_record)
        STRING_ITEM(GPRS_TOTAL)  // 国内流量总量
        STRING_ITEM(GPRS_FREE)  // 国内流量剩余
        STRING_ITEM(GPRS_USED)  // 国内流量已使用
        STRING_ITEM(AREA_GPRS_TOTAL)  // 省内流量总量
        STRING_ITEM(AREA_GPRS_FREE)  // 省内流量剩余
        STRING_ITEM(AREA_GPRS_USED)  // 省内流量已使用
        STRING_ITEM(excess_traffic_fee)  // 套餐外流量费
    )
)

// 入口节点
DEFINE_AGENCY( COppugnTrafficFeeAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
        STRING_SYSTEM_CONCEPT(excess_traffic_fee)
        CUSTOM_SYSTEM_CONCEPT(result, CRemainingTrafficResult)
    )

    DEFINE_SUBAGENTS(
        SUBAGENT(GainGetExcessTrafficFee, CGainGetExcessTrafficFee, "none")
        SUBAGENT(GainGetRemainTrafficInfo, CGainGetRemainTrafficInfo, "none")
        SUBAGENT(SetExcessTrafficFee, CSetExcessTrafficFeeAgent, "none")
        SUBAGENT(OppugnTrafficDealResultAgency, COppugnTrafficDealResultAgency, "none")
    )
)

// 调用billMsgOutService接口获取套餐外流量费
DEFINE_GAIN_CONCEPT_AGENT( CGainGetExcessTrafficFee,
    CONCEPT_NAME(excess_traffic_fee)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetExcessTrafficFee)
)

// 调用NumRemainOutService接口获取剩余流量
DEFINE_GAIN_CONCEPT_AGENT( CGainGetRemainTrafficInfo,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GainRemainingTrafficOppugn)
)

// 判断套餐外流量费和剩余流量的情况
DEFINE_AGENCY( COppugnTrafficDealResultAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnTrafficIsExcessHasRemainAgency, COppugnTrafficIsExcessHasRemainAgency, "none")
        SUBAGENT(OppugnTrafficIsExcessNoRemainAgency, COppugnTrafficIsExcessNoRemainAgency, "none")
        SUBAGENT(OppugnTrafficNoExcessHasRemainAgency, COppugnTrafficNoExcessHasRemainAgency, "none")
        SUBAGENT(OppugnTrafficNoExcessNoRemainAgency, COppugnTrafficNoExcessNoRemainAgency, "none")
    )
)

// 产生套餐外流量 && 有剩余流量
DEFINE_AGENCY( COppugnTrafficIsExcessHasRemainAgency,
    PRECONDITION(
        (std::string)C("excess_traffic_fee") != "0" && C("result")["is_exist_record"].IsUpdated() && (bool)C("result")["is_exist_record"] == true
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(GainCheckFourGExclusiveNetAgency, CGainCheckFourGExclusiveNetAgency, "none")
    )
)

// 判断是否4G专网数据
DEFINE_AGENCY( CGainCheckFourGExclusiveNetAgency,
    DEFINE_CONCEPTS(
        CUSTOM_SYSTEM_CONCEPT(result, CFourGExclusiveNetResult)
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(GainGetFourGExclusiveNetResultAgency, CGainGetFourGExclusiveNetResultAgency, "none")
        SUBAGENT(OppugnTrafficDealFourGExclusiveNetAgency, COppugnTrafficDealFourGExclusiveNetAgency, "none")
    )
)

// 调用FourGonlyAndNetOutService接口获取4G专网数据
DEFINE_GAIN_CONCEPT_AGENT( CGainGetFourGExclusiveNetResultAgency,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetFourGExclusiveNetResult)
)

// 判断4G专网的情况
DEFINE_AGENCY( COppugnTrafficDealFourGExclusiveNetAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnTrafficIsFourGExclusiveNetAgency, COppugnTrafficIsFourGExclusiveNetAgency, "none")
        SUBAGENT(OppugnTrafficNotFourGExclusiveNetAgency, COppugnTrafficNotFourGExclusiveNetAgency, "none")
    )
)

// 4G专网
DEFINE_AGENCY( COppugnTrafficIsFourGExclusiveNetAgency,
    PRECONDITION(
        (std::string)C("result")["four_g_traffic"] != "0MB"
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(IsFourGExclusiveNetInform, COppugnTrafficContinueInform, "none")
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 非4G专网
DEFINE_AGENCY( COppugnTrafficNotFourGExclusiveNetAgency,
    PRECONDITION(
        (std::string)C("result")["four_g_traffic"] == "0MB"
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(NotFourGExclusiveNetInform, COppugnTrafficContinueInform, "none")
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 产生套餐外流量 && 没有剩余流量
DEFINE_AGENCY( COppugnTrafficIsExcessNoRemainAgency,
    PRECONDITION(
        (std::string)C("excess_traffic_fee") != "0" && C("result")["is_exist_record"].IsUpdated() && (bool)C("result")["is_exist_record"] == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(IsExcessNoRemainInform, COppugnTrafficContinueInform, "none")
        SUBAGENT(ExecuteSendSMS, CExecuteSendSMS, "none")
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        SUBAGENT(OppugnTrafficDealIsOutOfServiceAgency, COppugnTrafficDealIsOutOfServiceAgency, "none")
    )
)

// 给播报模板赋值
DEFINE_OPERATE_AGENT( CSetExcessTrafficFeeAgent,
    OPERATE(
        C("result")["excess_traffic_fee"] = (std::string)C("excess_traffic_fee");
    )
)

// 未产生套餐外流量 && 没有剩余流量
DEFINE_AGENCY( COppugnTrafficNoExcessNoRemainAgency,
    PRECONDITION(
        (std::string)C("excess_traffic_fee") == "0" && C("result")["is_exist_record"].IsUpdated() && (bool)C("result")["is_exist_record"] == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(NoExcessNoRemainInform, COppugnTrafficContinueInform, "none")
        SUBAGENT(ExecuteSendSMS, CExecuteSendSMS, "none")
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        SUBAGENT(OppugnTrafficDealIsOutOfServiceAgency, COppugnTrafficDealIsOutOfServiceAgency, "none")
    )
)

// 未产生套餐外流量 && 有剩余流量
DEFINE_AGENCY( COppugnTrafficNoExcessHasRemainAgency,
    PRECONDITION(
        (std::string)C("excess_traffic_fee") == "0" && C("result")["is_exist_record"].IsUpdated() && (bool)C("result")["is_exist_record"] == true \
        && ( (C("result")["AREA_GPRS_FREE"].IsUpdated() && (std::string)C("result")["AREA_GPRS_FREE"] != "0MB") \
            || (C("result")["GPRS_FREE"].IsUpdated() && (std::string)C("result")["GPRS_FREE"] != "0MB") )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(NoExcessHasRemainInform, COppugnTrafficContinueInform, "none")
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 无操作节点
DEFINE_INFORM_AGENT( COppugnTrafficContinueInform,
    RETURN_CODE(dercContinueExecution)
)

// 判断是否停机的情况
DEFINE_AGENCY( COppugnTrafficDealIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnTrafficNotOutOfServiceAgency, COppugnTrafficNotOutOfServiceAgency, "none")
        SUBAGENT(OppugnTrafficOutOfServiceAgency, COppugnTrafficOutOfServiceAgency, "none")
    )
)

// 未停机
DEFINE_AGENCY( COppugnTrafficNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 已停机
DEFINE_AGENCY( COppugnTrafficOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipOutOfServiceJudge, CSkipOutOfServiceJudge, "none")
    )
)

void RegisterOppugnTrafficFee() {
    DECLARE_AGENT(COppugnTrafficFeeAgency)
    DECLARE_AGENT(CGainGetExcessTrafficFee)
    DECLARE_AGENT(CGainGetRemainTrafficInfo)
    DECLARE_AGENT(COppugnTrafficDealResultAgency)
    DECLARE_AGENT(COppugnTrafficIsExcessHasRemainAgency)
    DECLARE_AGENT(CGainCheckFourGExclusiveNetAgency)
    DECLARE_AGENT(CGainGetFourGExclusiveNetResultAgency)
    DECLARE_AGENT(COppugnTrafficDealFourGExclusiveNetAgency)
    DECLARE_AGENT(COppugnTrafficIsFourGExclusiveNetAgency)
    DECLARE_AGENT(COppugnTrafficNotFourGExclusiveNetAgency)
    DECLARE_AGENT(COppugnTrafficIsExcessNoRemainAgency)
    DECLARE_AGENT(CSetExcessTrafficFeeAgent)
    DECLARE_AGENT(COppugnTrafficNoExcessNoRemainAgency)
    DECLARE_AGENT(COppugnTrafficNoExcessHasRemainAgency)
    DECLARE_AGENT(COppugnTrafficContinueInform)
    DECLARE_AGENT(COppugnTrafficDealIsOutOfServiceAgency)
    DECLARE_AGENT(COppugnTrafficNotOutOfServiceAgency)
    DECLARE_AGENT(COppugnTrafficOutOfServiceAgency)
}

}
