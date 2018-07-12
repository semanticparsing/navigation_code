#include "TSBalanceInquiry.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {
DEFINE_FRAME_CONCEPT_TYPE( CBalance,
    ITEMS(
        STRING_ITEM(remain_fee)
    )
)
/** 
* @brief 话费查询
*/
DEFINE_AGENCY( CBalanceInquiryAgency,
    PRECONDITION(false)
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION 
    /** 
    * @brief 定义变量：余额，是否欠费
    */
    DEFINE_CONCEPTS(
        CUSTOM_SYSTEM_CONCEPT(result, CBalance)
        BOOL_SYSTEM_CONCEPT(is_arrearage)
    )

    DEFINE_SUBAGENTS(
        //播放指定欢迎语，具体内容在nlp_prompt.templ中进行配置
        SUBAGENT(BalanceInquiryInform, CTaskPromptInform, "none") 

        //获取用余额
        SUBAGENT(GainBalance, CGainBalance, "none")

        //根据余额设置用户欠费状态信息
        SUBAGENT(SetIsArrearage, CSetIsArrearage, "none")

        //根据用户欠费状态进行后续处理
        SUBAGENT(DealIsArrearageAgency, CDealIsArrearageAgency, "none")
    )
)

/** 
* @brief 取得余额
*/
DEFINE_GAIN_CONCEPT_AGENT( CGainBalance,
    //设置结果存储位置
    CONCEPT_NAME(result)

    //调用指定func，将func的结果存储到前面指定的变量中
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetBalance)
)

DEFINE_OPERATE_AGENT( CSetIsArrearage,
    OPERATE(
        if (C("result")["remain_fee"].IsUpdated())
        {
            string sBalance = (string)C("result")["remain_fee"];
            int nBalance = stoi(sBalance);
            if (nBalance >= 0)
            {
                C("is_arrearage") = false;
            }
            else
            {
                C("is_arrearage") = true;
                C("result")["remain_fee"] = to_string(abs(nBalance));
            }
        }        
    )
)

/** 
* @brief 处理是否欠费
*/
DEFINE_AGENCY( CDealIsArrearageAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT(NotArrearageAgency, CNotArrearageAgency, "none")
        SUBAGENT(ArrearageAgency, CArrearageAgency, "none")
    )
)

/** 
* @brief 未欠费
*/
DEFINE_AGENCY( CNotArrearageAgency,
    PRECONDITION(
        C("is_arrearage").IsUpdated() && (bool)C("is_arrearage") == false
    )
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_out_of_service )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( NotArrearageBalanceBroadcast, CTaskPromptInform, "none" )
        SUBAGENT( SendSMSInform, CTaskPromptInform, "none" )
        SUBAGENT( NotArrearageExecuteSendSMS, CExecuteSendSMS, "none" )
        SUBAGENT( GainIsOutOfService, CGainIsOutOfService, "none" )
        SUBAGENT( DealNotArrearageIsOutOfServiceAgency, CDealNotArrearageIsOutOfServiceAgency, "none" )
    )
)

/** 
* @brief 未欠费：处理停机状态
*/
DEFINE_AGENCY( CDealNotArrearageIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT( NotArrearageNotOutOfServiceAgency, CNotArrearageNotOutOfServiceAgency, "none")
        SUBAGENT( OutOfServiceAgency, COutOfServiceAgency, "none")
    )
)

/** 
* @brief 未欠费：没有停机
*/
DEFINE_AGENCY( CNotArrearageNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SkipNtMain_3, CSkipNtMain_3, "none")
    )
)

/** 
* @brief 欠费
*/
DEFINE_AGENCY( CArrearageAgency,
    PRECONDITION(
        C("is_arrearage").IsUpdated() && (bool)C("is_arrearage") == true
    )
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_out_of_service )
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( ArrearageBalanceBroadcast, CTaskPromptInform, "none" )
        SUBAGENT( SendSMSInform, CTaskPromptInform, "none" )
        SUBAGENT( ArrearageExecuteSendSMS, CExecuteSendSMS, "none" )
        SUBAGENT( GainIsOutOfService, CGainIsOutOfService, "none" )
        SUBAGENT( DealArrearageIsOutOfServiceAgency, CDealArrearageIsOutOfServiceAgency, "none" )
    )
)

/** 
* @brief 欠费：处理停机状态
*/
DEFINE_AGENCY( CDealArrearageIsOutOfServiceAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        SUBAGENT( ArrearageNotOutOfServiceAgency, CArrearageNotOutOfServiceAgency, "none")
        SUBAGENT( OutOfServiceAgency, COutOfServiceAgency, "none")
    )
)

/** 
* @brief 欠费：未停机
*/
DEFINE_AGENCY( CArrearageNotOutOfServiceAgency,
    PRECONDITION(
        C("is_out_of_service").IsUpdated() && (bool)C("is_out_of_service") == false
    )
    DEFINE_SUBAGENTS(
        SUBAGENT( SkipNtMain_4, CSkipNtMain_4, "none")
    )
)

/** 
* @brief 节点声明
*/
void RegisterBalanceInquiry() {
    DECLARE_AGENT( CBalanceInquiryAgency )
    DECLARE_AGENT( CGainBalance )
    DECLARE_AGENT( CSetIsArrearage )
    DECLARE_AGENT( CDealIsArrearageAgency )
    DECLARE_AGENT( CNotArrearageAgency )
    DECLARE_AGENT( CDealNotArrearageIsOutOfServiceAgency )
    DECLARE_AGENT( CNotArrearageNotOutOfServiceAgency )
    DECLARE_AGENT( CArrearageAgency)
    DECLARE_AGENT( CDealArrearageIsOutOfServiceAgency )
    DECLARE_AGENT( CArrearageNotOutOfServiceAgency )
}

}
