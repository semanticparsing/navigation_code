#include "GAAccept.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"

namespace TrioTDM {

CGAAccept::CGAAccept(const std::string& nw_configuration) :
    CGroundingAction(nw_configuration) {
        ;
}

std::string CGAAccept::GetName() {
    return "ACCEPT";
}

void CGAAccept::Run(void* params) {
    if ( ((CConcept*)params)->IsUpdated() ) {
        ((CConcept*)params)->SetGroundedFlag(true);
    }

    if ( CGroundingManagerAgent::Instance() != NULL && CDMCoreAgent::Instance() != NULL ) {
        CGroundingManagerAgent::Instance()->SetConceptGroundingRequestStatus((CConcept*)params, GRS_EXECUTING);
        CDMCoreAgent::Instance()->SignalAcceptOnConcept((CConcept*)params);
        CGroundingManagerAgent::Instance()->ConceptGroundingRequestCompleted((CConcept*)params);
    }
}

void CGAAccept::RegisterDialogAgency() {
}

}
