#include "CRSBrain_serving_handler.h"

//#include <string>
//#include <map>

//#include "Common/Base/flags.h"
//#include "Common/Base/stringUtil.h"
//#include "Common/Util/Thread/callback.h"
//#include "Common/Util/Encode/encodeUtil.h"
//#include "Common/Util/File/simpleLineReader.h"

#include "Projects/ChinaMobile/CRS/Src/Interface/HttpOperator.h"
//#include "Projects/ChinaMobile/CRS/Src/Interface/OperateLogInterface.h"
#include "Projects/ChinaMobile/CRS/Src/Interface/OperateRedisInterface.h"

//using std::map;
using std::string;
using std::vector;
//using encode::GBK2UTF8;
//using encode::UTF82GBK;

//DEFINE_int32(caller_thread_num, 48, "");
DEFINE_string(backend_config_file, "../Config/backend.config", "");
//DEFINE_string(preprocessor_lst, "", "");

//DEFINE_bool(debug, false, "");
//DEFINE_bool(random_select, false, "");
//DEFINE_int32(show_topN, 1, "");

DEFINE_string(backends_service, "NLU,ChinaMobileCRS", "");

DEFINE_string(access_control_key, "UVN_CAC_TOKEN_", "");
//DEFINE_string(java_redis_url, "http://139.219.69.127:8080", "");

DEFINE_bool(IsCheckCAC, true, "");
DEFINE_bool(IsFakeASR, false, "");

DEFINE_string(timeout_response, "[action]=超时|[target]=1", "");
DEFINE_string(nomatch_response, "[action]=拒识|[target]=1", "");

