#include "TSTrasferArtificialConfirm.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {
//人工服务节点
DEFINE_AGENCY( CTrasferArtificialConfirmAgency,
    IS_MAIN_TOPIC()
    ERROR_HANDLE( SECOND_SKIP_NT_MAIN_3 )
    DEFINE_CONCEPTS(
        //是否开通
        BOOL_SYSTEM_CONCEPT( is_need )
        //是否为夜间服务
        BOOL_SYSTEM_CONCEPT( is_night_service )
    )
    DEFINE_SUBAGENTS(
        //获取夜间服务状态：[是：否]
        SUBAGENT( GainIsNightService, CGainIsNightService, "none")
        //处理夜间服务状态
        SUBAGENT( DealTrasferArtificialIsNightServiceAgency, CDealTrasferArtificialIsNightServiceAgency, "none")
    )
)

//获取夜间服务状态：[是：否]
DEFINE_GAIN_CONCEPT_AGENT( CGainIsNightService,
    CONCEPT_NAME( is_night_service )
    GAIN_CONCEPT_FUNC(&CGainConceptApi::IsNight)
)

//处理夜间服务状态
DEFINE_AGENCY( CDealTrasferArtificialIsNightServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        //是夜间服务
        SUBAGENT( NightServiceAgency, CNightServiceAgency, "none")
        //不是夜间服务
        SUBAGENT( NotNightServiceAgency, CNotNightServiceAgency, "none")
    )
)

//不是夜间服务
DEFINE_AGENCY( CNotNightServiceAgency,
    PRECONDITION(
        C("is_night_service").IsUpdated() && (bool)C("is_night_service") == false
    )
    DEFINE_SUBAGENTS(
        //跳转到人工服务节点
        SUBAGENT( TrasferArtificialAgency, CRealTrasferArtificialAgency, "none")
    )
)

//是夜间服务
DEFINE_AGENCY( CNightServiceAgency,
    PRECONDITION(
        C("is_night_service").IsUpdated() && (bool)C("is_night_service")
    )
   DEFINE_SUBAGENTS(
        //语音播报：nt_main_zrg02
        SUBAGENT( NightServiceInform, CTaskPromptInform, "none")
        //跳转到人工服务节点
        SUBAGENT( TrasferArtificialAgency, CRealTrasferArtificialAgency, "none")
    )
)

//真正转到人工服务节点
DEFINE_AGENCY( CRealTrasferArtificialAgency,    
    DEFINE_SUBAGENTS(
        //请求用户输入
        SUBAGENT( RequestConfirmAgency, CRequestConfirmAgency, "none")
        //处理用户输入
        SUBAGENT( DealTrasferArtificialConfirm, CDealTrasferArtificialConfirm, "none")
    )
)

//处理用户输入
DEFINE_AGENCY( CDealTrasferArtificialConfirm,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        //请求人工服务
        SUBAGENT( RequestTrasferArtificial, CRequestTrasferArtificial, "none")
        //如果无法识别，判断是否为其他业务
        SUBAGENT( TriggerSpecificBusinessAgency, CTriggerSpecificBusinessAgency, "none")
    )
)

//请求人工服务
DEFINE_REQUEST_AGENT( CRequestTrasferArtificial,
    PRECONDITION(
        C("is_need").IsUpdated() && (bool)C("is_need")
    )
    PROMPT( "template RequestTrasferArtificial" )
    public:
        virtual int Outaction() {
            if ( Config::Instance() != NULL ) {
                return Config::Instance()->GetIntValue("session/field_definition/outaction_skip");
            } else {
                VLOG(FATAL) << __FUNCTION__ << ", Config::Instance is NULL.";
                return 0;
            }
        }
)

//跳过人工确认
DEFINE_TASK_SKIP_AGENT( CSkipTrasferArtificialConfirm,
    DIALOG_TYPE( CTrasferArtificialConfirmAgency )
    DIALOG_NAME( TrasferArtificialConfirm )
)

//声明服务
void RegisterTrasferArtificialConfirm() {
    DECLARE_AGENT( CTrasferArtificialConfirmAgency)
    DECLARE_AGENT( CGainIsNightService)
    DECLARE_AGENT( CDealTrasferArtificialIsNightServiceAgency)
    DECLARE_AGENT( CNightServiceAgency)
    DECLARE_AGENT( CNotNightServiceAgency)
    DECLARE_AGENT( CRealTrasferArtificialAgency)
    DECLARE_AGENT( CDealTrasferArtificialConfirm)
    DECLARE_AGENT( CRequestTrasferArtificial)
    DECLARE_AGENT( CSkipTrasferArtificialConfirm)
}

}
