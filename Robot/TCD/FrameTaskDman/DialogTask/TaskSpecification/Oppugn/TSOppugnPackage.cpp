#include "TSOppugnPackage.h"
#include "../TSCommonAgent.h"
#include "../TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

// 主资费套餐接口MaintarOutService返回结果
DEFINE_FRAME_CONCEPT_TYPE( COppugnPackageMaintarResult,
    ITEMS(
        STRING_ITEM(prod_prc_name)  // 主资费套餐
    )
)

// 入口节点
DEFINE_AGENCY( COppugnPackageAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(send_sms)
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
        CUSTOM_SYSTEM_CONCEPT(result, COppugnPackageMaintarResult)
    )

    DEFINE_SUBAGENTS(
        SUBAGENT(GainGetOppugnPackageMaintar, CGainGetOppugnPackageMaintar, "none")
        SUBAGENT(OppugnPackageMaintarInform, CTaskPromptInform, "none")
        SUBAGENT(OppugnPackageMsseageOutService, COppugnPackageMsseageOutService, "none")
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        SUBAGENT(OppugnPackageDealIsOutOfServiceAgency, COppugnPackageDealIsOutOfServiceAgency, "none")
    )
)

// 调用MaintarOutService接口获取主资费套餐
DEFINE_GAIN_CONCEPT_AGENT( CGainGetOppugnPackageMaintar,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetMaintarInfo_ProdPrcName)
)

// 调用MsseageOutService接口发送短信
DEFINE_GAIN_CONCEPT_AGENT( COppugnPackageMsseageOutService,
    CONCEPT_NAME(send_sms)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::MsseageOutService)
)

// 判断是否停机的情况
DEFINE_AGENCY( COppugnPackageDealIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnPackageNotOutOfServiceAgency, COppugnPackageNotOutOfServiceAgency, "none")
        SUBAGENT(OppugnPackageOutOfServiceAgency, COppugnPackageOutOfServiceAgency, "none")
    )
)

// 未停机
DEFINE_AGENCY( COppugnPackageNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 已停机
DEFINE_AGENCY( COppugnPackageOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipOutOfServiceJudge, CSkipOutOfServiceJudge, "none")
    )
)

void RegisterOppugnPackage() {
    DECLARE_AGENT(COppugnPackageAgency)
    DECLARE_AGENT(CGainGetOppugnPackageMaintar)
    DECLARE_AGENT(COppugnPackageMsseageOutService)
    DECLARE_AGENT(COppugnPackageDealIsOutOfServiceAgency)
    DECLARE_AGENT(COppugnPackageNotOutOfServiceAgency)
    DECLARE_AGENT(COppugnPackageOutOfServiceAgency)
}

}
