#include "AllGroundingModels.h"

namespace TrioTDM {

CGMRequestAgent* TurnGroundingModelFactory(const std::string& type, const std::string& spec) {
    return new CGMRequestAgent(spec);
}

CGMConcept* ConceptGroundingModelFactory(const std::string& type, const std::string& spec) {
    return new CGMConcept(spec);
}

}
