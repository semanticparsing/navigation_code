#include "TaskFlowRoot.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

DEFINE_AGENCY( CTaskFlowRoot,
    IS_MAIN_TOPIC()
    DEFINE_CONCEPTS(
        INT_SYSTEM_CONCEPT( nt_main )
       
        // 查询、办理、咨询、通用字段、集外说法、拒识
        STRING_USER_CONCEPT( action, "none" )
        // 查询     余额/账单/本机业务/月初扣费/本机号码/套餐余量/充值缴费记录/积分
        // 办理     密码/宽带/手机充值/套餐/流量/GPRS
        // 咨询     宽带/套餐
        // 通用字段 是/否/转人工/重听/帮助/返回/短信下发
        // 集外说法 友好问候/感谢/结束语/机器人/批评，抱怨，脏话/集外业务咨询
        // 拒识     1
        STRING_USER_CONCEPT( target, "none" )
        // 月份
        // 1~12
        //STRING_USER_CONCEPT( month, "none" )
        // 金额
        //STRING_USER_CONCEPT( money, "none" )
        // 流量单位 
        // MB/GB
        //STRING_USER_CONCEPT( data_traffic, "none" )

        // 实体标签
        // 流量/语音/短信
        // 充值卡/银行卡
        // 手机/宽带
        // 充值卡/银行卡
        STRING_USER_CONCEPT( entity_tag, "none" )

        // 取代 流量业务操作类型 密码服务操作类型 
        // 开通/取消/修改/重置
        // 咨询/预约
        //STRING_USER_CONCEPT( action_tag, "none" )
        // 流量业务对象类型
        // 基础流量包/夜间流量包/周末流量包/10元流量包/20元流量包/加油包/小时流量包/日流量包
        //STRING_USER_CONCEPT( flow_business_object_type, "none" )

        // 宽带类型
        // 单品宽带/4G融合宽带/群组宽带
        //STRING_USER_CONCEPT( broadband_type, "none" )

        STRING_SYSTEM_CONCEPT( error_handle )
        // 证件验证错误次数
        INT_SYSTEM_CONCEPT( certificate_verification_error_time )
    )
    DEFINE_SUBAGENTS(
        // 业务以触发优先级 排序， TenYuanNightTrafficPackOpen 必须排到  TrafficOpen 前
        // 即 触发条件里 A 的触发条件包含 B 的触发条件， 则 A 必须排到 B 之前。
        // 有<UNDEFINED> 的要定义在前边

        // ***** START *****
        // 无触发条件
        SUBAGENT( Welcome, CWelcomeAgency, "none")

        // 有<UNDEFINED>的触发 <UNDEFINED> 数量多的优先放到前边
        // sd012密码服务 [action]=<UNDEFINED>|[target]=<UNDEFINED>|[entity_tag]=密码
        //SUBAGENT( PasswordService, CPasswordServiceAgency, "none")

        // ***** 触发条件数：5 *****

        // ***** 触发条件数：4 *****

        // ***** 触发条件数：3 *****

        // jl006手机密码修改
        SUBAGENT( TelephonePasswordModify, CTelephonePasswordModifyAgency, "none")
        // jl007宽带密码修改
        SUBAGENT( ModifyPasswordOfWideBand, CModifyPasswordOfWideBand, "none")

        // ***** 触发条件数：2 *****
        // 回到主菜单
        SUBAGENT( ReturnPrimaryMenu, CReturnPrimaryMenuAgency, "none")
        SUBAGENT( Quit, CQuitAgency, "none")
        // 业务列表播报
        SUBAGENT( BusinessesBroadcast, CRequestBusinessesBroadcast, "none")
        // sd001余额查询
        SUBAGENT( BalanceInquiry, CBalanceInquiryAgency, "none")
        // jl002剩余通话分钟数查询
        SUBAGENT( RemainingTalkInquiry, CRemainingTalkInquiryAgency, "none")
        // jl003流量剩余查询
        SUBAGENT( RemainingTrafficInquiry, CRemainingTrafficInquiryAgency, "none")
        // jl004手机密码重置
        SUBAGENT( TelephonePasswordReset, CTelephonePasswordResetAgency, "none")
        // jl005密码修改服务
        SUBAGENT( ModifyPassword, CModifyPassword, "none")
        // jl008已开业务查询
        SUBAGENT( OpenedBusinessInquiry, COpenedBusinessInquiryAgency, "none")
        // jl011充值缴费
        SUBAGENT( RechargeBill, CRechargeBill, "none")
        // jl012复机
        SUBAGENT( Recuperate, CRecuperate, "none")

        //未上线业务
        SUBAGENT( NonOnlineBusiness, CNonOnlineBusinessAgency, "none")
        //jl009已开业务-套餐资费类--打电话怎么收费
        SUBAGENT( OppugnCallCharge, COppugnCallChargeAgency, "none")
        //jl009已开业务-套餐资费类--卡怎么收费
        SUBAGENT( OppugnExpenses, COppugnExpensesAgency, "none")
        //jl009已开业务-套餐资费类--固定收费都有啥
        SUBAGENT( OppugnFixedFee, COppugnFixedFeeAgency, "none")
        //jl009已开业务-套餐资费类--最低消费多少钱
        SUBAGENT( OppugnLowestSpend, COppugnLowestSpendAgency, "none")
        //jl009已开业务-套餐资费类--月租费多少钱
        SUBAGENT( OppugnMonthlyRent, COppugnMonthlyRentAgency, "none")
        //jl009已开业务-套餐资费类--上网怎么收费
        SUBAGENT( OppugnNetCharge, COppugnNetChargeAgency, "none")
        //jl009已开业务-套餐资费类--套餐里都有啥
        SUBAGENT( OppugnPackage, COppugnPackageAgency, "none")
        //jl010已开业务-质疑类
        SUBAGENT( OppugnTrafficFee, COppugnTrafficFeeAgency, "none")
        // 转 IVR
        SUBAGENT( TrasferIVR, CTrasferIVRAgency, "none")
        // zrg转人工确认
        SUBAGENT( TrasferArtificial, CTrasferArtificialAgency, "none")
        // ***** 触发条件数：1 *****

    )
)

