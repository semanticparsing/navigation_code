#ifndef NLU_SERVING_HANDLER_H_
#define NLU_SERVING_HANDLER_H_

#include "Projects/ChinaMobile/Proto/NLUServing.h"
#include "Robot/Public/serviceHead.h"
#include "Projects/ChinaMobile/Proto/ChinaMobileServiceHead.h"

//#include <map>
//#include <vector>
//#include <string>

//#include "Common/Base/trio.h"
#include "Common/Util/Thread/threadPool.h"
//#include "Common/Util/Trie/jumpTrie.h"
#include "Common/Util/FrameSemanticParser/QueryProcessor/QueryPreProcessor.h"

#include "Robot/Public/QueryPreprocess/queryProcessManager.h"

using namespace robot;
namespace chinaMobile {


class NLUServingHandler: public NLUServingIf {
public:
    // 构造函数
    NLUServingHandler() {
        caller_pool_.reset(NULL);
    }
    // 析构函数
    virtual ~NLUServingHandler() {
        ;
    }

    // 程序初始化
    bool Init();
    // 主要流程逻辑处理
    virtual void NLUProcess( ChinaMobileBundle &response, const ChinaMobileBundle &request);

private:
    //DEFINE_CALL_RPC_SERVICE(Faq);
    // 声明调用Parser的thrift对应函数
    DEFINE_CALL_RPC_SERVICE(QueryParser);
    // 声明调用FAQ的thrift对应函数
    DEFINE_CALL_CHINAMOBILE_FAQ_SERVICE(TrioBrain);

    // 生成传递给FAQ的Bundle
    void ConvertRequest(const ChinaMobileBundle &request, TrioDataBundle* trio_request);
    // 将FAQ返回的Bundle进行解析(未使用)
    void ConvertResponse(const TrioDataBundle &trio_response, ChinaMobileBundle* response);

    // bool RandomReply(const std::vector<Reply>& replys, Reply* reply);
    // 对传入数据进行预处理
    void Preprocess(ChinaMobileBundle &response, const ChinaMobileBundle &request); 
    // 调用底层自然语言分析模块，进行数据处理
    bool QueryUnderstanding(ChinaMobileBundle &response) ;   
    // Parser和FAQ模块处理及合并(域处理)
    string MergeDomain(string domainFaqResult, string parserResult, string ASRResult);
    // Parser和FAQ模块处理及合并(特殊情况处理及槽位合并)
    string MergeFaqQueryParser(string faqResult, string parserResult, string ASRResult);
    // void FillProtoPreprocess(const preprocess::QueryInfo& info, DataBundle& resp);

    // bool FormatResponse(DataBundle &response, int32_t n);

private:
    // 声明线程池
    scoped_ptr<ThreadPool> caller_pool_;
    // 声明FAQ的thrift对应的调用对象
    scoped_ptr<ThriftClientManager<TrioBrainServingClient> > trio_brain_client_manager_;
    // scoped_ptr<ThriftClientManager<FaqServingClient> > faq_client_mgr_;
    // 声明Parser的thrift对应的调用对象
    scoped_ptr<ThriftClientManager<QueryParserServingClient> > query_parser_client_manager_;
    // (未使用)
    preprocess::DocProcessHandlerManager query_preproc_mod_;
    // 声明删除预处理对象
    trie::JumpTrie removeWord_trie;
    // 声明预处理对象
    TrioQueryPreProcesser::QueryPreProcessor numericNormalizer;
};

} // namespace robot
#endif  // NLU_SERVING_HANDLER_H_
