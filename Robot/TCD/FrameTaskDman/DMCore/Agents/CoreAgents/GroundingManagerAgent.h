// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-17 15:00
// Description: 

#ifndef _GROUNDING_MANAGER_AGENT_H__
#define _GROUNDING_MANAGER_AGENT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Agent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Grounding/Grounding.h"

namespace TrioTDM {

typedef struct {
    std::string sGroundingManagerConfiguration;
    bool bGroundConcepts;
    bool bGroundTurns;
    std::string sConceptGM;
    std::string sTurnGM;
    std::string sBeliefUpdatingModelName;
} TGroundingManagerConfiguration;

#define GRS_UNPROCESSED 0
#define GRS_PENDING     1
#define GRS_READY       2
#define GRS_SCHEDULED   3
#define GRS_EXECUTING   5
#define GRS_DONE        6

typedef struct {
    CConcept* pConcept;
    int iGroundingRequestStatus;
    int iSuggestedActionIndex;
    int iTurnNumber;
} TConceptGroundingRequest;

typedef std::vector<TConceptGroundingRequest> TConceptGroundingRequests;

#define GAT_TURN    0
#define GAT_CONCEPT 1
#define GAT_NONE    2

typedef struct {
    std::string sGroundingModelName;
    std::string sActionName;
    int iGroundingActionType;
    bool bBargeIn;
} TGroundingActionHistoryItem;

typedef CGroundingModel* (*FCreateGroundingModel)(const std::string&);

class CGroundingManagerAgent : public CAgent {

public:
    CGroundingManagerAgent(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CGroundingManagerAgent");
    virtual ~CGroundingManagerAgent();
    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);
    static CGroundingManagerAgent* Instance();

public:
    void SetConfiguration(const std::string& configuration);
    TGroundingManagerConfiguration GetConfiguration();

    virtual void LoadPoliciesFromString(const std::string& policies);
    virtual void LoadPoliciesFromFile(const std::string& file);

    virtual std::string GetPolicy(const std::string& model_name);
    CExternalPolicyInterface* CreateExternalPolicyInterface(const std::string& host);
    void ReleaseExternalPolicyInterfaces();

    virtual void LoadBeliefUpdatingModel(const std::string& file);
    virtual void SetBeliefUpdatingModelName(const std::string& model_name);
    virtual std::string GetBeliefUpdatingModelName();

    virtual std::map<std::string, std::vector<float> >& GetBeliefUpdatingModelForAction(const std::string& action);
    virtual float GetConstantParameter(const std::string& param);
    virtual void PrecomputeBeliefUpdatingFeatures(CConcept* concept, CConcept* new_concept, const std::string& action);
    virtual float GetGroundingFeature(const std::string& feature_name);
    virtual std::string GetGroundingFeatureAsString(const std::string& feature_name);
    virtual void ClearBeliefUpdatingFeatures();
    virtual float GetPriorForConceptHyp(const std::string& concept, const std::string& hyp);
    virtual float GetConfusabilityForConceptHyp(const std::string& concept, const std::string& hyp);
    virtual std::string GetConceptTypeInfoForConcept(const std::string& concept);
    void UseGroundingAction(const std::string& action_name, CGroundingAction* grounding_action);

    int GroundingActionNameToIndex(const std::string& grounding_action_name);
    std::string GroundingActionIndexToName(size_t index);

    CGroundingAction* operator[] (const std::string& action_name);
    CGroundingAction* operator[] (size_t index);

    void RequestTurnGrounding(bool is_turn_grounding_request = true);
    void RequestConceptGrounding(CConcept* concept);

    std::string ScheduleConceptGrounding(CConcept* concept);
    void LockConceptGroundingRequestsQueue();
    void UnlockConceptGroundingRequestsQueue();
    void SetConceptGroundingRequestStatus(CConcept* concept, int status);
    int GetConceptGroundingRequestStatus(CConcept* concept);
    void ConceptGroundingRequestCompleted(CConcept* concept);
    void RemoveConceptGroundingRequest(CConcept* concept);
    void PurgeConceptGroundingRequestsQueue();

    void GAHAddHistoryItem(const std::string& grounding_model_name, const std::string& action_name, int type);
    void GAHSetBargeInFlag(int turn_num, bool flag);
    std::string GAHGetTurnGroundingAction(int turn_num);
    int GAHCountTakenInLastNTurns(bool is_heard, const std::string& name, int num_turns = -1);
    int GAHCountTakenByGroundingModelInLastNTurns(bool is_heard, const std::string& name, const std::string& model_name, int num_turns = -1);

    void RegisterGroundingModelType(const std::string& name, FCreateGroundingModel func);
    CGroundingModel* CreateGroundingModel(const std::string& model_type, const std::string& policy);

    bool HasPendingRequests();
    bool HasPendingTurnGroundingRequest();
    bool HasPendingConceptGroundingRequests();
    bool HasUnprocessedConceptGroundingRequests();
    bool HasScheduledConceptGroundingRequests();
    bool HasExecutingConceptGroundingRequests();
    bool GroundingInProgressOnConcept(CConcept* concept);
    std::string GetScheduledGroundingActionOnConcept(CConcept* concept);

    virtual void Run();

private:
    int GetConceptGroundingRequestIndex(CConcept* concept);
    std::string LoadPolicy(const std::string& file);

private:
    static CGroundingManagerAgent* s_instance;

private:
    std::map<std::string, std::string> _policies;
    std::map<std::string, float> _constant_parameters;
    std::map<std::string, std::map<std::string, std::vector<float> > > _belief_updating_models;
    std::map<std::string, std::map<std::string, std::vector<float> > > _concept_values_info;
    std::map<std::string, std::string> _concept_type_info;
    std::map<std::string, float> _belief_updating_features;
    std::vector<CGroundingAction*> _ga_actions;
    std::vector<std::string> _action_names;
    std::map<std::string, CExternalPolicyInterface*> _external_policies;
    TGroundingManagerConfiguration _config;
    bool _is_turn_grounding_request;
    TConceptGroundingRequests _concept_grounding_requests;
    bool _is_locked_grounding_requests;
    std::vector<std::vector<TGroundingActionHistoryItem> > _grounding_actions_history;
    std::map<std::string, FCreateGroundingModel> _grounding_model_type_registry;
};

}

#endif
