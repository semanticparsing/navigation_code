#include "TSOtherMean.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {
//主菜单集外说法
DEFINE_AGENCY( COtherMeanAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()

    DEFINE_SUBAGENTS(
        //友好问候语音节点
        SUBAGENT( FriendlyGreetings, CFriendlyGreetingsAgency, "none")
        //感谢语音节点
        SUBAGENT( Thanks, CThanksAgency, "none")
        //结束语音节点
        SUBAGENT( Finish, CFinishAgency, "none")
        //机器人/疑问节点
        SUBAGENT( Doubt, CDoubtAgency, "none")
        //业务咨询/集外节点
        SUBAGENT( NotActivateBusiness, CNotActivateBusinessAgency, "none")
        //批评/抱怨/脏话节点
        SUBAGENT( Tiresome, CTiresomeAgency, "none")
        //投诉节点
        SUBAGENT( Complaint, CComplaintAgency, "none")
        //简短词拒识节点
        SUBAGENT( ShortRejection, CShortRejectionAgency, "none")
    )
)

//可打断通知
DEFINE_REQUEST_AGENT( CInterruptInform,
    IS_NOT_DTS_AGENT()
    public:
        virtual std::string Prompt() {
            if ( _da_parent != NULL ) {
                return "template " +  _da_parent->GetDialogAgentName() + "." + _dialog_agent_name;
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", _da_parent is NULL, AgentName: " << _name;
                return "";
            }
        }
)

//不可打断通知
DEFINE_INFORM_AGENT( CInterruptContinueInform,
    IS_NOT_DTS_AGENT()
    public:
        virtual std::string Prompt() {
            if ( _da_parent != NULL ) {
                return "template " +  _da_parent->GetDialogAgentName() + "." + _dialog_agent_name;
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", _da_parent is NULL, AgentName: " << _name;
                return "";
            }
        }
)

//友好问候节点
DEFINE_AGENCY( CFriendlyGreetingsAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
    //打断通知
        SUBAGENT( InterruptInform, CInterruptInform, "none")
    )
)

//感谢节点
DEFINE_AGENCY( CThanksAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        //打断通知
        SUBAGENT( InterruptInform, CInterruptInform, "none")
    )
)

//结束语音节点
DEFINE_AGENCY( CFinishAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        SUBAGENT( InterruptInform, CInterruptContinueInform, "none")
        SUBAGENT( HangUp, CInterruptInform, "none")
    )
)

//疑问节点
DEFINE_AGENCY( CDoubtAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        //中断节点
        SUBAGENT( InterruptInform, CInterruptInform, "none")
    )
)

//业务咨询/集外节点
DEFINE_AGENCY( CNotActivateBusinessAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    RESET_ON_COMPLETION
    
    /*
     * 变量声明：
     * is_out_of_service：判断是否停机
    */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT(is_out_of_service)
    )

    DEFINE_SUBAGENTS(
        //中断节点
        SUBAGENT( InterruptInform, CInterruptContinueInform, "none")
        //发送短信：nt_main_noservice_sms
        SUBAGENT(ExecuteSendSMS, CNotActivateBusinessExecuteSendSMS, "none")
        //获取是否停机状态值：状态[是：否]
        SUBAGENT(GainIsOutOfService, CGainIsOutOfService, "none")
        //如果为停机，进入停机判断
        SUBAGENT(OtherMeanOutOfServiceAgency, COtherMeanDealIsOutOfSeverice, "none")
    )
)

// 停机判断结果处理
DEFINE_AGENCY(COtherMeanDealIsOutOfSeverice,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(OtherMeanDealNotOutOfSeverice,COtherMeanDealNotOutOfSeverice,"none")
        SUBAGENT(OtherMeanDealOutOfSeverice,COtherMeanDealOutOfSeverice,"none")
    )
)

// 停机处理
DEFINE_AGENCY(COtherMeanDealOutOfSeverice,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == true
    )
    DEFINE_SUBAGENTS(
        // 进入停机判断（共通函数，需要再确认）
        SUBAGENT( OutOfServiceAgency, COutOfServiceAgency, "none")
    )
)

/* 未停机状态 */
DEFINE_AGENCY(COtherMeanDealNotOutOfSeverice,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

/* 停机状态 */
DEFINE_AGENCY( COtherMeanOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service")
    )
    DEFINE_SUBAGENTS(
        /* 跳转到停机判断 */
        SUBAGENT(SkipOutOfServiceJudge, CSkipOutOfServiceJudge, "none")
    )
)

//批评/抱怨/脏话节点
DEFINE_AGENCY( CTiresomeAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        SUBAGENT( InterruptInform, CInterruptContinueInform, "none")
        //返回IVR首层
        SUBAGENT(RequestTrasferIVRFloor, CRequestTrasferIVRFloor, "none")
    )
)

//投诉节点    
DEFINE_AGENCY( CComplaintAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        SUBAGENT( InterruptInform, CInterruptContinueInform, "none")
        //返回IVR首层
        SUBAGENT(RequestTrasferIVRFloor, CRequestTrasferIVRFloor, "none")
    )
)

//简短词拒识节点
DEFINE_AGENCY( CShortRejectionAgency,
    PRECONDITION( false )
    IS_NOT_DTS_AGENT()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        //中断节点
        SUBAGENT( InterruptInform, CInterruptInform, "none")
    )
)

//声明节点
void RegisterOtherMean() {
    DECLARE_AGENT( COtherMeanAgency)
    DECLARE_AGENT( CInterruptInform)
    DECLARE_AGENT( CInterruptContinueInform)
    DECLARE_AGENT( CFriendlyGreetingsAgency)
    DECLARE_AGENT( CThanksAgency)
    DECLARE_AGENT( CFinishAgency)
    DECLARE_AGENT( CDoubtAgency)
    DECLARE_AGENT( CNotActivateBusinessAgency)
    DECLARE_AGENT( CTiresomeAgency)
    DECLARE_AGENT( CComplaintAgency)
    DECLARE_AGENT( CShortRejectionAgency)
    DECLARE_AGENT( COtherMeanOutOfServiceAgency)
    DECLARE_AGENT( COtherMeanDealIsOutOfSeverice)
    DECLARE_AGENT( COtherMeanDealOutOfSeverice)
    DECLARE_AGENT( COtherMeanDealNotOutOfSeverice)
}

}
