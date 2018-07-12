// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-11-09 17:22
// Description: 节点工厂类

#ifndef _REGISTRY_AGENT_TYPE_H__
#define _REGISTRY_AGENT_TYPE_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"

namespace TrioTDM {

class CAgent;

/**
 * @brief 节点创建函数
 */
typedef CAgent* (*FCreateAgent)(const std::string&, const std::string& );
typedef void (*FRegisterAgent) (void);

class CRegistryAgentType {

public:
    CRegistryAgentType();
    ~CRegistryAgentType();

    static CRegistryAgentType* Instance();

    void Clear();
    /**
     * @brief 注册 节点类型 和 节点的创建函数
     */
    bool RegisterAgentType(const std::string& agent_type_name, FCreateAgent fct_create_agent);
    /**
     * @brief 解除 节点类型 到 节点的创建函数 的映射
     */
    bool UnRegisterAgentType(const std::string& agent_type_name);
    /**
     * @brief 判断 节点类型是否存在映射
     */
    bool IsRegisteredAgentType(const std::string& agent_type_name) const;
    /**
     * @brief 创建节点
     * @param[in]   agent_type_name 节点类型
     * @param[in]   agent_name 创建的节点名
     * @param[in]   agent_configuration 创建节点的配置信息
     */
    CAgent* CreateAgent(const std::string& agent_type_name, const std::string& agent_name, const std::string& agent_configuration = "") const;

private:
    static CRegistryAgentType*          s_instance;
    /**
     * @brief 节点类型到节点创建函数的映射
     */
    std::map<std::string, FCreateAgent> _agents_type_hash;
};

}

#endif
