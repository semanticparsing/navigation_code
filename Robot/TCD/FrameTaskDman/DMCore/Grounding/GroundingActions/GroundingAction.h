// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-16 10:11
// Description: 

#ifndef _GROUNDING_ACTION_H__
#define _GROUNDING_ACTION_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"

namespace TrioTDM {

class CGroundingAction {

public:
    CGroundingAction() {};
    CGroundingAction(const std::string& new_configuration);
    virtual ~CGroundingAction() {};

    virtual std::string GetName() = 0;
    void SetConfiguration(const std::string& new_configuration);
    std::string GetConfiguration();

    virtual void Run(void* params = NULL) = 0;
    virtual void RegisterDialogAgency() = 0;

protected:
    std::map<std::string, std::string> _configuration;
};

}

#endif
