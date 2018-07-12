// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-18 14:54
// Description: 任务树管理类

#ifndef _DTT_MANAGER_AGENT_H__
#define _DTT_MANAGER_AGENT_H__

#include <boost/thread.hpp>
#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Agent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DialogAgent.h"

namespace TrioTDM {
/**
 * @brief 增加子节点的方法,枚举值
 */
typedef enum {
    mmAsLastChild,
    mmAsFirstChild,
    mmAsLeftSibling,
    mmAsRightSibling,
} TMountingMethod;
/**
 * @brief 增加子节点的方法，字符串值
 */
static std::string MountingMethodAsString[] = {
    "last child", "first child",
    "left sibling", "right sibling"
};

/**
 * @brief 通用流程的节点信息
 */
typedef struct {
    std::string sDAName;
    std::string sDAType;
    FRegisterAgent fRegisterAgent;
    std::string sDAConfiguration;
} TDiscourseAgentInfo;
/**
 * @brief 任务树管理类
 */
class CDTTManagerAgent: public CAgent {

public:
    CDTTManagerAgent(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDTTManagerAgent");
    virtual ~CDTTManagerAgent();

    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);
    static CDTTManagerAgent* Instance();

    virtual void Reset();
    virtual void Reset(const std::string& root_name);
    virtual void Initialize();

public:
    /**
     * @brief 增加通用任务的节点信息
     */
    void Use(const std::string& type, const std::string& name, FRegisterAgent func, const std::string& configuration);
    /**
     * @brief 创建任务树
     */
    CDialogAgent* CreateDialogTree(const std::string& root_name, CDMCoreAgent* core_agent);
    /**
     * @brief 销毁任务树
     */
    void DestroyDialogTree(const std::string& root_name);
    /**
     * @brief 重建任务树
     */
    void ReCreateDialogTree(const std::string& root_name, CDMCoreAgent* core_agent);
    /**
     * @brief 创建任务树
     */
    CDialogAgent* CreateDialogTaskTree(const std::string& root_name);
    /**
     * @brief 注册树节点创建函数
     */
    void CreateDialogTaskAgentome();
    /**
     * @brief 返回任务树根节点
     */
    CDialogAgent* GetDialogTaskTreeRoot(const std::string& root_name, CDMCoreAgent* core_agent);
    /**
     * @brief 增加任务树节点
     * @param[in] da_where 新增节点的父节点
     * @param[in] da_who 新建节点
     * @param[in] how 新增节点的方法
     * @param[in] agent_id 新增节点的动态ID
     */
    void MountAgent(CDialogAgent* da_where, CDialogAgent* da_who, TMountingMethod how, const std::string& agent_id = "");
    /**
     * @brief 增加任务树节点
     * @param[in] where 新增节点的父节点
     * @param[in] agent_type 新增节点的类型
     * @param[in] agent_name 新增节点的名字
     * @param[in] configuration 新增节点的配置
     * @param[in] how 新增节点的方法
     * @param[in] agent_id 新增节点的动态ID
     */
    CDialogAgent* MountAgent(CDialogAgent* where, const std::string& agent_type, const std::string& agent_name, 
            const std::string& configuration, TMountingMethod how, const std::string& agent_id = "");
    /**
     * @brief 从任务树删除节点
     */
    void UnMountAgent(CDialogAgent* who);
    /**
     * @brief 使用数组型 concept 批量增加树节点
     */
    void MountAgentsFromArrayConcept(CDialogAgent* parent, const std::string& concepts_name, 
            const std::string& agents_type, const std::string& agents_name, 
            const std::string& concept_name, const std::string& dynamic_id = "#");
    /**
     * @brief 返回节点的父节点名字
     * @param[in] 节点带路径名
     */
    static std::string GetParentName(const std::string& agent_path);
    /**
     * @brief 判断节点是否是父子关系
     */
    static bool IsParentOf(const std::string& parent_agent_path, const std::string& agent_path);
    /**
     * @brief 判断节点是否是子父关系
     */
    static bool IsChildOf(const std::string& child_agent_path, const std::string& agent_path);
    /**
     * @brief 判断节点是否有继承关系
     */
    static bool IsAncestorOf(const std::string& ancestor_agent_path, const std::string& agent_path);
    /**
     * @brief 判断节点是否有继承关系或者相等
     */
    static bool IsAncestorOrEqualOf(const std::string& ancestor_agent_path, const std::string& agent_path);
    /**
     * @brief 判断节点是否有继承关系
     */
    static bool IsDescendantOf(const std::string& descendant_agent_path, const std::string& agent_path);
    /**
     * @brief 判断节点是否属于同一父节点
     */
    static bool IsSiblingOf(const std::string& agent_path_a, const std::string& agent_path_b);

private:
    static CDTTManagerAgent* s_instance; ///< 单例
    boost::recursive_mutex _mutex; ///< 递归锁
    std::map<std::string, CDialogAgent*> _session_2_root; ///< session 到 更任务树根节点的映射
    std::vector<TDiscourseAgentInfo> _da_infos; ///< 通用任务树节点集合
};

CDialogAgent* MountAgent(CDialogAgent* where, const std::string& agent_type, const std::string& agent_name, const std::string& configuration, TMountingMethod how);
void MountAgentsFromArrayConcept(CDialogAgent* parent, const std::string& array_concept_name, const std::string& agents_type, const std::string& agent_name,
        const std::string& agents_configuration = "", const std::string& agents_dy_id = "#");

#define DECLARE_DIALOG_TASK_ROOT(RootAgentType, GroundingModelSpec) \
    CDialogAgent* CDTTManagerAgent::CreateDialogTaskTree(const std::string& root_name ) {\
        if ( CRegistryAgentType::Instance() == NULL ) {\
            VLOG(FATAL) << __FUNCTION__ << ", CRegistryAgentType::Instance() is NULL.";\
            return NULL;\
        } \
        /*调用 CDialogAgent::Create() */\
        CDialogAgent* root = (CDialogAgent*)\
                CRegistryAgentType::Instance()->CreateAgent(#RootAgentType, root_name);\
        root->SetParent(NULL);\
        root->CreateGroundingModel(GroundingModelSpec);\
        root->Initialize();\
        /*root->Register();*/\
        return root;\
    }\

#define DECLARE_AGENTS(DECLARATIONS) \
    void CDTTManagerAgent::CreateDialogTaskAgentome() {\
        DECLARATIONS;\
    }\

#define DECLARE_AGENT(AgentClass) \
    if ( CRegistryAgentType::Instance() != NULL ) {\
        CRegistryAgentType::Instance()->RegisterAgentType(#AgentClass, AgentClass::AgentFactory);\
    }\

}

#endif
