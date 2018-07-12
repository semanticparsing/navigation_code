
#ifndef _INFO_QUERY_SERVING_HANDLER_H__
#define _INFO_QUERY_SERVING_HANDLER_H__

#include "Robot/Public/serviceHead.h"
#include "Robot/TCD/FrameSemanticParser_2_0/grammarParser/Parser.h"
//#include "Robot/TCD/FrameSemanticParser_2_0/grammarParser/TreeTaggingScriptTrans.h"

using FrameSemantic::Parser;
using FrameSemantic::GrammarGraph;

namespace robot {

class QueryParserServingHandler : public QueryParserServingIf {
public:
    QueryParserServingHandler() {
        ;
    }

    virtual ~QueryParserServingHandler() ;

    bool Init();
    virtual void QueryParserProcess(robot::DataBundle& response, const robot::DataBundle& request);
    string FormatResult(const string& str);

private:


private:
    //Parser* mpParser = NULL;
    GrammarGraph* mpGrammarGraph = NULL;
    FrameSemantic::ScriptTransBase* mpScriptTrans = NULL;
};

}


#endif
