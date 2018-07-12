// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-04 17:39
// Description: 

#ifndef _GA_ACCEPT_H__
#define _GA_ACCEPT_H__

#include "GroundingAction.h"

namespace TrioTDM {

class CGAAccept : public CGroundingAction {

public:
    CGAAccept(const std::string& nw_configuration);
    virtual std::string GetName();
    virtual void Run(void* params);
    virtual void RegisterDialogAgency();

};

#define ACCEPT CGAAccept

}

#endif
