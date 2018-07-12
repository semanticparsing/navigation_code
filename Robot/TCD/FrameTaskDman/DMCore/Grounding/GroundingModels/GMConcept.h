// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-16 20:18
// Description: 

#ifndef _GM_CONCEPT_H__
#define _GM_CONCEPT_H__

#include "GroundingModel.h"

#define SI_INACTIVE 0
#define SS_INACTIVE "INACTIVE"

#define SI_CONFIDENT 1
#define SS_CONFIDENT "CONFIDENT"

#define SI_UNCONFIDENT 2
#define SS_UNCONFIDENT "UNCONFIDENT"

#define SI_GROUNDED 3
#define SS_GROUNDED "GROUNDED"

namespace TrioTDM {

class CConcept;

class CGMConcept : public CGroundingModel {

public:
    CGMConcept(const std::string& model_policy = "", const std::string& name = "UNKNOWN");
    CGMConcept(CGMConcept& concept);
    virtual ~CGMConcept();

    static CGroundingModel* GroundingModelFactory(const std::string& model_policy);
    virtual std::string GetType();
    virtual void SetName(const std::string& name);
    virtual std::string GetName();

    void SetConcept(CConcept* concept);
    CConcept* GetConcept();

    virtual CGroundingModel* Clone();
    virtual bool LoadPolicy();

    virtual void RunAction(int action_index);
    virtual void LogStateAction();

protected:
    virtual void ComputeFullState();
    virtual void ComputeBeliefState();

protected:
    CConcept* _concept;
};

}

#endif
