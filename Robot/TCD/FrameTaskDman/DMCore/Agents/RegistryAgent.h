// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-11-09 18:09
// Description: 节点类对象管理类

#ifndef _REGISTRY_AGENT_H__
#define _REGISTRY_AGENT_H__


#include "Robot/TCD/FrameTaskDman/Utils/Common.h"

namespace TrioTDM {

class CAgent;

class CRegistryAgent {

public:
    CRegistryAgent();
    ~CRegistryAgent();

    void Clear();
    /**
     * @brief 重建 [] 运算符
     * @param[in]  agent_name 节点名字
     * @return 对应节点名的对象指针，不存在的情况返回 NULL
     */
    CAgent* operator[](const std::string& agent_name) const;
    /**
     * @brief 注册节点
     */
    bool RegisterAgent(const std::string& agent_name, CAgent* pagent);
    /**
     * @brief 节点解除注册
     */
    bool UnRegisterAgent(const std::string& agent_name);
    /**
     * @brief 判断节点是否存在
     */
    bool IsRegisteredAgent(const std::string& agent_name) const;

private:
    /**
     * @brief 节点名 到 节点对象指针的映射
     */
    std::map<std::string, CAgent*> _agents_hash;

};

}

#endif