using namespace robot;
using namespace ChinaMobileProvince;
namespace chinaMobile {

bool CRSBrainServingHandler::Init()
{
    VLOG(1) << __FUNCTION__;

    //init java redis
    //InitializeHttpPost2Java(FLAGS_java_redis_url);

    // init caller pool
    //caller_pool_.reset(new ThreadPool(FLAGS_caller_thread_num));
    //caller_pool_->StartWorkers();
    
    // init service manager
    vector<string> backend_service_vec;
    set<string> backend_service_set;
    SplitString(FLAGS_backends_service, ',', &backend_service_vec);
    for(size_t i = 0; i < backend_service_vec.size(); ++i)
    {
        if (backend_service_vec[i].empty())
        {
            continue;
        }
        backend_service_set.insert(backend_service_vec[i]);
    }
    if (backend_service_set.find("NLU") != backend_service_set.end())
    {
        nlu_client_mgr_.reset(new ThriftClientManager<NLUServingClient>(FLAGS_backend_config_file, NLU_GROUP));
    }
    if (backend_service_set.find("ChinaMobileCRS") != backend_service_set.end())
    {
        chinaMobileCRS_client_mgr_.reset(new ThriftClientManager<ChinaMobileCRSServingClient>(FLAGS_backend_config_file, CHINA_MOBILE_CRS_GROUP));
    }
    
    return true;
}

void CRSBrainServingHandler::CRSBrainProcess(ChinaMobileBundle &response, const ChinaMobileBundle &request)
{
    // VLOG(4) << "userId = " << request.userid;
    int64 st_time = common::GetTimeInMs();
    VLOG(1) << __FUNCTION__ << ", start, call_id: " << request.userid << " trace_id: " << request.trace_id;
    ChinaMobileBundle CRSRequest = request;
    ChinaMobileBundle CRSResponse;
    CRSResponse.__set_ret(0);
    // Access Control
    if (request.inaction == 8)
    {
        if (!FLAGS_IsCheckCAC)
        {
            BlockingCounter crs_block_counter(1);
            PrintRequest(CRSRequest);
            CRSBrainServingHandler::CallChinaMobileCRS(&CRSRequest, &CRSResponse, &crs_block_counter);
            PrintResponse(CRSResponse);
            response = CRSResponse;
        }
        else
        {
            string accessControlKey = FLAGS_access_control_key + request.inparams.auth_token;
            bool accessControlKeyIsExist = false;
            if(RedisOperator_Exist(accessControlKey, accessControlKeyIsExist))
            {
                // if not exist the access key; pass the request and return with outaction == 10
                if(!accessControlKeyIsExist)
                {
                    response.__set_userid(request.userid);
                    response.__set_ret(0);
                    response.__set_outaction(10);
                    OutParams outparams;
                    outparams.__set_call_id(request.inparams.call_id);
                    response.__set_outparams(outparams);
                    response.__set_trace_id(request.trace_id);
                    VLOG(1) << __FUNCTION__ << ", over Redis Not Exist, call_id: " << request.userid << " trace_id: " << request.trace_id <<  " call time : " << common::GetTimeInMs() - st_time;
                    return;
                }
                else
                {
                    BlockingCounter crs_block_counter(1);
                    PrintRequest(CRSRequest);
                    CRSBrainServingHandler::CallChinaMobileCRS(&CRSRequest, &CRSResponse, &crs_block_counter);
                    PrintResponse(CRSResponse);
                    response = CRSResponse;
                }
            }
        }
    }
    
    //ASR Control
    else if (request.inaction == 9)
    {
        // 输入为语音识别结果
        if (request.inparams.flow_result_type == "1")
        {
            if(request.inparams.input != "" && request.inparams.input != "Hangup" && request.inparams.input != "timeout" && request.inparams.input != "sys_err" && request.inparams.input != "playover" && request.inparams.input != "nomatch")
            {
                string ASRResultKey = request.inparams.input;
                string ASRResultValue = "";
                if(FLAGS_IsFakeASR)
                {
                    VLOG(4) << "Fake ASR";
                    ChinaMobileBundle NLURequest;
                    NLURequest = request;
                    NLURequest.__set_ASRResult(ASRResultKey);
                    ChinaMobileBundle NLUResponse;
                    BlockingCounter nlu_block_counter(1);
                    CRSBrainServingHandler::CallNLU(&NLURequest, &NLUResponse, &nlu_block_counter);
                    CRSRequest.__set_ASRResult(ASRResultKey);
                    if (NLUResponse.NLUResult == "")
                    {
                        VLOG(4) << "Fake ASR reutrn empty, nomatch";
                        CRSRequest.__set_NLUResult(FLAGS_nomatch_response);
                    }
                    else 
                    {
                        VLOG(4) << "Fake ASR reutrn " << NLUResponse.NLUResult;
                        CRSRequest.__set_NLUResult(NLUResponse.NLUResult);
                    }
                }
                // 从redis中获取语音转写的文本内容，然后调用NLU进行解析，并将NLU解析结果写入到request的NLUResult中
                else if(RedisOperator_Get(ASRResultKey, ASRResultValue) && !FLAGS_IsFakeASR)
                {
                    VLOG(4) << "Real ASR";
                    ChinaMobileBundle NLURequest;
                    NLURequest = request;
                    
                    NLURequest.__set_ASRResult(ASRResultValue);
                    ChinaMobileBundle NLUResponse;
                    BlockingCounter nlu_block_counter(1);
                    CRSBrainServingHandler::CallNLU(&NLURequest, &NLUResponse, &nlu_block_counter);
                    CRSRequest.__set_ASRResult(ASRResultValue);
                    if (NLUResponse.NLUResult == "")
                    {
                        VLOG(4) << "Real ASR reutrn empty, nomatch";
                        CRSRequest.__set_NLUResult(FLAGS_nomatch_response);
                    }
                    else 
                    {
                        VLOG(4) << "Real ASR reutrn empty, nomatch";
                        CRSRequest.__set_NLUResult(NLUResponse.NLUResult);
                    }
                }
            }
            else if (request.inparams.input == "timeout")
            {
                VLOG(4) << "input is timeout";
                CRSRequest.__set_ASRResult(request.inparams.input);
                CRSRequest.__set_NLUResult(FLAGS_timeout_response);
            }
            else if (request.inparams.input == "nomatch" || request.inparams.input == "Hangup" || request.inparams.input == "playover" || request.inparams.input == "")
            {
                VLOG(4) << "input is " << request.inparams.input;
                CRSRequest.__set_ASRResult(request.inparams.input);
                CRSRequest.__set_NLUResult(FLAGS_nomatch_response);
            }
            else
            {
                VLOG(4) << "Invalid input";
                CRSRequest.__set_ASRResult(request.inparams.input);
                CRSRequest.__set_NLUResult(FLAGS_nomatch_response);
            }
        }
        // 输入为按键输入
        else if (request.inparams.flow_result_type == "2")
        {
            VLOG(4) << "flow_result_type is 2";
            if (request.inparams.input == "timeout")
            {
                VLOG(4) << "input is timeout";
                CRSRequest.__set_ASRResult(request.inparams.input);
                CRSRequest.__set_NLUResult(FLAGS_timeout_response);
            }
            else
            {
                CRSRequest.__set_ASRResult(request.inparams.input);
                CRSRequest.__set_NLUResult("[dtmf]=" + request.inparams.input);
            }
        }
        // 3 正常无输入，-1 其他
        else if (request.inparams.flow_result_type == "3" || request.inparams.flow_result_type == "-1")
        {
            VLOG(4) << "flow_result_type is " << request.inparams.flow_result_type;
            if (request.inparams.input == "timeout")
            {
                VLOG(4) << "input is timeout";
                CRSRequest.__set_ASRResult(request.inparams.input);
                CRSRequest.__set_NLUResult(FLAGS_timeout_response);
            }
            else
            {
                CRSRequest.__set_ASRResult("");
                CRSRequest.__set_NLUResult(FLAGS_nomatch_response);
            }
        }
        else
        {
            VLOG(4) << "Invalid flow_result_type";
            CRSRequest.__set_ASRResult("");
            CRSRequest.__set_NLUResult(FLAGS_nomatch_response);
        }
        VLOG(4) << "CRSRequest.ASRResult = " << CRSRequest.ASRResult;
        VLOG(4) << "CRSRequest.NLUResult = " << CRSRequest.NLUResult;
        BlockingCounter crs_block_counter(1);
        PrintRequest(CRSRequest);
        CRSBrainServingHandler::CallChinaMobileCRS(&CRSRequest, &CRSResponse, &crs_block_counter);
        PrintResponse(CRSResponse);
        response = CRSResponse;
    }
    else
    {
        BlockingCounter crs_block_counter(1);
        PrintRequest(CRSRequest);
        CRSBrainServingHandler::CallChinaMobileCRS(&CRSRequest, &CRSResponse, &crs_block_counter);
        PrintResponse(CRSResponse);
        response = CRSResponse;
    }
    response = CRSResponse;
    VLOG(1) << __FUNCTION__ << ", over, call_id: " << response.userid << " trace_id: " << response.trace_id << " call time : " << common::GetTimeInMs() - st_time;
}

void CRSBrainServingHandler::PrintRequest(const ChinaMobileBundle &request)
{
    // VLOG(4) << "request.userid = " << request.userid;
    VLOG(4) << "[" << request.userid << "] request.inaction = " << request.inaction;
    VLOG(4) << "[" << request.userid << "] request.authResult = " << request.authResult;
    VLOG(4) << "[" << request.userid << "] request.NLUResult = " << request.NLUResult;
    VLOG(4) << "[" << request.userid << "] request.ASRResult = " << request.ASRResult;
    if (request.inaction == 9)
    {
        VLOG(4) << "[" << request.userid << "] request.inparams.flow_result_type = " << request.inparams.flow_result_type;
        VLOG(4) << "[" << request.userid << "] request.inparams.input = " << request.inparams.input;
    }
}
void CRSBrainServingHandler::PrintResponse(const ChinaMobileBundle &response)
{
    // VLOG(4) << "response.userid = " << response.userid;
    VLOG(4) << "[" << response.userid << "] response.ret = " << response.ret;
    VLOG(4) << "[" << response.userid << "] response.outaction = " << response.outaction;
    if (response.outaction == 9)
    {
        VLOG(4) << "[" << response.userid << "] response.outparams.model_type = " << response.outparams.model_type;
        VLOG(4) << "[" << response.userid << "] response.outparams.prompt_type = " << response.outparams.prompt_type;
        VLOG(4) << "[" << response.userid << "] response.outparams.prompt_wav = " << response.outparams.prompt_wav;
        VLOG(4) << "[" << response.userid << "] response.outparams.prompt_text = " << response.outparams.prompt_text;
    }
}

REGISTER_CALL_CHINAMOBILE_SERVICE(CRSBrainServingHandler, NLU, nlu_client_mgr_, "nlu")
REGISTER_CALL_CHINAMOBILE_SERVICE(CRSBrainServingHandler, ChinaMobileCRS, chinaMobileCRS_client_mgr_, "ChinaMobileCRS")
}
