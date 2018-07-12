

#include "NLU_serving_handler.h"

//#include <string>
//#include <map>

//#include "Common/Base/flags.h"
//#include "Common/Base/stringUtil.h"
//#include "Common/Util/Thread/callback.h"
#include "Common/Util/Encode/encodeUtil.h"
#include "Common/Util/File/simpleLineReader.h"

using std::map;
using std::string;
using std::vector;
using encode::GBK2UTF8;
using encode::UTF82GBK;

DEFINE_int32(caller_thread_num, 48, "");// 设置线程池最大线程数
DEFINE_string(backend_config_file, "../Config/backend.config", "");// 设置配置文件路径
DEFINE_string(preprocessor_lst, "", "");//未使用
DEFINE_string(faq_index, "faqchinamobile", "");//未使用
DEFINE_string(word_seg_tag_data_dir, "../Data/WordSegTag/data", "");//未使用
DEFINE_string(remove_word_file, "../Data/removeWords.txt", "remove word list");//
DEFINE_string(punct_file, "../Data/NumericNormalizer/punct.dic", "punct.dic");//query预处理文件
DEFINE_string(num_file, "../Data/NumericNormalizer/num.dic", "num.dic");//query预处理文件
DEFINE_string(punct_remove_file, "../Data/NumericNormalizer/spec_punct.dic", "spec_punct.dic");//query预处理文件

DEFINE_bool(debug, false, "");//未使用
DEFINE_bool(random_select, false, "");//未使用
DEFINE_int32(show_topN, 1, "");//未使用

DEFINE_int32(faq_platform, 10, "");//Thrift通讯参数
DEFINE_string(faq_bot_name, "ChinaMobile", "");//Thrift通讯参数，服务唯一标识
DEFINE_int32(faq_suggest_count, 0, ""); //FAQ配置项，POST分值介于suggest和best之间时回复的条数
DEFINE_int32(faq_recall_count, 25, "");//FAQ配置项，POST备选项条数
DEFINE_double(faq_best_reply_threshold, 0.7, "");//FAQ配置项，POST超过该分值则回复
DEFINE_double(faq_suggest_reply_threshold, 0.6, "");//FAQ配置项，POST超过该分值作为备选回复
DEFINE_string(faq_default_reply, "", "");//FAQ配置项，默认回复
DEFINE_string(faq_trace_id, "ChinaMobileFakeTraceId", "");//FAQ配置项
DEFINE_string(faq_esIndex, "faq_jlchinamobile", "");//FAQ es库索引

DEFINE_string(backends_service, "queryParser,faq", "");//子模块列表
#if 0
DEFINE_string(offline_business, "", "暂时下线业务并设置为集外业务");
DEFINE_string(keyword_avoid_business, "详单![action]=查询|[target]=账单", "关键字非特定业务，属于集外业务");
#endif
using namespace robot;
namespace chinaMobile {

bool NLUServingHandler::Init() {
    // 初始化线程池
    caller_pool_.reset(new ThreadPool(FLAGS_caller_thread_num));
    caller_pool_->StartWorkers();

    // 初始化子服务Thrift的调用对象
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
        trio_brain_client_manager_.reset(new ThriftClientManager<TrioBrainServingClient>(FLAGS_backend_config_file, FAQ_GROUP));
    }

    //加载配置文件
    file::SimpleLineReader reader(true); 
    VLOG(3) << "Loading removed word ...";
    if (!reader.Open(FLAGS_remove_word_file)) 
    {
        VLOG(3) << "Failed to open removed word file.";
        return false;
    }

    vector<string> lines;
    reader.ReadLines(&lines);
    for (size_t i = 0; i < lines.size(); ++i) {
        removeWord_trie.AddString(lines[i]);
    }
    removeWord_trie.Build();
    
    //CHECK(!FLAGS_preprocessor_lst.empty()) << "preprocessor_lst is empty";
    //query_preproc_mod_.InitByNames(FLAGS_preprocessor_lst);

    numericNormalizer.Init(FLAGS_punct_file, FLAGS_num_file, FLAGS_punct_remove_file);
#if 0
    // 暂时下线业务槽位处理--初始化要下线业务
    InitOffineBusinessInfo();
    // 针对容易误识别情况做下线处理--初始化
    InitOtherKeywordBusinessInfo();
#endif
    return true;
}
#if 0
void NLUServingHandler::InitOffineBusinessInfo()
{
    _offlineBusinessSlot.clear();
    if (!FLAGS_offline_business.empty()) {
        vector<string> offlineBusTmp;
        SplitString(FLAGS_offline_business, ',', &offlineBusTmp);
        for (vector<string>::iterator iter = offlineBusTmp.begin(); iter != offlineBusTmp.end(); ++iter) {
            _offlineBusinessSlot.push_back(*iter);
        }
    }
}

