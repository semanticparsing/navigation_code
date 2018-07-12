#include "MAGainConcept.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Common/Util/File/simpleLineReader.h"

namespace TrioTDM {

/*
static std::map<std::string, std::string> GetKeyValue() {
    std::map<std::string, std::string> tmp;
        file::SimpleLineReader reader;

        if ( !reader.Open("../Data/api_fake.data") ) {
            VLOG(FATAL) << "Can not read. file: " << "../Data/api_fake.data";
        } else {
            std::vector<std::string> lines;
            std::string key;
            std::string value;

            reader.ReadLines(&lines);

            for ( size_t i = 0;i < lines.size(); ++i ) {
                if ( SplitOnFirst(lines[i], "\t", key, value)) {
                    tmp.insert(std::make_pair(key, value));
                }
            }
        }
    return tmp;
}

std::map<std::string, std::string> CMAGainConcept::s_key2value = GetKeyValue();
*/
CMAGainConcept::CMAGainConcept(const std::string& name, const std::string& configuration, const std::string& type) :
    CDialogAgent(name, configuration, type) {
    ;
}

CMAGainConcept::~CMAGainConcept() {
    ;
}

CAgent* CMAGainConcept::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CMAGainConcept(name, configuration);
}

TDialogExecuteReturnCode CMAGainConcept::Execute(COutput* output) {
    VLOG(DEBUG) << "CMAGainConcept::Execute, AgentName: " << _name;
    IncrementExecuteCounter();
    if ( _owner_core_agent == NULL || _owner_core_agent->GetGainConceptApi() == NULL ) {
        VLOG(FATAL) << "CMAGainConcept::Execute, Can not get GainConceptApi. _owner_core_agent == NULL: "\
            << (_owner_core_agent == NULL) << "\tAgentName: " << _name;
        return dercError;
    }

    if ( GetGainConceptFunc() != NULL ) {
        if ( !( (_owner_core_agent->GetGainConceptApi()->*GetGainConceptFunc())(C(ConceptName())) ) ) {
            VLOG(WARNING) << "CMAGainConcept::Execute, system error.";
            return dercError;
        }
    } else if ( GetGainConceptParamFunc() != NULL ) {
        if ( !( (_owner_core_agent->GetGainConceptApi()->*GetGainConceptParamFunc())(GetExecuteParam(), C(ConceptName())) ) ) {
            VLOG(WARNING) << "CMAGainConcept::Execute, system error.";
            return dercError;
        }
    } else {
        VLOG(FATAL) << "CMAGainConcept::Execute, GainConceptFunc is NULL.";
        return dercError;
    }

    return dercContinueExecution;
}

bool CMAGainConcept::SuccessCriteriaSatisfied() {
    return C(ConceptName()).IsUpdated();
}

std::string CMAGainConcept::ConceptName() {
    return "";
}

std::string CMAGainConcept::ParamName() {
    return "";
}

CConcept& CMAGainConcept::GetExecuteParam() {
    return C(ParamName());
}

GainConceptFunc CMAGainConcept::GetGainConceptFunc() {
    return NULL;
}

GainConceptParamFunc CMAGainConcept::GetGainConceptParamFunc() {
    return NULL;
}

}
