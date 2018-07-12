// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-16 20:06
// Description: 

#ifndef _ALL_GROUNDING_MODELS_H__
#define _ALL_GROUNDING_MODELS_H__

#include "GroundingModel.h"
#include "GMConcept.h"
#include "GMRequestAgent.h"

namespace TrioTMD {

#define ALL_GROUNDING_MODEL_TYPES \
    GROUNDING_MODEL_TYPE(request_default, TrioTDM::CGMRequestAgent::GroundingModelFactory) \
    GROUNDING_MODEL_TYPE(concept_default, TrioTDM::CGMConcept::GroundingModelFactory) \


}

#endif
