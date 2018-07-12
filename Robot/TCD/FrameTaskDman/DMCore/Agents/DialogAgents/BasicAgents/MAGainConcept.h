// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-06 14:48
// Description: 获取系统数据

#ifndef _MA_GAIN_CONCEPT_H__
#define _MA_GAIN_CONCEPT_H__

#include "../DialogAgent.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

namespace TrioTDM {

typedef bool (CGainConceptApi::*GainConceptFunc)(CConcept& data); ///< 不带参获取数据接口
typedef bool (CGainConceptApi::*GainConceptParamFunc)(CConcept& param, CConcept& data); ///< 带参获取数据接口

/**
 * @brief 获取系统数据的节点类
 */
class CMAGainConcept : public CDialogAgent {

public:
    CMAGainConcept(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMAGainConcept");
    virtual ~CMAGainConcept();

    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);

public:
    virtual TDialogExecuteReturnCode Execute(COutput* output);
    virtual bool SuccessCriteriaSatisfied();
    virtual std::string ConceptName();

protected:
    /**
     * @brief 返回获取数据的不带参函数
     */
    virtual GainConceptFunc GetGainConceptFunc();
    /**
     * @brief 返回获取数据的带参函数
     */
    virtual GainConceptParamFunc GetGainConceptParamFunc();
    /**
     * @brief 返回传入参数
     */
    virtual CConcept& GetExecuteParam();
    /**
     * @brief 返回传入参数的名字
     */
    virtual std::string ParamName();

//private:
//    static std::map<std::string, std::string> s_key2value;
};

#define DEFINE_GAIN_CONCEPT_AGENT(GainConceptClass, OTHER_CONTENTS) \
    class GainConceptClass : public CMAGainConcept {\
    public:\
        GainConceptClass(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMAGainConcept:"#GainConceptClass) : \
        CMAGainConcept(name, configuration, type) {\
            ;\
        }\
        ~GainConceptClass() {\
            ;\
        }\
        static CAgent* AgentFactory(const std::string& name, const std::string& configuration) {\
            return new GainConceptClass(name, configuration);\
        }\
        OTHER_CONTENTS \
    };\

#define CONCEPT_NAME(Name) \
    public:\
        virtual std::string ConceptName() {\
            return (#Name);\
        }\

#define GAIN_CONCEPT_FUNC(Func) \
    public:\
        virtual  GainConceptFunc GetGainConceptFunc() {\
            return Func;\
        }\

#define GAIN_CONCEPT_PARAM_FUNC( Func ) \
    public:\
        virtual GainConceptParamFunc GetGainConceptParamFunc() {\
            return Func;\
        }\

#define PARAM_NAME( Name ) \
    public:\
        virtual std::string ParamName() {\
            return (#Name);\
        }\

#define EXECUTE_PARAM(DO_STUFF) \
    public:\
        virtual CConcept& GetExecuteParam() {\
            DO_STUFF;\
        }\

}

#endif
