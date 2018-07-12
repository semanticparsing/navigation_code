// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-18 10:13
// Description: 

#ifndef _GM_REQUEST_AGENT_H__
#define _GM_REQUEST_AGENT_H__

#include "GroundingModel.h"

#define SI_FAILED 0
#define SS_FAILED "FAILED"

#define SI_UNDERSTANDING 1
#define SS_UNDERSTANDING "UNDERSTANDING"

#define SI_NONUNDERSTANDING 2
#define SS_NONUNDERSTANDING "NONUNDERSTANDING"

namespace TrioTDM {

class CDialogAgent;

class CGMRequestAgent: public CGroundingModel {

public:
    CGMRequestAgent(const std::string& model_policy = "", const std::string& name = "UNKNOWN");
    CGMRequestAgent(const CGMRequestAgent& request_agent);
    virtual ~CGMRequestAgent();

    static CGroundingModel* GroundingModelFactory(const std::string& model_policy);

    virtual std::string GetType();
    virtual std::string GetName();

    virtual void SetName(const std::string& name);
    void SetRequestAgent(CDialogAgent* dialog_agent);
    CDialogAgent* GetRequestAgent();

    virtual CGroundingModel* Clone();
    virtual bool LoadPolicy();
    virtual void RunAction(int index);

protected:
    virtual void ComputeFullState();
    virtual void ComputeBeliefState();

protected:
    CDialogAgent* _request_agent;

};

}

#endif