void NLUServingHandler::InitOtherKeywordBusinessInfo() {
    _other_keyword_business.clear();
    if (!FLAGS_keyword_avoid_business.empty()) {
        vector<string> otherKeywordBusinessTmp;
        SplitString(FLAGS_keyword_avoid_business, ',', &otherKeywordBusinessTmp);
        for (vector<string>::iterator iter = otherKeywordBusinessTmp.begin(); iter != otherKeywordBusinessTmp.end(); ++iter) {
            vector<string> keyword_other_items;
            SplitString(*iter, '!', &keyword_other_items);
            if (2 == keyword_other_items.size()) {
                _other_keyword_business[keyword_other_items[0]] = keyword_other_items[1];
            } else {
                 VLOG(5) << "OtherKeyword Format Error!";
            }
        }
    }
}
#endif
void NLUServingHandler::NLUProcess(ChinaMobileBundle &response, const ChinaMobileBundle &request) {

    int64 usec_a = common::GetTimeInMs();
    // VLOG(4) << "userId = " << request.userid;

    VLOG(1) << __FUNCTION__ << ", start, call_id: " << request.userid << " trace_id: " << request.trace_id;
    // 对传入数据进行预处理
    Preprocess(response, request);

    // 调用底层自然语言分析模块，进行数据处理
    if(QueryUnderstanding(response)){
        response.__set_ret(1); // 0 unknow  1 succeed  -1 failed 2 failed 3 timeout
    }
    else {
        response.__set_ret(-1);
    }
    int64 usec_b = common::GetTimeInMs();
    VLOG(0) << __FUNCTION__ << ", over, call_id: " << request.userid << " trace_id: " << request.trace_id << " call time : " << (usec_b - usec_a);
}

bool NLUServingHandler::QueryUnderstanding(ChinaMobileBundle &response) {
        
        VLOG(5) << "QueryUnderstanding";
        //const DataBundle& ref_request = response;
        DataBundle request;
        request.__set_query(response.ASRResult);
        request.__set_user_id(response.inparams.call_id);
        const DataBundle& ref_request = request;
        bool ret = true;
        
        
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
    
        //call query parser server
        //调用parser模块提取槽值对
        DataBundle queryParser_response;
        int64 st_time = common::GetTimeInMs();
        VLOG(1) << __FUNCTION__ << ", queryParser faq start, call_id: " << response.userid;
        if (backend_service_set.find("queryParser") != backend_service_set.end()) {
            caller_pool_->Add(common::NewOneTimeCallback(this,
                &NLUServingHandler::CallQueryParser, &ref_request, &queryParser_response, &block_counter));
        }
        
        //call faq server
        //调用faq模块识别意图
        TrioDataBundle faq_response;
        TrioDataBundle faq_request_temp;
        ConvertRequest(response,  &faq_request_temp);
        const TrioDataBundle& faq_request = faq_request_temp;

        if (backend_service_set.find("faq") != backend_service_set.end()) {
            caller_pool_->Add(common::NewOneTimeCallback(this,
                &NLUServingHandler::CallTrioBrain, &faq_request, &faq_response, &block_counter));
        }
    
        block_counter.Wait();
        VLOG(1) << __FUNCTION__ << ", queryParser faq over, call_id: " << response.userid << " call time : " << common::GetTimeInMs() - st_time;
        // FormatResponse(faq_response, FLAGS_show_topN);
        string queryParserResult = "";
        string faqResult = "";
        string realReply = "";
        if (queryParser_response.__isset.reply && !queryParser_response.reply.reply.empty()) 
        {//parser识别结果
            queryParserResult = queryParser_response.reply.reply;
            VLOG(4) << "hit query parser";
            VLOG(4) << "parser result = " + queryParserResult;
        }
        if(faq_response.__isset.resp && faq_response.resp.__isset.faq_reply_info 
           && !faq_response.resp.faq_reply_info.reply.empty()){
            //faq识别结果
            response.__set_ret(faq_response.resp.return_status);
            faqResult += faq_response.resp.faq_reply_info.reply;
            VLOG(4) << "hit faq";
            VLOG(4) << "faq result = " + faqResult;
        }
        //合并识别结果
        realReply = MergeDomain(faqResult, queryParserResult, response.ASRResult);
        response.__set_NLUResult(realReply);
        if(realReply.empty()){
            VLOG(4) << "hit no match";
            ret = false;
        }
        
        return ret ; 
    }

