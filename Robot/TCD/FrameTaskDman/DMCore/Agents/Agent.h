/****************************************
 * @author : lzy                        *
 * @email  : liuzhengyong@trio.ai       *
 * @ate   : 2017-08-07 17:12            *
 * @file Agent.h                        *
 * @brief 所有节点类的基类              *
 ****************************************/

#ifndef _AGENT_H__
#define _AGENT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Registry.h"

namespace TrioTDM {

/**
 * @brief 管理类的前置申明
 */
class CDMCoreAgent;

/**
 * @brief 所有节点类的的基类
 */
class CAgent {

public:
    /**
     * @brief 带参构造函数
     * @param [in]  name 节点名
     * @param [in]  configuration 节点的配置信息
     * @param [in]  type 节点的类型
     */
    CAgent(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent");
    virtual ~CAgent();
    /**
     * @brief 静态类成员函数，生成一个 节点对象
     * @param [in]  name 节点名
     * @param [in]  configuration 节点配置信息
     * @return 返回类对象指针
     */
    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);

    std::string GetName();
    std::string GetType();

    virtual void SetOwnerCoreAgent(CDMCoreAgent* core_agent);

    void SetConfiguration(const std::string& configuration);
    void SetConfiguration(const std::map<std::string, std::string>& configuration);
    void SetParameter(const std::string& param, const std::string& value);
    bool HasParameter(const std::string& param);
    std::string GetParameterValue(const std::string& param);
    /**
     * @brief 注册类对象到对象管理类
     */
    virtual void Register();
    /**
     * @brief 从对象管理类解除类对象的注册
     */
    virtual void UnRegister();
    /**
     * @brief 生成类对象的关联数据
     * @note 类对象生成后会立即调用该函数
     */
    virtual void Create();
    /**
     * @brief 初始化函数
     */
    virtual void Initialize();
    /**
     * @brief 重置节点管理的数据
     */
    virtual void Reset();

protected:
    std::string _name; ///< 节点名字，带路径
    std::string _type; ///< 节点类型
    CDMCoreAgent*                       _owner_core_agent; ///< 所属的管理对象
    std::map<std::string, std::string>  _configuration; ///< 配置信息
};


}

#endif
