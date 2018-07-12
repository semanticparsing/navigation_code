#include "GroundingAction.h"

namespace TrioTDM {

CGroundingAction::CGroundingAction(const std::string& new_configuration) {
    SetConfiguration(new_configuration);
}

void CGroundingAction::SetConfiguration(const std::string& new_configuration) {
    _configuration = StringToS2SHash(new_configuration);
}

}
