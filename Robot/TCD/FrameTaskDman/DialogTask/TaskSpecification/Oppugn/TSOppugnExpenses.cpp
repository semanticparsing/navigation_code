#include "TSOppugnExpenses.h"
#include "../TSCommonAgent.h"
#include "../TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

// 主资费套餐接口MaintarOutService返回结果
DEFINE_FRAME_CONCEPT_TYPE( COppugnExpensesMaintarResult,
    ITEMS(
        STRING_ITEM(prod_prc_name)  // 主资费套餐
    )
)

// 入口节点
DEFINE_AGENCY( COppugnExpensesAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(send_sms)
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
        CUSTOM_SYSTEM_CONCEPT(result, COppugnExpensesMaintarResult)
    )

    DEFINE_SUBAGENTS(
        SUBAGENT(GainGetOppugnExpensesMaintar, CGainGetOppugnExpensesMaintar, "none")
        SUBAGENT(OppugnExpensesMaintarInform, CTaskPromptInform, "none")
        SUBAGENT(OppugnExpensesMsseageOutService, COppugnExpensesMsseageOutService, "none")
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        SUBAGENT(OppugnExpensesDealIsOutOfServiceAgency, COppugnExpensesDealIsOutOfServiceAgency, "none")
    )
)

// 调用MaintarOutService接口获取主资费套餐
DEFINE_GAIN_CONCEPT_AGENT( CGainGetOppugnExpensesMaintar,
    CONCEPT_NAME(result)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetMaintarInfo_ProdPrcName)
)

// 调用MsseageOutService接口发送短信
DEFINE_GAIN_CONCEPT_AGENT( COppugnExpensesMsseageOutService,
    CONCEPT_NAME(send_sms)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::MsseageOutService)
)

// 判断是否停机的情况
DEFINE_AGENCY( COppugnExpensesDealIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OppugnExpensesNotOutOfServiceAgency, COppugnExpensesNotOutOfServiceAgency, "none")
        SUBAGENT(OppugnExpensesOutOfServiceAgency, COppugnExpensesOutOfServiceAgency, "none")
    )
)

// 未停机
DEFINE_AGENCY( COppugnExpensesNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

// 已停机
DEFINE_AGENCY( COppugnExpensesOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipOutOfServiceJudge, CSkipOutOfServiceJudge, "none")
    )
)

void RegisterOppugnExpenses() {
    DECLARE_AGENT(COppugnExpensesAgency)
    DECLARE_AGENT(CGainGetOppugnExpensesMaintar)
    DECLARE_AGENT(COppugnExpensesMsseageOutService)
    DECLARE_AGENT(COppugnExpensesDealIsOutOfServiceAgency)
    DECLARE_AGENT(COppugnExpensesNotOutOfServiceAgency)
    DECLARE_AGENT(COppugnExpensesOutOfServiceAgency)
}

}
