#ifndef QueryUnderstanding_SERVING_HANDLER_H_
#define QueryUnderstanding_SERVING_HANDLER_H_

#include "Robot/Public/serviceHead.h"

//#include <map>
//#include <string>

//#include "Common/Base/trio.h"
#include "Common/Util/Thread/threadPool.h"
//#include "Common/Util/Trie/jumpTrie.h"


#include "Robot/Public/QueryPreprocess/queryProcessManager.h"

namespace robot {


class QueryUnderstandingServingHandler: public QueryUnderstandingServingIf {
public:
    QueryUnderstandingServingHandler() {
        caller_pool_.reset(NULL);
    }
    virtual ~QueryUnderstandingServingHandler() {
        ;
    }

    bool Init();
    virtual void QueryUnderstandingProcess( DataBundle &response, const DataBundle &request);

private:
    DEFINE_CALL_RPC_SERVICE(Faq);
    DEFINE_CALL_RPC_SERVICE(QueryParser);

    bool RandomReply(const std::vector<Reply>& replys, Reply* reply);
    void FillProtoPreprocess(const preprocess::QueryInfo& info, DataBundle& resp);

    bool FormatResponse(DataBundle &response, int32_t n);

private:
    scoped_ptr<ThreadPool> caller_pool_;
    scoped_ptr<ThriftClientManager<FaqServingClient> > faq_client_mgr_;
    scoped_ptr<ThriftClientManager<QueryParserServingClient> > query_parser_client_manager_;
    preprocess::DocProcessHandlerManager query_preproc_mod_;
    //trie::JumpTrie keyword_trie;
};

} // namespace robot
#endif  // QueryUnderstanding_SERVING_HANDLER_H_