#if 0
DEFINE_AGENCY( CExpectConceptAgency,
    DEFINE_SUBAGENTS(
        // SUBAGENT( ExpectAction, CExpectAction, "none")
        // SUBAGENT( ExpectTarget, CExpectTarget, "none")
        SUBAGENT( ExpectMonth, CExpectMonth, "none")
        SUBAGENT( ExpectMoney, CExpectMoney, "none")
        SUBAGENT( ExpectDataTraffic, CExpectDataTraffic, "none")
        SUBAGENT( ExpectActionTag, CExpectActionTag, "none")
        SUBAGENT( ExpectFlowBusinessObjectType, CExpectFlowBusinessObjectType, "none")
        SUBAGENT( ExpectBroadbandType, CExpectBroadbandType, "none")
    )
)
#endif

DEFINE_EXPECT_AGENT( CExpectAction,
    EXPECT_CONCEPT( action )
    GRAMMAR_MAPPING("*[action]")
)

DEFINE_EXPECT_AGENT( CExpectTarget,
    EXPECT_CONCEPT( target )
    GRAMMAR_MAPPING("*[target]")
)

#if 0
DEFINE_EXPECT_AGENT( CExpectMonth,
    EXPECT_CONCEPT( month )
    GRAMMAR_MAPPING("*[month]")
)

DEFINE_EXPECT_AGENT( CExpectMoney,
    EXPECT_CONCEPT( money )
    GRAMMAR_MAPPING("*[money]")
)

DEFINE_EXPECT_AGENT( CExpectDataTraffic,
    EXPECT_CONCEPT( data_traffic )
    GRAMMAR_MAPPING("*[data_traffic]")
)

DEFINE_EXPECT_AGENT( CExpectActionTag,
    EXPECT_CONCEPT( action_tag )
    GRAMMAR_MAPPING( "*[action_tag]" )
)

DEFINE_EXPECT_AGENT( CExpectFlowBusinessObjectType,
    EXPECT_CONCEPT( flow_business_object_type )
    GRAMMAR_MAPPING("*[flow_business_object_type]")
)

DEFINE_EXPECT_AGENT( CExpectBroadbandType,
    EXPECT_CONCEPT( broadband_type )
    GRAMMAR_MAPPING("*[broadband_type]")
)
#endif

void RegisterTaskFlowRoot() {
    DECLARE_AGENT( CTaskFlowRoot)
    DECLARE_AGENT( CExpectAction)
    DECLARE_AGENT( CExpectTarget )
#if 0
    DECLARE_AGENT( CExpectConceptAgency)
    DECLARE_AGENT( CExpectMonth)
    DECLARE_AGENT( CExpectMoney)
    DECLARE_AGENT( CExpectDataTraffic)
    DECLARE_AGENT( CExpectActionTag)
    DECLARE_AGENT( CExpectFlowBusinessObjectType)
    DECLARE_AGENT( CExpectBroadbandType)
#endif
}

}
