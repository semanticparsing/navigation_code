// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-16 13:28
// Description: 管理节点 / 父节点 

#ifndef _DIALOG_AGENCY_H__
#define _DIALOG_AGENCY_H__

#include "../DialogAgent.h"

#define LEFT_TO_RIGHT_OPEN "left_to_right_open"
#define LEFT_TO_RIGHT_ENFORCED "left_to_right_enforced"
#define START_AGAIN_LEFT_TO_RIGHT "start_again_left_to_right"

namespace TrioTDM {

#define SUCCESS_POLICY_NORMAL "normal"
#define SUCCESS_POLICY_SELECT "select"
/**
 * @brief 管理类节点（父节点）
 */
class CDialogAgency : public CDialogAgent {

public:
    CDialogAgency(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CDialogAgency");
    virtual ~CDialogAgency();
    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);

public:
    virtual void Create();
    virtual void Initialize();
    virtual int GetMaxExecuteCounter();

public:
    virtual void CreateSubAgents();
    virtual TDialogExecuteReturnCode Execute(COutput* output);
    /**
     * @brief 返回下一个应该执行的节点
     */
    virtual CDialogAgent* NextAgentToExecute();
    /**
     * @brief 返回执行策略
     */
    virtual std::string GetExecutionPolicy();
    virtual bool SuccessCriteriaSatisfied();
    /**
     * @brief 初始化根节点的时候是否需要建树
     */
    virtual bool IsCreateBuildTree();
    virtual void OnFocusclaims();
    virtual bool IsNeedSaveForTaskSkip();


protected:
    virtual std::string GetSuccessPolicy();
    //virtual bool IsNeedDeclareExpectations();
};


#define DEFINE_AGENCY(agency_class, OTHER_CONTENTS) \
    class agency_class: public CDialogAgency {\
    public:\
           agency_class(const std::string name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CDialogAgency:"#agency_class) : \
        CDialogAgency(name, configuration, type) {\
           } \
        virtual ~agency_class() {;} \
        static CAgent* AgentFactory(const std::string& name, const std::string& configuration) {\
            return new agency_class(name, configuration);\
        }\
        OTHER_CONTENTS\
    };\

#define EXECUTION_POLICY(CONTENTS) \
    public:\
        virtual CDialogAgent* NextAgentToExecute() {\
            CONTENTS;\
        }\


#define DEFINE_SUBAGENTS(SUBAGENTS) \
    public:\
    virtual void CreateSubAgents() {\
        _is_create_sub_agent = true;\
        CDialogAgent* new_agent;\
        SUBAGENTS\
    }\

#define SUBAGENT(sub_agent_name, sub_agent_type, grounding_model_spec) \
    if ( CRegistryAgentType::Instance() != NULL ) {\
        new_agent = (CDialogAgent*)CRegistryAgentType::Instance()->CreateAgent(#sub_agent_type, #sub_agent_name);\
        new_agent->SetParent(this);\
        new_agent->CreateGroundingModel(grounding_model_spec);\
        _sub_agents.push_back(new_agent);\
        new_agent->Initialize();\
    }\

#define DEFINE_CONCEPTS(CONCEPTS) \
    public:\
        virtual void CreateConcepts() {\
            CONCEPTS;\
        }\

#define USER_CONCEPT(concept_name, concept_type, grounding_model_spec) \
    _concepts.push_back(new concept_type(#concept_name, csUser));\
    _concepts.back()->CreateGroundingModel(grounding_model_spec);\
    _concepts.back()->SetOwnerDialogAgent(this);\

#define SYSTEM_CONCEPT(concept_name, concept_type) \
    _concepts.push_back(new concept_type(#concept_name, csSystem));\
    _concepts.back()->CreateGroundingModel("none");\
    _concepts.back()->SetOwnerDialogAgent(this);\

#define SUCCESS_POLICY_IS_SELECT() \
    protected:\
        virtual std::string GetSuccessPolicy() {\
            return SUCCESS_POLICY_SELECT;\
        }\

#define EXECUTION_POLICY( Policy ) \
    public:\
        std::string GetExecutionPolicy() {\
            return Policy;\
        }\

#define CREATE_NOT_BUILD_TREE \
    public:\
        virtual bool IsCreateBuildTree() {\
            return false;\
        }\

}

#endif
