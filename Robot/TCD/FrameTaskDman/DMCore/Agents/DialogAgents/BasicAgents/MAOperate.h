// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-01 16:34
// Description: 执行以下简单的操作 

#ifndef _MA_OPERATE_H__
#define _MA_OPERATE_H__

#include "../DialogAgent.h"

namespace TrioTDM {
/**
 * @brief 执行内部操作的的节点类
 */
class CMAOperate : public CDialogAgent {

public:
    CMAOperate(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMAOperate");
    virtual ~CMAOperate();

    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);

public:
    virtual TDialogExecuteReturnCode Execute(COutput* output);
    virtual bool SuccessCriteriaSatisfied();

public:
    /**
     * @brief 执行的操作
     */
    virtual void Operate();
};

#define DEFINE_OPERATE_AGENT(OperatorAgentClass, OTHER_CONTENTS) \
    class OperatorAgentClass : public CMAOperate {\
    public:\
        OperatorAgentClass(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMAOperate:"#OperatorAgentClass) : \
        CMAOperate(name, configuration, type) {;}\
        ~OperatorAgentClass() {;}\
        static CAgent* AgentFactory(const std::string& name, const std::string& configuration) {\
            return new OperatorAgentClass(name, configuration);\
        }\
        OTHER_CONTENTS \
    };\

#define OPERATE(CONTENTS) \
    public: \
        virtual void Operate() {\
            CONTENTS \
        }\

}

#endif
