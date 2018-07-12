// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-16 10:27
// Description: 

#ifndef _GA_ASK_REPEAT_H__
#define _GA_ASK_REPEAT_H__

#include "GroundingAction.h"

namespace TrioTDM {

class CGAAskRepeat: public CGroundingAction {

public:
    CGAAskRepeat(const std::string& new_configuration);

    virtual std::string GetName();
    virtual void Run(void *params);
    virtual void RegisterDialogAgency();
};

#define ASK_REPEAT CGAAskRepeat

}

#endif
