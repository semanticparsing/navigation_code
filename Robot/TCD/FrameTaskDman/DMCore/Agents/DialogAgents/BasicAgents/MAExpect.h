// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-26 10:42
// Description: 期望节点类

#ifndef _MA_EXPECT_H__
#define _MA_EXPECT_H__

#include "../DialogAgent.h"

namespace TrioTDM {
/**
 * @brief 期望节点类
 */
class CMAExpect : public CDialogAgent {

public:
    CMAExpect(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMAExpect");
    virtual ~CMAExpect();

    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);

public:
    virtual bool PreconditionsSatisfied();
    virtual bool IsExecutable();
    virtual TDialogExecuteReturnCode Execute(COutput* output);
    /**
     * @brief 生成 concept 期望信息集合
     */
    virtual int DeclareExpectations(std::vector<TConceptExpectation>& expections, bool is_must_not_dts_agent = false);
    virtual void SetCompleted(TCompletionType type = ctSuccess);

public:
    /**
     * @brief 返回语法映射信息
     */
    virtual std::string GrammarMapping();
    /**
     * @brief 返回期望的 concept 的名字
     */
    virtual std::string ExpectedConceptName();
    /**
     * @brief 返回期望的 concept 指针
     */
    virtual CConcept* ExpectedConcept();

};

#define DEFINE_EXPECT_AGENT(ExpectAgentClass, OTHER_CONTENTS) \
    class ExpectAgentClass : public CMAExpect {\
    public:\
        ExpectAgentClass(const std::string& name, const std::string& configuration, const std::string& type = "CAgent:CDialogAgent:CMAExpect:"#ExpectAgentClass) : \
        CMAExpect(name, configuration, type) {;}\
        ~ExpectAgentClass() { ; }\
        static CAgent* AgentFactory(const std::string& name, const std::string& configuration) {\
            return new ExpectAgentClass(name, configuration);\
        }\
        OTHER_CONTENTS\
    };\

#define EXPECT_CONCEPT(ConceptName) \
    public:\
    virtual std::string ExpectedConceptName() {\
        return (#ConceptName);\
    };\

#define GRAMMAR_MAPPING(GrammarMappingAsString) \
    public:\
    virtual std::string GrammarMapping() {\
        return (std::string)(GrammarMappingAsString);\
    };\

} 

#endif
