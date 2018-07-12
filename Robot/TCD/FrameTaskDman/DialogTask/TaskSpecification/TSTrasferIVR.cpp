#include "TSTrasferIVR.h"

#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

DEFINE_AGENCY( CTrasferIVRAgency,
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    IS_MAIN_TOPIC()
    PRECONDITION( false )
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        SUBAGENT( RequestTrasferIVRFloor, CRequestTrasferIVRFloor, "none")
    )
)

DEFINE_REQUEST_AGENT( CRequestTrasferIVRFloor,
    PROMPT("template RequestTrasferIVRGroundFloor")
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

DEFINE_TASK_SKIP_AGENT( CSkipTrasferIVR,
    DIALOG_TYPE( CTrasferIVRAgency )
    DIALOG_NAME( TrasferIVR )
)

DEFINE_REQUEST_AGENT( CRequestTrasferIVRRecharge,
    PROMPT("template RequestTrasferIVRRecharge")
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

DEFINE_TASK_SKIP_AGENT( CSkipTrasferIVRRecharge,
    DIALOG_TYPE( CRequestTrasferIVRRecharge )
    DIALOG_NAME( RequestTrasferIVRRecharge )
)

DEFINE_REQUEST_AGENT( CRequestTrasferIVRWideBandPasswordModify,
    PROMPT("template RequestTrasferIVRWideBandPasswordModify")
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

DEFINE_TASK_SKIP_AGENT( CSkipTrasferIVRWideBandPasswordModify,
    DIALOG_TYPE( CRequestTrasferIVRWideBandPasswordModify )
    DIALOG_NAME( RequestTrasferIVRWideBandPasswordModify )
)

DEFINE_REQUEST_AGENT( CRequestTrasferIVRRecuperate,
    PROMPT("template RequestTrasferIVRRecuperate")
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

DEFINE_TASK_SKIP_AGENT( CSkipTrasferIVRRecuperate,
    DIALOG_TYPE( CRequestTrasferIVRRecuperate )
    DIALOG_NAME( RequestTrasferIVRRecuperate )
)

void RegisterTrasferIVR() {
    DECLARE_AGENT( CTrasferIVRAgency)
    DECLARE_AGENT( CRequestTrasferIVRFloor)
    DECLARE_AGENT( CSkipTrasferIVR)
    DECLARE_AGENT( CRequestTrasferIVRRecharge)
    DECLARE_AGENT( CSkipTrasferIVRRecharge)
    DECLARE_AGENT( CRequestTrasferIVRWideBandPasswordModify)
    DECLARE_AGENT( CSkipTrasferIVRWideBandPasswordModify)
    DECLARE_AGENT( CRequestTrasferIVRRecuperate)
    DECLARE_AGENT( CSkipTrasferIVRRecuperate)
}

}
