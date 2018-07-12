// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-25 20:24
// Description: 输出提示语类

#ifndef _MA_INFORM_H__
#define _MA_INFORM_H__

#include "../DialogAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Outputs/Output.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Outputs/NLG.h"

namespace TrioTDM {
/**
 * @brief 输出提示语的节点类
 */
class CMAInform : public CDialogAgent {

public:
    CMAInform(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMAInform");
    virtual ~CMAInform();
    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);

public:
    virtual TDialogExecuteReturnCode Execute(COutput* output);
    virtual void ReOpenTopic();
    virtual void Reset();
    virtual bool SuccessCriteriaSatisfied();
    virtual bool RequiresFloor() { return true; }
    virtual TDialogExecuteReturnCode ReturnCode();

    virtual int ModelTypeOperator();
};

#define DEFINE_INFORM_AGENT(InformAgentClass, OTHER_CONTENTS) \
    class InformAgentClass: public CMAInform {\
    public:\
        InformAgentClass(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMAInform:"#InformAgentClass): \
        CMAInform(name, configuration, type) {\
        }\
        ~InformAgentClass() {;}\
        static CAgent* AgentFactory(const std::string& name, const std::string& configuration) {\
            return new InformAgentClass(name, configuration);\
        }\
        OTHER_CONTENTS\
    };\

#define RETURN_CODE( code ) \
    public:\
        TDialogExecuteReturnCode ReturnCode() {\
            return code;\
        }\

}

#endif