// Parser和FAQ模块处理及合并(域处理)
string NLUServingHandler::MergeDomain(string domainFaqResult, string parserResult, string ASRResult) {
    VLOG(5) << "domainFaqResult = " + domainFaqResult;
    VLOG(5) << "parserResult = " + parserResult;
    string trimDomainFaqResult;
    TrimWhitespace(domainFaqResult, TRIM_ALL , &trimDomainFaqResult);
    if (trimDomainFaqResult == "") {
        return MergeFaqQueryParser("", parserResult, ASRResult);
    }
    vector<string> domainFaq;
    SplitString(trimDomainFaqResult, '&', &domainFaq);
    string mergeDomainResult = "";
    string domainTab = "";
    for(size_t i = 0; i < domainFaq.size(); ++i) {
        if ( domainFaq[i].size() < 2 ) {
            VLOG(1) << __FUNCTION__ << ", domainFaq error, " << domainFaq[i];
            continue;
        }
        if ( domainFaq[i][0] == '{' ) {
            size_t pos = domainFaq[i].find('}');
            if ( pos != std::string::npos ) {
                mergeDomainResult += domainTab;
                mergeDomainResult += domainFaq[i].substr(0, pos+1);
                mergeDomainResult += MergeFaqQueryParser(domainFaq[i].substr(pos+1), parserResult, ASRResult);
            } else {
                VLOG(1) << __FUNCTION__ << ", DomanName error, " << domainFaq[i];
                continue;
            }
        } else {
            mergeDomainResult += domainTab;
            mergeDomainResult += MergeFaqQueryParser(domainFaq[i], parserResult, ASRResult);
        }
        domainTab = '&';
    }
    VLOG(5) << "Merge Domain Result = " + mergeDomainResult;
    return mergeDomainResult;
}

