

#include "query_understanding_serving_handler.h"

//#include <string>
//#include <map>

//#include "Common/Base/flags.h"
//#include "Common/Base/stringUtil.h"
//#include "Common/Util/Thread/callback.h"
#include "Common/Util/Encode/encodeUtil.h"
//#include "Common/Util/File/simpleLineReader.h"

using std::map;
using std::string;
using std::vector;
using encode::GBK2UTF8;
using encode::UTF82GBK;

DEFINE_int32(caller_thread_num, 48, "");
DEFINE_string(backend_config_file, "../Config/backend.config", "");
DEFINE_string(preprocessor_lst, "", "");
DEFINE_string(faq_index, "faqchinamobile", "");
DEFINE_string(word_seg_tag_data_dir, "../Data/WordSegTag/data", "");
DEFINE_string(keywords_file, "../Data/keywords.txt", "keywords list");


DEFINE_bool(debug, false, "");
DEFINE_bool(random_select, false, "");
DEFINE_int32(show_topN, 1, "");


DEFINE_string(backends_service, "queryParser,faq", "");


namespace robot {

bool QueryUnderstandingServingHandler::Init() {
    caller_pool_.reset(new ThreadPool(FLAGS_caller_thread_num));
    caller_pool_->StartWorkers();

    vector<string> backend_service_vec;
    set<string> backend_service_set;
    SplitString(FLAGS_backends_service, ',', &backend_service_vec);
    for(size_t i = 0; i < backend_service_vec.size(); ++i){
        if (backend_service_vec[i].empty()) {
            continue;
        }
        backend_service_set.insert(backend_service_vec[i]);
    }
    if (backend_service_set.find("queryParser") != backend_service_set.end()) {
        query_parser_client_manager_.reset(new ThriftClientManager<QueryParserServingClient>(FLAGS_backend_config_file, QUERYPARSER_GROUP));
    }

    if (backend_service_set.find("faq") != backend_service_set.end()) {
        faq_client_mgr_.reset(new ThriftClientManager<FaqServingClient>(FLAGS_backend_config_file, FAQ_GROUP));
    }


    CHECK(!FLAGS_preprocessor_lst.empty()) << "preprocessor_lst is empty";
    query_preproc_mod_.InitByNames(FLAGS_preprocessor_lst);

    return true;
}

void QueryUnderstandingServingHandler::QueryUnderstandingProcess(DataBundle &response, const DataBundle &request) {


    if (!request.__isset.query) {
        VLOG(2) << "Meet empty query";
        return;
    }
    VLOG(3) << "query:" << request.query;
    string query = common::to_lower(request.query);
    response = request;
    VLOG(5) << "request:" << FromThriftToUtf8DebugString(&response);
    
    preprocess::QueryInfo query_info;
    query_info.query = query;
    // query_preproc_mod_.ProcessDoc(query_info);
    FillProtoPreprocess(query_info, response);


    response.user_info.uid = "test";
    const DataBundle& ref_request = response;
    
    VLOG(2) << "query:" << ref_request.query;


    vector<string> backend_service_vec;
    set<string> backend_service_set;
    SplitString(FLAGS_backends_service, ',', &backend_service_vec);
    for(size_t i = 0; i < backend_service_vec.size(); ++i){
        if (backend_service_vec[i].empty()) {
            continue;
        }
        backend_service_set.insert(backend_service_vec[i]);
    }
 
    BlockingCounter block_counter(backend_service_set.size());

    /******* call query parser server *******/
    DataBundle queryParser_response;
    if (backend_service_set.find("queryParser") != backend_service_set.end()) {
        caller_pool_->Add(common::NewOneTimeCallback(this,
            &QueryUnderstandingServingHandler::CallQueryParser, &ref_request, &queryParser_response, &block_counter));
    }

    /******* call faq server *******/
    DataBundle faq_response;
    DataBundle faq_request = ref_request;
    if (backend_service_set.find("faq") != backend_service_set.end()) {
        if (faq_request.__isset.query && !faq_request.query.empty()) {
            faq_request.query = FLAGS_faq_index + "\001" + faq_request.query;
        }
        caller_pool_->Add(common::NewOneTimeCallback(this,
            &QueryUnderstandingServingHandler::CallFaq, &((const DataBundle &)faq_request), &faq_response, &block_counter));
    }

    block_counter.Wait();

    FormatResponse(faq_response, FLAGS_show_topN);


    if (queryParser_response.__isset.reply && !queryParser_response.reply.reply.empty()) {
        response = queryParser_response;
        response.__set_reply_from_service(ServiceName::QUERYPARSER);
        VLOG(4) << "hit query parser";
    } else if (faq_response.__isset.reply && !faq_response.reply.reply.empty()) {
        response = faq_response;
        
        vector<string> reply_vec;
        SplitString(response.reply.reply, '\001', &reply_vec);

        if (reply_vec.size() > 1)
        {
            response.reply.reply = reply_vec[1];
        }

        response.__set_reply_from_service(ServiceName::FAQ);
        VLOG(4) << "hit faq";   
    }else {
        response.reply.__isset.reply=true;
        response.__isset.reply=true;
        response.reply.reply = "我们还在不断的学习中，稍后会有客服人员为您解答！";
        response.__set_reply_from_service(ServiceName::FAQ);
        VLOG(4) << "hit faq";
    }



    VLOG(5) << "result:" << FromThriftToUtf8DebugString(&response);
}

bool QueryUnderstandingServingHandler::FormatResponse(DataBundle &response, int32_t n) {

    int32_t reply_num = 0;
    if ((response.__isset.reply && !response.reply.reply.empty())){
        reply_num++;
        vector<string> qa;
        SplitString(response.reply.reply, '\001', &qa);
        if (qa.size() == 2 && FLAGS_debug == false) {
            response.reply.reply = qa[1];
        }
    }
    if (response.__isset.replys && !response.replys.empty()) {

        for (size_t idx = 0; idx < response.replys.size(); ++idx) {
            if (!response.replys[idx].__isset.reply || 
                    response.replys[idx].reply.empty()) {

                continue;
            }
            vector<string> qa;
            SplitString(response.replys[idx].reply, '\001', &qa);
            if (qa.size() == 2 && FLAGS_debug == false) {
                response.replys[idx].reply = qa[1];
            }
            if (reply_num < n) {
                response.reply.reply += ";" + response.replys[idx].reply;
                reply_num++;
            }
        }
    }
    return true;
}


void QueryUnderstandingServingHandler::FillProtoPreprocess(const preprocess::QueryInfo& query_info, DataBundle& response)
{

    string query_basic_seg_str("");
    GBK2UTF8(query_info.basic_seg, query_basic_seg_str);
    VLOG(5) << "basic" << query_basic_seg_str;

    vector<string> query_basic_seg_vec;
    SplitString(query_basic_seg_str, ' ', &query_basic_seg_vec);
    response.__set_query_basic_seg(query_basic_seg_vec);

    string query_phrase_seg_str("");
    GBK2UTF8(query_info.phrase_seg, query_phrase_seg_str);
    VLOG(5) << "phrase" << query_phrase_seg_str;

    vector<string> query_phrase_seg_vec;
    SplitString(query_phrase_seg_str, ' ', &query_phrase_seg_vec);
    response.__set_query_phrase_seg(query_phrase_seg_vec);

    string preprocessed_query;
    GBK2UTF8(query_info.preprocessed_query, preprocessed_query);
    response.__set_preprocessed_query(preprocessed_query);

    string preprocessed_query_basic_seg_str("");
    GBK2UTF8(query_info.preprocessed_basic_seg, preprocessed_query_basic_seg_str);
    vector<string> preprocessed_query_basic_seg_vec;
    SplitString(preprocessed_query_basic_seg_str, ' ', &preprocessed_query_basic_seg_vec);
    response.__set_preprocessed_query_basic_seg(preprocessed_query_basic_seg_vec);

    string preprocessed_query_phrase_seg_str("");
    GBK2UTF8(query_info.preprocessed_phrase_seg, preprocessed_query_phrase_seg_str);
    vector<string> preprocessed_query_phrase_seg_vec;
    SplitString(preprocessed_query_phrase_seg_str, ' ', &preprocessed_query_phrase_seg_vec);
    response.__set_preprocessed_query_phrase_seg(preprocessed_query_phrase_seg_vec);
}

REGISTER_CALL_RPC_SERVICE(QueryUnderstandingServingHandler, Faq, faq_client_mgr_, "Faq")
REGISTER_CALL_RPC_SERVICE(QueryUnderstandingServingHandler, QueryParser, query_parser_client_manager_, "QueryParser")
} // namespace robot
