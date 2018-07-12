#include "TSWelcome.h"
#include "TSCommonAgent.h"
#include "TSSendMsg.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {
//欢迎菜单节点
DEFINE_AGENCY( CWelcomeAgency,
    IS_MAIN_TOPIC()
    RESET_ON_COMPLETION

    DEFINE_CONCEPTS(
        //当前时间段  0:(0点到8点]    1:(8点到11点]    2:(11点到14点]    3:(14点到18点]    4:(18点到24点]
        STRING_SYSTEM_CONCEPT( current_time_range )
    )

    DEFINE_SUBAGENTS(
        // SUBAGENT( LogoSound, CTaskPromptInform, "none") 
        //获取时间段
        SUBAGENT( GetTimeRange, CGetTimeRange, "none")
        //语音播报：nt_wel
        SUBAGENT( GreetingAgency, CGreetingAgency, "none")
        //语音播报：nt_main
        SUBAGENT( MainInfo, CTaskPromptInform, "none")
        //跳转到nt-main1
        SUBAGENT( SkipNtMain_1, CSkipNtMain_1, "none")
    )
)

//获取时间段
DEFINE_GAIN_CONCEPT_AGENT( CGetTimeRange,
    CONCEPT_NAME( current_time_range )
    GAIN_CONCEPT_FUNC(&CGainConceptApi::GetCurrentTimeRange)
)

DEFINE_AGENCY( CGreetingAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        //(0点到8点] 
        SUBAGENT( GoodMorningAgency, CGoodMorningAgency, "none")
        //(8点到11点] 
        SUBAGENT( GoodAmAgency, CGoodAmAgency, "none")
        //(11点到14点] 
        SUBAGENT( GoodNoonAgency, CGoodNoonAgency, "none")
        //(14点到18点] 
        SUBAGENT( GoodPmAgency, CGoodPmAgency, "none")
        //(18点到24点] 
        SUBAGENT( GoodEveningAgency, CGoodEveningAgency, "none")

    )
)

//您好
DEFINE_AGENCY( CGoodMorningAgency,
    PRECONDITION(
        C("current_time_range").IsUpdated() && (string)C("current_time_range") == "0"
    )
    DEFINE_SUBAGENTS(
        //nt_wel_morning
        SUBAGENT( GoodMorning, CTaskPromptInform, "none")
    )
)

//上午好
DEFINE_AGENCY( CGoodAmAgency,
    PRECONDITION(
        C("current_time_range").IsUpdated() && (string)C("current_time_range") == "1"
    )
    DEFINE_SUBAGENTS(
        //nt_wel_am
        SUBAGENT( GoodAm, CTaskPromptInform, "none")
    )
)

//中午好
DEFINE_AGENCY( CGoodNoonAgency,
    PRECONDITION(
        C("current_time_range").IsUpdated() && (string)C("current_time_range") == "2"
    )
    DEFINE_SUBAGENTS(
        //nt_wel_noon
        SUBAGENT( GoodNoon, CTaskPromptInform, "none")
    )
)

//下午好
DEFINE_AGENCY( CGoodPmAgency,
    PRECONDITION(
        C("current_time_range").IsUpdated() && (string)C("current_time_range") == "3"
    )
    DEFINE_SUBAGENTS(
        //nt_wel_pm
        SUBAGENT( GoodPm, CTaskPromptInform, "none")
    )
)

//晚上好
DEFINE_AGENCY( CGoodEveningAgency,
    PRECONDITION(
        C("current_time_range").IsUpdated() && (string)C("current_time_range") == "4"
    )
    DEFINE_SUBAGENTS(
        //nt_wel_evening
        SUBAGENT( GoodEvening, CTaskPromptInform, "none")
    )
)

//声明节点
void RegisterWelcome() {
    DECLARE_AGENT(CWelcomeAgency)
    DECLARE_AGENT(CGetTimeRange)
    DECLARE_AGENT(CGreetingAgency)
    DECLARE_AGENT(CGoodMorningAgency)
    DECLARE_AGENT(CGoodAmAgency)
    DECLARE_AGENT(CGoodNoonAgency)
    DECLARE_AGENT(CGoodPmAgency)
    DECLARE_AGENT(CGoodEveningAgency)
}

}