// Parser和FAQ模块处理及合并(特殊情况处理及槽位合并)
string NLUServingHandler::MergeFaqQueryParser(string faqResult, string parserResult, string ASRResult)
{
    bool isShortQuery = false;
    bool isOutsideBusiness = false;
    map<string, string> kvMap;
    
    vector<string> faqVec;
    if (faqResult != "") SplitString(faqResult, '|', &faqVec);
    vector<string> parserVec;
    if (parserResult != "") SplitString(parserResult, '|', &parserVec);
    
    
    for(size_t i = 0; i < parserVec.size(); ++i)
    {
        if (parserVec[i] == "[short_query]=短拒识")
        {
            isShortQuery = true;
        }
        else if (parserVec[i] == "[business_tag]=集外")
        {
            isOutsideBusiness = true;
        }
        else
        {
            vector<string> kvp;
            SplitString(parserVec[i], '=', &kvp);
            if (kvp.size() == 2)
            {
                kvMap[kvp[0]] = kvp[1];
            }
#if 0
            if(kvp[0] == "[flow_business_object_type]")
            {
                if(kvp[1] == "10元流量包")
                {
                    kvMap["[money]"] = "10";
                }
                else if (kvp[1] == "20元流量包")
                {
                    kvMap["[money]"] = "20";
                }
            }
#endif
            // finalResult += tab;
            // finalResult += parserVec[i];
            // tab = "|";
        }
    }
    for(size_t i = 0; i < faqVec.size(); ++i)
    {
        vector<string> kvp;
        SplitString(faqVec[i], '=', &kvp);
        if (kvp.size() == 2)
        {
            kvMap[kvp[0]] = kvp[1];
        }

        // finalResult += tab;
        // finalResult += faqVec[i];
        // tab = "|";
    }
#if 0
    // switch "五十流量包" to "五十元流量包"
    if (kvMap.count("[target]") > 0)
    {
        if (kvMap["[target]"] == "套餐" || kvMap["[target]"] == "流量" )
        {
            if (kvMap.count("[money]") == 0 && kvMap.count("[number]") > 0)
            {
                string numberKey = "[number]";
                string numberValue = kvMap["[number]"];
                kvMap["[money]"] = numberValue;
            }
        }
    }
#endif
    // genenrate the result based on dict
    string finalResult = "";
    string tab = "";
    map <string, string>::iterator iter;
    for( iter=kvMap.begin(); iter!=kvMap.end(); iter++)
    {
        string slotString = iter->first + "=" + iter->second;
        finalResult += tab;
        finalResult += slotString;
        tab = "|";
    }
    if (isShortQuery && finalResult == "")
    {
        finalResult = "[action]=集外说法|[target]=短拒识";
    }
    else if (isOutsideBusiness && finalResult == "")
    {
        finalResult = "[action]=集外说法|[target]=集外业务咨询";
    }
#if 0
    else if(0 != (int)(_offlineBusinessSlot.size()) && "" != finalResult) {
        if(0 == IsOfflineBusiness(kvMap)) {
            finalResult = "[action]=集外说法|[target]=集外业务咨询";
            VLOG(3) << "current business is offlinebusiness, Merge Result = " + finalResult;
        }
    }
    if (0 != (int)(_other_keyword_business.size()) && "" != finalResult) {
        if(0 == IsKeywordOtherBusiness(kvMap, ASRResult)) {
            finalResult = "[action]=集外说法|[target]=集外业务咨询";
            VLOG(3) << "current business is keyword To other business, Merge Result = " + finalResult;
        }
    } else {
#endif
    else {
        //No Need
    }
    VLOG(4) << "Merge Result = " + finalResult;
    return finalResult;
}
// 对传入数据进行预处理 
void NLUServingHandler::Preprocess(ChinaMobileBundle &response, const ChinaMobileBundle &request) {

    VLOG(5) << "Raw query:" << request.ASRResult;
    string query = common::to_lower(request.ASRResult);

    //find common pattern 
    vector<const string*> matches;
    removeWord_trie.LongestMatch(query, &matches);  
    for(size_t i = 0; i < matches.size(); i++)
    {
        ReplaceSubstringsAfterOffset(&query, 0, *matches[i], "");
      //  VLOG(5) << "removed word:"<< *matches[i] <<"; query: "<< query;
    }
    if (query != "")
    {
        numericNormalizer.Normalize(query, query);
        VLOG(5) << "After Normalize, the query :" << query;
    }
    response = request;
    response.ASRResult = query;
    //VLOG(5) << "request:" << FromThriftToUtf8DebugString(&response);
    /*
    preprocess::QueryInfo query_info;
    query_info.query = query;
    // query_preproc_mod_.ProcessDoc(query_info);
    FillProtoPreprocess(query_info, response);    

    response.user_info.uid = "test";

    VLOG(5) << "Preqrocessed query:" << query_info.query;*/
}





/*
bool NLUServingHandler::FormatResponse(DataBundle &response, int32_t n) {

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
}*/

/*
void NLUServingHandler::FillProtoPreprocess(const preprocess::QueryInfo& query_info, DataBundle& response)
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
}*/

// 生成传递给FAQ的Bundle
void NLUServingHandler::ConvertRequest(const ChinaMobileBundle &request, TrioDataBundle* trio_request) {
    
    string question = request.ASRResult;
    
    robot::UserInfo user_info;
    user_info.__set_uid(request.userid);
    user_info.__set_platform(FLAGS_faq_platform);
    user_info.__set_bot_name(FLAGS_faq_bot_name);
    trio_request->__set_user(user_info);
    trio_request->__isset.params = true;
    trio_request->params.__set_is_debug(false);

    robot::FaqControlParams faq_params;
    faq_params.__set_suggest_count(FLAGS_faq_suggest_count);
    faq_params.__set_recall_count(FLAGS_faq_recall_count);
    faq_params.__set_best_reply_threshold(FLAGS_faq_best_reply_threshold);
    faq_params.__set_suggest_reply_threshold(FLAGS_faq_suggest_reply_threshold);
    faq_params.__set_default_reply(FLAGS_faq_default_reply);
    faq_params.__set_enable_faq_redux(false);
    faq_params.__set_detail_info(false);
    trio_request->params.__set_faq_params(faq_params);
    trio_request->params.__set_trace_id(FLAGS_faq_trace_id);
            
    trio_request->__set_query(question);
    trio_request->params.faq_params.__set_es_index(FLAGS_faq_esIndex);
}

void NLUServingHandler::ConvertResponse(const TrioDataBundle &trio_response, ChinaMobileBundle* response) {
    /*
    // return status
    response->__set_ret(trio_response.resp.return_status);
    
    // reply from service
    response->__set_reply_from_service(trio_response.resp.reply_from_service);
    
    // reply info
    string reply_info = GenerateReplyJson(trio_response.resp.faq_reply_info);
    response->__set_faq_reply_info(reply_info);

    // debug message
    DebugInfo di;
    string debugInfo = "{}";
    if(trio_response.params.faq_params.detail_info) {
        debugInfo = GenerateDebugJson(trio_response.resp.faq_debug_fixture);
    }
    di.__set_debug_message(debugInfo);
    response->__set_debug_info(di);

    // replies
    if (!trio_response.resp.replys.empty()) {
        // set best reply
        if (trio_response.resp.replys[0].score > trio_response.params.faq_params.best_reply_threshold) {
            Reply best_reply;
            best_reply.__set_reply(trio_response.resp.replys[0].original_post + '\001' + trio_response.resp.replys[0].answer);
            best_reply.__set_score(trio_response.resp.replys[0].score);
            response->__set_reply(best_reply);
        }
        // set suggest replies
        vector<robot::Reply> replies;
        for (size_t i = 0; i < trio_response.resp.replys.size(); i++) {
            if (i == 0 && trio_response.resp.replys[i].score > trio_response.params.faq_params.best_reply_threshold) {
                // this is best reply, ignore
                continue;
            }
            robot::Reply sugg_reply;
            sugg_reply.__set_reply(trio_response.resp.replys[i].original_post);
            sugg_reply.__set_answer(trio_response.resp.replys[i].answer);
            sugg_reply.__set_score(trio_response.resp.replys[i].score);
            replies.push_back(sugg_reply);
        }
        response->__set_replys(replies);
        response->__isset.replys = true;
    }
    */
}
#if 0
// 暂时下线业务槽位处理--判断业务是否是要下线业务
int NLUServingHandler::IsOfflineBusiness(map<string, string> &replyMap)
{
    int ret = -1;

    string replyStr = "";
    for (map<string,string>::iterator iter = replyMap.begin(); iter != replyMap.end(); ++iter) {
        if ("[action]" == iter->first) {
            replyStr += iter->first + "=" + iter->second;
            break;
        }
    }
    for (map<string,string>::iterator iter = replyMap.begin(); iter != replyMap.end(); ++iter) {
        if ("[target]" == iter->first) {
            if ("" != replyStr) {
                replyStr += "|";
            }
            replyStr += iter->first + "=" + iter->second;
            break;
        }
    }

    for(vector<string>::iterator iter = _offlineBusinessSlot.begin(); iter != _offlineBusinessSlot.end(); ++iter) {
        if(*iter == replyStr) {
            ret = 0;
            break;
        }
    }

    return ret;
}

// 针对容易误识别情况做下线处理--判定
int NLUServingHandler::IsKeywordOtherBusiness(map<string, string> &replyMap, string ASRResult) {
    int ret = -1;
    for(map<string,string>::iterator iter_keymap = _other_keyword_business.begin(); iter_keymap != _other_keyword_business.end(); ++iter_keymap) {
        if (ASRResult.find(iter_keymap->first) != string::npos ) {
            VLOG(3) << "current business other keyword = " + iter_keymap->first;
            string replyStr = "";
            for (map<string,string>::iterator iter = replyMap.begin(); iter != replyMap.end(); ++iter) {
                if ("[action]" == iter->first) {
                    replyStr += iter->first + "=" + iter->second;
                    break;
                }
            }
            for (map<string,string>::iterator iter = replyMap.begin(); iter != replyMap.end(); ++iter) {
                if ("[target]" == iter->first) {
                    if ("" != replyStr) {
                        replyStr += "|";
                    }
                    replyStr += iter->first + "=" + iter->second;
                    break;
                }
            }
            if(iter_keymap->second == replyStr) {
                ret = 0;
                break;
            }
        }
    }

    return ret;
}
#endif
// 通过宏来定义调用子模块thrift相关的函数
REGISTER_CALL_CHINAMOBILE_FAQ_SERVICE(NLUServingHandler, TrioBrain, trio_brain_client_manager_, "TrioBrain")
//REGISTER_CALL_RPC_SERVICE(NLUServingHandler, Faq, faq_client_mgr_, "Faq")
REGISTER_CALL_RPC_SERVICE(NLUServingHandler, QueryParser, query_parser_client_manager_, "QueryParser")
} // namespace robot
