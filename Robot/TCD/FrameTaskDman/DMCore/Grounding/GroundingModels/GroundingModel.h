// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-09 16:14
// Description: 

#ifndef _GROUNDING_MODEL_H__
#define _GROUNDING_MODEL_H__

#include "../GroundingUtils.h"

#define UNAVAILABLE_ACTION INVALID_EVENT

namespace TrioTDM {

typedef struct {
    std::string sStateName;
    std::map<int, float> i2fActionsValues;
} TStateActionsValues;

class CExternalPolicyInterface {
public:
    CExternalPolicyInterface(const std::string& host);
    ~CExternalPolicyInterface();

    int ComputeSuggestedActionIndex(CState& state);

private:
    unsigned long int socket;
};

class CGroundingModel {

public:
    CGroundingModel(const std::string& model_policy = "", const std::string& model_name = "UNKNOWN");
    CGroundingModel(CGroundingModel& grounding_model);
    virtual ~CGroundingModel();

    static CGroundingModel* GroundingModelFactory(const std::string& model_policy);

    virtual std::string GetType();
    virtual std::string GetModelPolicy();
    virtual std::string GetName();
    virtual void SetName(const std::string& name);

    virtual void Initialize();
    virtual CGroundingModel* Clone() = 0;
    virtual bool LoadPolicy();
    virtual void ComputeState();
    virtual void ComputeActionValuesDistribution();
    virtual int ComputeSuggestedActionIndex();
    virtual void Run();
    virtual void RunAction(int index);
    virtual void LogStateAction();

protected:
    virtual void ComputeFullState() = 0;
    virtual void ComputeBeliefState() = 0;
    std::string BeliefStateToString();
    std::string ActionValuesToString();

protected:
    std::string _name;
    std::string _model_policy;
    std::vector<TStateActionsValues> _policys;
    bool _is_external_policy;
    std::vector<int> _action_mappings;
    std::string _exploration_mode;
    float _exploration_parameter;
    CState _full_state;
    CBeliefDistribution _belief_state;
    CBeliefDistribution _action_values;
    int _suggested_action_index;
};

}

#endif
