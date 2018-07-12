#include "query_parser_serving_handler.h"

//#include <boost/regex.hpp>

//#include "Common/Base/stringUtil.h"
//#include "Common/Base/flags.h"
//#include "Common/Util/ThriftClientWrapper/thrift.h"

using namespace std;

DEFINE_string(dir, "../Data/Grammer/", "dir");
DEFINE_string(dict_file, "base.dic", "dict_file");
DEFINE_string(grammar_file, "chinamobile.net", "grammar_file");
DEFINE_string(frame_file, "frames", "frame_file");
DEFINE_string(netConfigFile, "", "netConfigFile");

DEFINE_int32(query_limit_length, 35, "");
namespace robot {

QueryParserServingHandler::~QueryParserServingHandler()
{
    if (mpGrammarGraph != NULL )
    {
        delete mpGrammarGraph;
    }
    if (mpScriptTrans != NULL )
    {
        delete mpScriptTrans;
    }
}

bool QueryParserServingHandler::Init()
{
    VLOG(1) << __FUNCTION__;
    mpGrammarGraph = new GrammarGraph();
    int res = mpGrammarGraph->load_graph_from_file(
            FLAGS_dir.c_str(), FLAGS_dict_file.c_str(), FLAGS_grammar_file.c_str(), FLAGS_frame_file.c_str()/*, FLAGS_netConfigFile.c_str()*/ );
    if( res <= 0 )
    {
        VLOG(ERROR) << "Failed to load GrammarGraph;";
        return false;
    }
    mpScriptTrans = new FrameSemantic::Utf8NormalizeScriptTrans();
    return true;
}

void QueryParserServingHandler::QueryParserProcess(robot::DataBundle& response, const robot::DataBundle& request) 
{
    VLOG(5) << "request:" << FromThriftToUtf8DebugString(&request);
    int64 usec_a = common::GetTimeInUsec();
    VLOG(1) << __FUNCTION__ << ", start, query: " << request.query << ", call_id: " << request.user_id;
    if ( !request.__isset.query /*|| !request.__isset.user_info || !request.user_info.__isset.uid */ ) 
    {
        VLOG(ERROR) << "check request.query ";
        return ;
    }
    
    VLOG(4) << "Query: " << request.query ;

    std::ostringstream oss;
    string str_sos = "^";
    string str_eos = "$";
    Parser* mpParser = new Parser( *mpGrammarGraph, str_sos, str_eos );
    mpParser->setScriptTrans( mpScriptTrans );
    mpParser->reset();
    mpParser->parse(request.query.c_str(), 1);
    /*
    mpParser->extractParses(oss, '.');
    string parserResult = oss.str();
    ReplaceSubstringsAfterOffset(&parserResult, 0, "\x02", "\n");
*/

    vector<string> results = mpParser->extractParses('.');
    string ret = "";
    for (int i = 0; i < results.size(); i++)
    {
        VLOG(4) << "Result = " << results[i];
        string temp = FormatResult(results[i]);
        if (temp != "")
        {
            if (ret == "")
            {
                ret = temp;
            }
            else 
            {
                ret += "\n" + temp;
            }
        }
    }

    robot::Reply reply;
    reply.__set_reply(ret);

    response.__set_reply(reply);
    response.__set_intention_score(0.95);
    
    if (mpParser != NULL )
    {
        delete mpParser;
    }
    
    int64 usec_b = common::GetTimeInUsec();
    VLOG(0) << __FUNCTION__ << ", over, query: " << request.query << ", call time : " << (usec_b - usec_a) << ", call_id: " << request.user_id;
    VLOG(5) << "response:" << FromThriftToUtf8DebugString(&response);
}

string QueryParserServingHandler::FormatResult(const string& str)
{
    string ret = "";
    vector<string> slotVec;
    SplitString(str, '\x01', &slotVec);
    for (int i = 0; i < slotVec.size(); i++)
    {
        vector<string> fieldVec;
        SplitString(slotVec[i], '.', &fieldVec); 

        for (int j = 0; j < fieldVec.size(); j++)
        {
            vector<string> valueVec;
            SplitString(fieldVec[j], '=', &valueVec);
            if (valueVec.size() != 2)
            {
                valueVec.clear();
                SplitString(fieldVec[j], '-', &valueVec);
            }
            else
            {
                vector<string> tmpVec;
                SplitString(valueVec[0], '+', &tmpVec);
                if (tmpVec.size() == 2)
                {
                    valueVec[0] = tmpVec[0];
                    valueVec[1] = valueVec[1]+ tmpVec[1];
                }
            }
            map<string, string> slotMap;
            if (valueVec.size() == 2)
            {
                slotMap[valueVec[0]] = valueVec[1];
                VLOG(4) << valueVec[0] << " -- " << valueVec[1];
            }
            map<string,string>::iterator it;
            for (it = slotMap.begin(); it != slotMap.end(); it++)
            {
                if (ret == "")
                {
                    ret = "[" + it->first + "]=" + it->second;
                }
                else 
                {
                    ret += "|[" + it->first + "]=" + it->second;
                }
            }
        }               
    }

    return ret;
}

}
