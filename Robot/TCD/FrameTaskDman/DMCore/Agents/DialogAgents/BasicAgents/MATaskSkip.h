// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-26 15:14
// Description: 调整节点 

#ifndef _MA_TASK_SKIP_H__
#define _MA_TASK_SKIP_H__

#include "../DialogAgent.h"

namespace TrioTDM {
/**
 * @brief 实现节点间跳转到功能节点类
 */
class CMATaskSkip : public CDialogAgent {

public:
    CMATaskSkip(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMATaskSkip");
    virtual ~CMATaskSkip();

public:
    virtual void OnDestruction();
    virtual bool IsDTSAgent();
    virtual void SetOwnerCoreAgent(CDMCoreAgent* core_agent);

    virtual TDialogExecuteReturnCode Execute(COutput* output);
    virtual bool SuccessCriteriaSatisfied();
    virtual void Register();
    
    virtual bool Load(const std::map<std::string, std::string>& agents, const std::map<std::string, std::string>& concepts);
    virtual bool DeclareSaveData(std::map<std::string, std::string>& agents, std::map<std::string, std::string>& concepts);
    virtual bool IsNeedSaveForTaskSkip();

protected:
    virtual bool CreateSkipTask();

protected:
    /**
     * @brief 返回跳转到的节点类型
     */
    virtual std::string DialogType() = 0;
    /**
     * @brief 返回跳转到的节点名字
     */
    virtual std::string DialogName() = 0;
    /**
     * @brief 返回跳转到底节点的配置
     */
    virtual std::string DialogConfiguration();

protected:
    CDialogAgent* _skip_task;

};

#define DEFINE_TASK_SKIP_AGENT( TaskSkipClass, OTHER_CONTENTS) \
    class TaskSkipClass : public CMATaskSkip {\
    public:\
        TaskSkipClass(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMATaskSkip:"#TaskSkipClass) : \
        CMATaskSkip(name, configuration, type) { ; }\
        ~TaskSkipClass() { ; }\
        static CAgent* AgentFactory(const std::string& name, const std::string& configuration) {\
            return new TaskSkipClass(name, configuration);\
        }\
        OTHER_CONTENTS \
    };\

#define DIALOG_TYPE( Type ) \
    public:\
        virtual std::string DialogType() {\
            return #Type;\
        }\

#define DIALOG_NAME( Name ) \
    public:\
        virtual std::string DialogName() {\
            return #Name;\
        }\

#define DIALOG_CONFIGURATION( Configuration ) \
    public:\
        virtual std::string DialogConfiguration() {\
            return #Configuration;\
        }\

}

#endif
