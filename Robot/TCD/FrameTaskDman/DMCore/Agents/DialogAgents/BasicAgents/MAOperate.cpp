#include "MAOperate.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

namespace TrioTDM {

CMAOperate::CMAOperate(const std::string& name, const std::string& configuration, const std::string& type) :
    CDialogAgent(name, configuration, type) {
        ;
}

CMAOperate::~CMAOperate() {
    ;
}

CAgent* CMAOperate::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CMAOperate(name, configuration);
}

TDialogExecuteReturnCode CMAOperate::Execute(COutput* output) {
    IncrementExecuteCounter();
    Operate();

    return dercContinueExecution;
}

bool CMAOperate::SuccessCriteriaSatisfied() {
    return (_execute_counter > 0 );
}

void CMAOperate::Operate() {
    ;
}

}
