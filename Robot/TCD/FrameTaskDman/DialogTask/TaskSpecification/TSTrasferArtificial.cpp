#include "TSTrasferArtificial.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM{

DEFINE_AGENCY( CTrasferArtificialAgency,
    PRECONDITION( false )
    IS_MAIN_TOPIC()
    CREATE_NOT_BUILD_TREE
    IS_BUSINESS()
    RESET_ON_COMPLETION

    DEFINE_SUBAGENTS(
        SUBAGENT( TrasferArtificialConfirm, CTrasferArtificialConfirmAgency, "none")
    )
)

void RegisterTrasferArtificial() {
    DECLARE_AGENT( CTrasferArtificialAgency)
}

}
