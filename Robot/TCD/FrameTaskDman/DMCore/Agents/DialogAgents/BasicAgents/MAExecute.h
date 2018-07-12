// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-16 15:34
// Description: 执行类节点 

#ifndef _MA_EXECUTE_H__
#define _MA_EXECUTE_H__

#include "../DialogAgent.h"
#include "Projects/ChinaMobile/CRS/Src/Session/ExecuteApi.h"

namespace TrioTDM {

typedef bool (CExecuteApi::*ExecuteFunc)(CConcept& param);
/**
 * @brief 执行节点类
 */
class CMAExecute : public CDialogAgent {

public:
    CMAExecute(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMAExecute");
    virtual ~CMAExecute();

    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);

public:
    virtual TDialogExecuteReturnCode Execute(COutput* output);
    virtual bool SuccessCriteriaSatisfied();

protected:
    /**
     * @brief 返回调用到执行函数
     */
    virtual ExecuteFunc GetExecuteFunc();
    /**
     * @brief 返回调用的执行函数的传入参数
     */
    virtual CConcept& GetExecuteParam();

};

#define DEFINE_EXECUTE_AGENT(execute_agent_class, OTHER_CONTENTS) \
    class execute_agent_class : public CMAExecute {\
    public:\
        execute_agent_class(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMAExecute:"#execute_agent_class) : \
        CMAExecute(name, configuration, type) {;}\
        ~execute_agent_class() {;}\
        static CAgent* AgentFactory(const std::string& name, const std::string& configuration) {\
            return new execute_agent_class(name, configuration);\
        }\
        OTHER_CONTENTS\
    };\

/*
#define EXECUTE(CONTENTS) \
    public:\
        virtual void ExecuteRoutine() {\
            CONTENTS;\
        }\

#define CALL(strg) \
    public:\
        virtual std::string GetExecuteCall() {\
            return strg;\
        }\
*/

#define EXECUTE_FUNC(Func) \
    public:\
        virtual ExecuteFunc GetExecuteFunc() {\
            return Func;\
        }\

#define EXECUTE_PARAM(DO_STUFF) \
    public:\
        virtual CConcept& GetExecuteParam() {\
            DO_STUFF;\
        }\

}

#endif
