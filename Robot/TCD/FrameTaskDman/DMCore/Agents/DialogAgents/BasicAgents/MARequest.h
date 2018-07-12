// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-16 16:12
// Description: 可以输出提示语，生成期望信息 

#ifndef _MA_REQUEST_H__
#define _MA_REQUEST_H__

#include "../DialogAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Outputs/Output.h"

namespace TrioTDM {
/**
 * @brief 输出提示语等待用户反馈，生成期望 concept 信息
 */
class CMARequest: public CDialogAgent {

public:
    CMARequest(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent:CMARequest");
    virtual ~CMARequest();
    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);

public:
    virtual TDialogExecuteReturnCode Execute(COutput* output);
    virtual void Reset();
    virtual int DeclareExpectations(std::vector<TConceptExpectation>& expectations, bool is_must_not_dts_agent = false);
    virtual bool PreconditionsSatisfied();
    virtual bool SuccessCriteriaSatisfied();
    virtual bool FailureCriteriaSatisfied();
    virtual bool RequiresFloor() {return true;}
    virtual int ModelTypeOperator();
    virtual void ClearOtherUserConcepts();
    virtual void UnSignalFocus();

public:
    virtual std::string GrammarMapping();
    virtual std::string RequestedConceptName();
    virtual CConcept* RequestedConcept();
    virtual int GetTimeoutPeriod();
    virtual float GetNonunderstandingThreshold();
};

#define DEFINE_REQUEST_AGENT(request_agent_class, OTHER_CONTENTS) \
    class request_agent_class : public CMARequest {\
    public:\
           request_agent_class(const std::string& name, const std::string& configuration = "", const std::string&type = "CAgent:CDialogAgent:CMARequest:"#request_agent_class) :\
        CMARequest(name, configuration, type) {;}\
        ~request_agent_class() {;}\
        static CAgent* AgentFactory(const std::string& name, const std::string& configuration) {\
            return new request_agent_class(name, configuration);\
        }\
        OTHER_CONTENTS\
    };\

#define REQUEST_CONCEPT(ConceptName) \
    public:\
        virtual std::string RequestedConceptName() {\
            return (ReplaceSubString(#ConceptName, "#", GetDynamicAgentID()));\
        };\

#define GRAMMAR_MAPPING(grammar_mapping_as_string) \
    public:\
        virtual std::string GrammarMapping() {\
            return (std::string)(grammar_mapping_as_string);\
        }\

#define TIMEOUT_PERIOD(period) \
    public:\
        virtual int GetTimeoutPeriod() {\
            return (period);\
        }\

#define NONUNDERSTANDING_THRESHOLD(threshold) \
    public:\
        virtual float GetNonunderstandingThreshold() {\
            return (threshold);\
        }\

#define CLEAR_OTHER_CONCEPTS { ClearOtherUserConcepts(); }

#define UNSIGNAL_FOCUSCLAIMS { UnSignalFocus(); }

}

#endif
