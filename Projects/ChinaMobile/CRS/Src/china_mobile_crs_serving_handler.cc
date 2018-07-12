#include "china_mobile_crs_serving_handler.h"
#include "Projects/ChinaMobile/CRS/Src/Interface/HttpOperator.h"
#include "Projects/ChinaMobile/ConfigurationManager/ConfigurationManager.h"
#include "Projects/ChinaMobile/CRS/Src/CRSLogger/CSFInterfaceLogger.h"
#include "Projects/ChinaMobile/CRS/Src/Session/ChinaMobileSession.h"

DEFINE_string( backend_config_file, "../Config/backend.config", "" );
DEFINE_string( csf_config_file, "../Config/config.ini", "" );
DEFINE_string( config_file, "../Data/TaskFlow/Config/Config.json", "" );
DEFINE_string( timeout, "5", "超时时间" );
DEFINE_bool(china_mobile_incalllog, false, "incall 日志开关");
DEFINE_bool(china_mobile_ivrcrslog, false, "IVR-CRS交互日志开关");

#define PARSER_DMAN_SPLIT '&'
#define PARSER_ITEM_SPLIT '|'

#define ST_TIME_IN_CRS st_time
#define CAL_TIME_IN_CRS common::GetTimeInMs() - st_time

namespace chinaMobile {

CChinaMobileCRSServingHandler::CChinaMobileCRSServingHandler() {
}

CChinaMobileCRSServingHandler::~CChinaMobileCRSServingHandler() {
    for (TSession2CoreAgentInfo::iterator iter = _session_2_core.begin();iter != _session_2_core.end(); ++iter ) {
        if ( iter->second.pCoreAgent != NULL ) {
            delete iter->second.pCoreAgent;
        }
    }
    _session_2_core.clear();
}

bool CChinaMobileCRSServingHandler::Init() {
    VLOG(DATA) << __FUNCTION__;
    
    if ( TrioTDM::Config::Instance() == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", Config::Instance() is NULL.";
        return false;
    }
#if 0
    ConfigurationManager::ConfigurationManager csfConfigManager;
    if (!csfConfigManager.Init(FLAGS_csf_config_file)){
        VLOG(FATAL) << __FUNCTION__ << ", CSFConfig::Init() is Failed."<<"Filepath:" << FLAGS_csf_config_file;
        return false;
    }
#endif
    if ( !TrioTDM::Config::Instance()->init(FLAGS_config_file) ) {
        VLOG(FATAL) << __FUNCTION__ << ", Config init fail. file: " << FLAGS_config_file;
        return false;
    }

#ifdef PERSISTENCE_REDIS
    if ( TrioDM::RedisWrapper::Instance() == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", TrioTDM::RedisWrapper::Instance() is NULL.";
        return false;
    }

    if ( !TrioDM::RedisWrapper::Instance()->Init("127.0.0.1", 6379, "1234567Myc") ) {
    //if ( !TrioDM::RedisWrapper::Instance()->Init("127.0.0.1", 6389) ) {
        VLOG(0) << __FUNCTION__ << ", TrioDM::RedisWrapper::Instance()->InitTrioDM::RedisWrapper::Instance Init fail.";
        return false;
    }
#endif
    
    if ( TrioTDM::CNLG::Instance() == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", CNLG::Instance() is NULL.";
        return false;
    }
    std::string nlg_file = TrioTDM::Config::Instance()->get_value("task_dman/file/nlg");
    if ( !TrioTDM::CNLG::Instance()->Init( nlg_file ) ) {
        VLOG(FATAL) << __FUNCTION__ << ", CNLG Init fail.";
        return false;
    }
#if 0
    std::string java_http_url = TrioTDM::Config::Instance()->get_value("session/url/java_http");
    VLOG(DEBUG) << __FUNCTION__ << ", JavaHttpURL: " << java_http_url;
    ChinaMobileProvince::InitializeHttpPost2Java(java_http_url);
#endif
    if ( !InitParamFromConfig() ) {
        VLOG(FATAL) << __FUNCTION__ << ", InitParamFromConfig fail.";
        return false;
    }

    if ( !InitParam2SetFunc() ) {
        VLOG(FATAL) << __FUNCTION__ << ", InitParam2SetFunc fail.";
        return false;
    }

    if ( !TrioTDM::InitDialogTaskOnBeginSession() ) {
        VLOG(FATAL) << "InitDialogTaskOnBeginSession fail..";
        return false;
    }

    return true;
}

bool CChinaMobileCRSServingHandler::InitParam2SetFunc() {
    _param_2_set_func.insert(std::make_pair("call_id", &OutParams::__set_call_id));
    _param_2_set_func.insert(std::make_pair("inter_idx", &OutParams::__set_inter_idx));
    _param_2_set_func.insert(std::make_pair("flow_type", &OutParams::__set_flow_type));
    _param_2_set_func.insert(std::make_pair("flow_node_name", &OutParams::__set_flow_node_name));
    _param_2_set_func.insert(std::make_pair("model_type", &OutParams::__set_model_type));
    _param_2_set_func.insert(std::make_pair("prompt_type", &OutParams::__set_prompt_type));
    _param_2_set_func.insert(std::make_pair("prompt_wav", &OutParams::__set_prompt_wav));
    _param_2_set_func.insert(std::make_pair("prompt_text", &OutParams::__set_prompt_text));
    _param_2_set_func.insert(std::make_pair("timeout", &OutParams::__set_timeout));
    _param_2_set_func.insert(std::make_pair("grammar", &OutParams::__set_grammar));
    _param_2_set_func.insert(std::make_pair("org", &OutParams::__set_org));
    _param_2_set_func.insert(std::make_pair("newsess", &OutParams::__set_newsess));
    _param_2_set_func.insert(std::make_pair("delay", &OutParams::__set_delay));
    _param_2_set_func.insert(std::make_pair("act_params", &OutParams::__set_act_params));
    _param_2_set_func.insert(std::make_pair("trans_idx", &OutParams::__set_trans_idx));
    _param_2_set_func.insert(std::make_pair("call_sor_id", &OutParams::__set_call_sor_id));
    _param_2_set_func.insert(std::make_pair("trans_type", &OutParams::__set_trans_type));
    _param_2_set_func.insert(std::make_pair("route_value", &OutParams::__set_route_value));
    return true;
}

bool CChinaMobileCRSServingHandler::InitParamFromConfig() {
    if ( TrioTDM::Config::Instance() == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", Config::Instance() is NULL.";
        return false;
    }
    _outaction_interactive = TrioTDM::Config::Instance()->GetIntValue("session/field_definition/outaction_interactive");
    _outaction_end = TrioTDM::Config::Instance()->GetIntValue("session/field_definition/outaction_end");
    _outaction_skip = TrioTDM::Config::Instance()->GetIntValue("session/field_definition/outaction_skip");

    _inaction_begin = TrioTDM::Config::Instance()->GetIntValue("session/field_definition/inaction_begin");
    _inaction_call_ret = TrioTDM::Config::Instance()->GetIntValue("session/field_definition/inaction_call_ret");
    _inaction_transfer_ret = TrioTDM::Config::Instance()->GetIntValue("session/field_definition/inaction_transfer_ret");

    _input_hangup = TrioTDM::Config::Instance()->get_value("session/field_definition/input_hangup");
    _input_sys_err = TrioTDM::Config::Instance()->get_value("session/field_definition/input_sys_err");
    _input_playover = TrioTDM::Config::Instance()->get_value("session/field_definition/input_playover");

   if ( !TrioTDM::Config::Instance()->get_key_value("session/trans_type2route", _trans_type2route) ) {
       VLOG(WARNING) << __FUNCTION__ << ", Can not Get _trans_type2route.";
       return false;
   }

   _session_num_limit = TrioTDM::Config::Instance()->GetIntValue("cache/session_num_limit", 1000);
   _can_del_time_delta = TrioTDM::Config::Instance()->GetIntValue("cache/can_del_time_delta", 86400);
   _must_del_time_delta = TrioTDM::Config::Instance()->GetIntValue("cache/must_del_time_delta", 604800);

   return true;
}

void CChinaMobileCRSServingHandler::ChinaMobileCRSProcess(ChinaMobileBundle& response, const ChinaMobileBundle& request) {

ST_TIME
    if ( request.__isset.inparams && request.inparams.__isset.call_id ) {
        VLOG(1) << __FUNCTION__ << ", start, call_id: " << request.inparams.call_id << " trace_id: " << request.trace_id;
    }
    CheckReleaseCoreAgent();
    bool ret = Process(request, response);
    if ( ret ) {
        response.__set_ret( 0 );
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", ret Fail,";
        response.__set_ret( -1 );
    }
    int idx = 0;
    if ( request.__isset.inparams && request.inparams.__isset.inter_idx ) {
        idx = StringToInt( request.inparams.inter_idx ) + 1;
    }
    if ( !response.__isset.outparams ) {
        OutParams out_params;
        response.__set_outparams(out_params); 
    }
    response.outparams.__set_inter_idx( IntToString(idx) );
    if ( request.__isset.inparams && request.inparams.__isset.call_id ) {
        response.outparams.__set_call_id(request.inparams.call_id);
        VLOG(1) << __FUNCTION__ << ", over, call_id: " << request.inparams.call_id << " trace_id: " << request.trace_id << " call time : " << CAL_TIME_IN_CRS;
    }
CAL_TIME 
    return ;
}

bool CChinaMobileCRSServingHandler::Process(const ChinaMobileBundle& request, ChinaMobileBundle& response) {
    response = request;
    
    if ( !request.__isset.inaction || !request.__isset.inparams || !request.inparams.__isset.call_id ) {
        VLOG(WARNING) << __FUNCTION__ << ", Not Must Field, Fail.";
        return false;
    }
    std::string session = request.inparams.call_id;
    VLOG(DEBUG) << __FUNCTION__ << ", CallID: " << session << ", Inaction: " << request.inaction;

    bool is_insert_session = true;
    TCoreAgentInfo core_agent_info = GetCoreAgent( session, is_insert_session );
    if ( core_agent_info.pCoreAgent == NULL ) {
        if ( is_insert_session ) {
            ReleaseSession( session );
        }
        VLOG(WARNING) << __FUNCTION__ << ", Can not Get CoreAgent, Fail, call_id: " << session;
        return false;
    }

    bool ret = DialogTaskRun(session, request, core_agent_info, response);

    if ( request.inparams.__isset.input && request.inparams.input == _input_hangup ) {
        if ( !ret ) {
            VLOG(WARNING) << __FUNCTION__ << ", DialogTaskRun fail, session: " << session;
        }
        VLOG(DATA) << __FUNCTION__ << ", Ending Dialogue, Session: " << session;
        if ( core_agent_info.pCoreAgent != NULL ) {
            ChinaMobileProvince::ChinaMobileSession *mobile_session = core_agent_info.pCoreAgent->GetMobileSession();
            if ( mobile_session != NULL ) {
                if (  !response.__isset.outparams ) {
                    OutParams out_params;
                    response.__set_outparams(out_params);   
                }
                std::string telephone;
                if ( mobile_session->GetTelephoneNumber(telephone) ) {
                    response.outparams.__set_call_sor_id( telephone );
                }
                std::string call_dst_id;
                if ( mobile_session->GetCallDstId(call_dst_id) ) {
                    response.outparams.__set_call_dst_id( call_dst_id );
                }
                Time c_end_time = Time::NowFromSystemTime();
                response.outparams.__set_end_time( ChinaMobileProvince::ChinaMobileCSFInterfaceLogger::Time_tToFormatString(c_end_time.ToTimeT()) );
                std::string entrance_id;
                if ( mobile_session->GetEntranceId(entrance_id) ) {
                    response.outparams.__set_entrance_id( entrance_id );
                }
                std::string start_time;
                if ( mobile_session->GetStartTime(start_time) ) {
                    response.outparams.__set_start_time( start_time );
                }
                std::string suilu_region_id;
                if ( mobile_session->GetSuiluRegionId(suilu_region_id) ) {
                    response.outparams.__set_suilu_region_id( suilu_region_id );
                }
                // 挂机满意度调查
                bool isNeedSendSatisfaction = false;
                mobile_session->IsNeedSendSatisfactionSMS(isNeedSendSatisfaction);
                if ( isNeedSendSatisfaction ) {
                    // 不包括从语音导航转入人工服务或IVR自助服务的客户
                    string closeReason = "";
                    mobile_session->GetCloseReason( closeReason );
                    if ( "1" != closeReason && "4001" != closeReason && "4002" != closeReason ) {
                        mobile_session->SendSatisfactionMessage("", "");
                    }
                }
                if ( FLAGS_china_mobile_incalllog ) {
                    string exit_id   = "";      //get from session
                    string panoramic_wav = "";
                    int64 st_time_incall = common::GetTimeInMs();
                    VLOG(1) << __FUNCTION__ << ", Incall start, call_id: " << request.inparams.call_id << " trace_id: " << request.trace_id;
                    bool logret = ChinaMobileProvince::ChinaMobileCSFInterfaceLogger::IncallLogRecorder(
                                  response, mobile_session, c_end_time, exit_id, panoramic_wav);
                    VLOG(1) << __FUNCTION__ << ", Incall over, call_id: " << request.inparams.call_id << " trace_id: " << request.trace_id << " call time : " << common::GetTimeInMs() - st_time_incall;
                    if (!logret) {
                        VLOG(FATAL) << __FUNCTION__ << " IncallLogRecorder Fail!";
                    }
                }
            }
        }
        DeleteCoreAgent( core_agent_info );
#ifdef PERSISTENCE_REDIS
        DeleCRSFromRedis( session );
#endif
    } else if ( ret ) {
#ifdef PERSISTENCE_REDIS
        SaveCRSToRedis( core_agent_info, session);
#endif 
        // TODO 保存 session
        if ( core_agent_info.pCoreAgent == NULL || !core_agent_info.pCoreAgent->SaveSession() ) {
            VLOG(WARNING) << __FUNCTION__ << ", SaveSession fail.";
        }
    }
    
    if ( is_insert_session ) {
        ReleaseSession( session );
    }

    return ret;
}

bool CChinaMobileCRSServingHandler::DialogTaskRun(const std::string& session, const ChinaMobileBundle& request, TCoreAgentInfo& core_agent_info, ChinaMobileBundle& response) {
    if ( core_agent_info.pCoreAgent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", Fail, pCoreAgent is NULL.";
        return false;
    }
    
    if ( request.inaction != _inaction_begin ) {
        // TODO 加载 Session
        if ( !core_agent_info.pCoreAgent->LoadSession(request.inparams.inter_idx) ) {
            VLOG(WARNING) << __FUNCTION__ << ", LoadSession Fail.";
            return false;
        }
    }

    // 初始化 Begin
    if ( request.inaction == _inaction_begin ) {
        VLOG(DATA) << __FUNCTION__ << ", Begin Session, CallID:" << session;
        if ( request.inparams.__isset.call_sor_id ) {
#ifdef PERSISTENCE_REDIS
            DeleCRSFromRedis( session );
#endif
            core_agent_info.pCoreAgent->InitDialogState(session, request.inparams.call_sor_id);
            ChinaMobileProvince::ChinaMobileSession *mobile_session = core_agent_info.pCoreAgent->GetMobileSession();
            if ( mobile_session != NULL ) {
                mobile_session->Init( request );
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", mobile_session Init Fail, session: " << session;
                return false;
            }
        } else {
            VLOG(FATAL) << __FUNCTION__ << ", Fail, Not call_sor_id.";
            return false;
        }
    } else if ( request.inaction == _inaction_call_ret ) { // 交互
        if ( !request.inparams.__isset.input ) {
            VLOG(FATAL) << __FUNCTION__ << ", Fail, input is not set.";
            return false;
        }
        
        VLOG(DATA) << __FUNCTION__ << ", IVR Interactive, CallID:" << session << ", Input: " << request.inparams.input;
        if ( request.inparams.input == _input_hangup || request.inparams.input == _input_sys_err ) {
            VLOG(DATA) << __FUNCTION__ << ", Hangup or SysErr, End, CallID: " << session;
            response.__set_outaction(_outaction_end);
            ChinaMobileProvince::ChinaMobileSession *mobile_session = core_agent_info.pCoreAgent->GetMobileSession();
            if ( mobile_session != NULL ) {
                if (request.inparams.input == _input_hangup) { // 用户挂机
                    mobile_session->SetCloseReason( "6" );
                } else if (request.inparams.input == _input_sys_err) { // 系统异常挂断
                    mobile_session->SetCloseReason( "-2" );
                } else {
                    mobile_session->SetCloseReason( "-2" );
                }
            } else {
                VLOG(FATAL) << __FUNCTION__ << ", mobile_session is NULL, Session: " << session;
                return false;
            }
            return true;
        }

        if ( request.inparams.input != _input_playover && !request.__isset.NLUResult ) {
            VLOG(WARNING) << __FUNCTION__ << ", Fail, Not NLUResult.";
            return false;
        }

        VLOG(DATA) << __FUNCTION__ << ", NLUResult: " << request.NLUResult;

#ifdef PERSISTENCE_REDIS
        if ( !LoadCRSFromRedis(core_agent_info, session) ) {
            VLOG(WARNING) << __FUNCTION__ << ", LoadCRSFromRedis Fail, Session: " << session;
            return false;
        }
#endif
        if ( request.inparams.input != _input_playover ) {
            TrioTDM::CInteractionEvent event(IET_USER_UTT_END);
            if ( Parser(request.NLUResult, event) ) {
                core_agent_info.pCoreAgent->AcquireNextEvent(event);
            } else {
                VLOG(FATAL) << __FUNCTION__ << ", parser query Fail, " << request.NLUResult;
                return false;
            }
        }
    } else if ( request.inaction == _inaction_transfer_ret ) {
        // 呼叫转移后结束
        VLOG(DATA) << __FUNCTION__ << ", Transfer, CallID: " << session;
        response.__set_outaction(_outaction_end);
        ChinaMobileProvince::ChinaMobileSession *mobile_session = core_agent_info.pCoreAgent->GetMobileSession();
        if ( mobile_session != NULL ) {
            // mobile_session->SetCloseReason( "CallTransfer" );

        } else {
            VLOG(FATAL) << __FUNCTION__ << ", mobile_session is NULL, Session: " << session;
            return false;
        }
        return true;
    } else {
        VLOG(FATAL) << __FUNCTION__ << ", Fail, Can not deal inaction, " << request.inaction;
        return false;
    }
    
    std::vector<TrioTDM::COutput> outputs;
    TrioTDM::TDialogExecuteReturnCode return_code = core_agent_info.pCoreAgent->Execute(outputs);

    if ( return_code == TrioTDM::dercError ) {
        VLOG(FATAL) << __FUNCTION__ << ", execute Fail.";
        return false;
    }

    if ( outputs.empty() ) {
        VLOG(WARNING) << __FUNCTION__ << ", Fail, outputs.empty().";
        return false;
    }
   
    // TODO 处理错
    if ( !FillResponse(response, outputs[0], request) ) {
        VLOG(FATAL) << __FUNCTION__ << ", FillResponse Fail.";
        return false;
    }
    if ( response.__isset.outaction && response.outaction == _outaction_end ) {
        ChinaMobileProvince::ChinaMobileSession *mobile_session = core_agent_info.pCoreAgent->GetMobileSession();
        if ( mobile_session != NULL ) {
            mobile_session->SetCloseReason( "6" ); // CRS发起挂机
        } else {
            VLOG(FATAL) << __FUNCTION__ << ", mobile_session is NULL, Session: " << session;
            return false;
        }
    } else if ( response.__isset.outaction && response.outaction == _outaction_skip ) {
        ChinaMobileProvince::ChinaMobileSession *mobile_session = core_agent_info.pCoreAgent->GetMobileSession();
        if ( mobile_session != NULL ) {
            if ( response.outparams.route_value == "1008600") {
                mobile_session->SetCloseReason( "1" );
            } else if ( response.outparams.route_value == "1008601") {
                mobile_session->SetCloseReason( "4001" );
            } else {
                mobile_session->SetCloseReason( "4002" );
            }
        } else {
            VLOG(FATAL) << __FUNCTION__ << ", mobile_session is NULL, Session: " << session;
            return false;
        }
    } else {
        // No Need
    }
    if ( FLAGS_china_mobile_ivrcrslog ) {
        ChinaMobileProvince::ChinaMobileSession *mobile_session = core_agent_info.pCoreAgent->GetMobileSession();
        std::string start_time;
        if ( mobile_session != NULL ) {
            mobile_session->GetStartTime(start_time);
        }
        int64 st_time_incommu = common::GetTimeInMs();
        VLOG(1) << __FUNCTION__ << ", Incommu start, call_id: " << request.inparams.call_id << " trace_id: " << request.trace_id;
        bool logret = ChinaMobileProvince::ChinaMobileCSFInterfaceLogger::IncommuLogRecorder(response, outputs[0], start_time);
        VLOG(1) << __FUNCTION__ << ", Incommu over, call_id: " << request.inparams.call_id << " trace_id: " << request.trace_id << " call time : " << common::GetTimeInMs() - st_time_incommu;
        if (!logret) {
            VLOG(0) << __FUNCTION__ << " IncommuLogRecorder Fail!";
        }
    }
    return true;
}

bool CChinaMobileCRSServingHandler::Parser(const std::string& query, TrioTDM::CInteractionEvent& event) {
    VLOG(DEBUG) << __FUNCTION__ << ", Query: " << query;
    std::vector<std::string> dmans;
    SplitString(query, PARSER_DMAN_SPLIT, &dmans);
    
    for (size_t i = 0;i < dmans.size(); ++i ) {
        if ( dmans[i].size() < 2 ) {
            VLOG(WARNING) << __FUNCTION__ << ", Doman error, " << dmans[i];
            return false;
        }
        std::string doman_name;
        std::vector<std::string> items;
        if ( dmans[i][0] == '{' ) {
            size_t pos = dmans[i].find('}');
            if ( pos != std::string::npos ) {
                doman_name = dmans[i].substr(1, pos-1);
                SplitString(dmans[i].substr(pos+1), PARSER_ITEM_SPLIT, &items);
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", DomanName error, " << dmans[i];
                return false;
            }
        } else {
            SplitString(dmans[i], PARSER_ITEM_SPLIT, &items);
        }
        std::string key;
        std::string value;

        for ( size_t j = 0;j < items.size(); ++j ) {
            if ( !TrioTDM::SplitOnFirst(items[j], "=", key, value) ) {
                VLOG(WARNING) << "items formt Fail, " << items[j];
                return false;
            }
            event.SetProperty(doman_name, key, value);
        }
    }

    
    event.SetComplete(true);
    return true;
}

TCoreAgentInfo CChinaMobileCRSServingHandler::GetCoreAgent(const std::string& session, bool& is_insert_session) {

ST_TIME
    VLOG(DEBUG) << __FUNCTION__ << ", session: " << session;
    TCoreAgentInfo core_agent_info;
    do {
        boost::recursive_mutex::scoped_lock lock(_mutex);
        is_insert_session = false;
        // 不能同时处理两个相同的 session 
        if ( _sessions.find(session) != _sessions.end() ) {
            VLOG(FATAL) << __FUNCTION__ << ", Session is same, SameSession: " << session;
CAL_TIME
            return core_agent_info;
        }
        is_insert_session = true;
        _sessions.insert(session);

        TSession2CoreAgentInfo::iterator f_iter = _session_2_core.find(session);
        if ( f_iter != _session_2_core.end() ) {
            VLOG(DEBUG) << __FUNCTION__ << ", exist. session: " << session;
CAL_TIME
            return f_iter->second;
        } else {
            VLOG(DEBUG) << __FUNCTION__ << ", not exist. session: " << session;
            core_agent_info.pCoreAgent = (TrioTDM::CDMCoreAgent*)TrioTDM::CRegistryAgentType::Instance()->CreateAgent("CDMCoreAgent", "DMCoreAgent:" + session);
            core_agent_info.sSession = session;
            core_agent_info.nCreateTime = common::GetTimeInSecond();
            _session_2_core.insert(std::make_pair(session, core_agent_info));
        }
    } while (0);

    if ( core_agent_info.pCoreAgent != NULL ) {
        core_agent_info.pCoreAgent->SetSession(session);
        core_agent_info.pCoreAgent->Initialize();
        core_agent_info.pCoreAgent->Register();
    }
CAL_TIME
    return core_agent_info;
}

bool CChinaMobileCRSServingHandler::LoadCRSFromRedis(TCoreAgentInfo& core_agent_info, const std::string& session) {
ST_TIME
    if ( TrioDM::RedisWrapper::Instance() == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", RedisWrapper::Instance is NULL.";
        return false;
    }
    if ( core_agent_info.pCoreAgent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", core_agent_info.pCoreAgent is NULL.";
        return false;
    }
    std::string execute_count_key = GetRedisExecuteCountKey( session );
    std::string execute_count;
    if ( !TrioDM::RedisWrapper::Instance()->get(execute_count_key, execute_count) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Redis get Fail, Key: " << execute_count_key;
        return false;
    }
    int n_execut_count = 0;
    if ( !StringToInt(execute_count, &n_execut_count) ) {
        VLOG(WARNING) << __FUNCTION__ << ", ToInt Fail, execute_count: " << execute_count;
        return false;
    }

    if ( n_execut_count == core_agent_info.nExecuteCount ) {
        VLOG(DATA) << __FUNCTION__ << ", Not Need Load Agent From Redis, ExecutCount: " << n_execut_count;
        return true;
    }
    if ( n_execut_count < core_agent_info.nExecuteCount ) {
        VLOG(DATA) << __FUNCTION__ << ", Fail, n_execut_count: " << n_execut_count 
            << ", ExecuteCount: " << core_agent_info.nExecuteCount;
        return false;
    }
    VLOG(DATA) << __FUNCTION__ << ", ExecuteLoad, n_execut_count: " << n_execut_count << ", core_agent_info.nExecuteCount: " << core_agent_info.nExecuteCount;
    core_agent_info.nExecuteCount = n_execut_count;
    
    std::string agent_key = GetRedisAgentKey( session );
    std::map<std::string, std::string> agents;
    if ( !TrioDM::RedisWrapper::Instance()->hGetAll(agent_key, agents) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Redis HGetAll Fail, Key: " << agent_key;
        return false;
    }

    std::string concept_key = GetRedisConceptKey( session );
    std::map<std::string, std::string> concepts;
    if ( !TrioDM::RedisWrapper::Instance()->hGetAll(concept_key, concepts) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Redis HGetAll Fail, Key: " << concept_key;
        return false;
    }
    
    VLOG(DATA) << __FUNCTION__ << ", Agetns.size: " << agents.size() << ", Concepts.size: " << concepts.size();
    /*
    for ( std::map<std::string, std::string>::iterator iter = agents.begin();iter != agents.end(); ++iter ) {
        VLOG(DATA) << "Agent: " << iter->first << ", Size: " << iter->second.size();
    }
    VLOG(DATA) << __FUNCTION__ << "----------------------------------------------------------------------------";
    */

    core_agent_info.pCoreAgent->Reset();
    TrioTDM::CDialogAgent* root = core_agent_info.pCoreAgent->GetTaskRoot();
    if ( root != NULL ) {
        if ( !root->Load(agents, concepts) ) {
            VLOG(WARNING) << __FUNCTION__ << ", Root Load Fail, Agents.size(): " << 
                agents.size() << ", concepts.size(): " << concepts.size();
CAL_TIME
            return false;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", root is NULL.";
CAL_TIME
        return false;
    }

    if ( !core_agent_info.pCoreAgent->Load( agents ) ) {
        VLOG(WARNING) << __FUNCTION__ << ", CoreAgent Load Fail, Agents.size(): " << agents.size();
CAL_TIME
        return false;
    }

CAL_TIME
    return true;
}

bool CChinaMobileCRSServingHandler::SaveCRSToRedis(TCoreAgentInfo core_agent_info, const std::string& session) {
ST_TIME
    VLOG(DEBUG) << __FUNCTION__ << ", Session: " << session;
    if ( core_agent_info.pCoreAgent == NULL ) {
        VLOG(WARNING) << __FUNCTION__ << ", CoreAgent is NULL.";
        return false;
    }
    std::map<std::string, std::string> agents;
    std::map<std::string, std::string> concepts;

    if ( !core_agent_info.pCoreAgent->DeclareSaveData( agents ) ) {
        VLOG(WARNING) << __FUNCTION__ << ", CoreAgent DeclareSaveData Fail.";
CAL_TIME
        return false;
    }
CAL_TIME

    TrioTDM::CDialogAgent* root = core_agent_info.pCoreAgent->GetTaskRoot();
    if ( root != NULL ) {
        if ( !root->DeclareSaveData(agents, concepts) ) {
            VLOG(WARNING) << __FUNCTION__ << ", Root DeclareSaveData Fail.";
CAL_TIME
            return false;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", CoreAgent GetTaskRoot is NULL.";
CAL_TIME
        return false;
    }
    
CAL_TIME
    if ( !DeleCRSFromRedis( session ) ) {
        VLOG(WARNING) << __FUNCTION__ << ", DeleCRSFromRedis Error, Session: " << session;
        return false;
    }

    VLOG(DATA) << __FUNCTION__ << ", Agent.size: " << agents.size();
   
CAL_TIME
    std::string agent_key = GetRedisAgentKey( session );

    if ( !agents.empty() && !TrioDM::RedisWrapper::Instance()->hMSet(agent_key, agents) ) {
        VLOG(WARNING) << __FUNCTION__ << ", AgentKey: " << agent_key << ", agents.size: " << agents.size();
CAL_TIME
        return false;
    }

    /*
    for (std::map<std::string, std::string>::iterator iter = agents.begin();iter != agents.end(); ++iter ) {
        if ( !TrioDM::RedisWrapper::Instance()->hSet(agent_key, iter->first, iter->second) ) {
            VLOG(WARNING) << __FUNCTION__ << ", AgentKey: " << agent_key << 
                ", Key: " << iter->first << ", Value: " << iter->second;
CAL_TIME
            return false;
        }
    }
    */
    
    std::string concept_key = GetRedisConceptKey( session );
    if ( !concepts.empty() && !TrioDM::RedisWrapper::Instance()->hMSet(concept_key, concepts) ) {
        VLOG(WARNING) << __FUNCTION__ << ", ConceptKey: " << concept_key << ", concepts.size: " << concepts.size();
CAL_TIME
        return false;
    }
    /*
    for (std::map<std::string, std::string>::iterator iter = concepts.begin();iter != concepts.end(); ++iter ) {
        if ( !TrioDM::RedisWrapper::Instance()->hSet(concept_key, iter->first, iter->second) ) {
            VLOG(WARNING) << __FUNCTION__ << ", ConceptKey: " << concept_key << 
                ", Key: " << iter->first << ", Value: " << iter->second;
CAL_TIME
            return false;
        }
    }
    */

    core_agent_info.nExecuteCount += 1;
    std::string execute_count_key = GetRedisExecuteCountKey( session );
    VLOG(DATA) << __FUNCTION__ << ", Set nExecuteCount, Key: " << execute_count_key << ", Value: " << core_agent_info.nExecuteCount;
    if ( !TrioDM::RedisWrapper::Instance()->set(execute_count_key, IntToString(core_agent_info.nExecuteCount) ) ) {
        VLOG(WARNING) << __FUNCTION__ << "set Fail, Key: " << execute_count_key 
            << ", Value: " << core_agent_info.nExecuteCount;
        return false;
    }
    boost::recursive_mutex::scoped_lock lock(_mutex);
    _session_2_core[session] = core_agent_info;

CAL_TIME
    return true;
}

bool CChinaMobileCRSServingHandler::DeleCRSFromRedis(const std::string& session) {
    std::string agent_key = GetRedisAgentKey( session );
    if ( !TrioDM::RedisWrapper::Instance()->del(agent_key) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Redis Del Fail, Key: " << agent_key;
        return false;
    }
    
    std::string concept_key = GetRedisConceptKey( session );
    if ( !TrioDM::RedisWrapper::Instance()->del(concept_key) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Redis Del Fail, Key: " << concept_key;
        return false;
    }

    std::string execute_count_key = GetRedisExecuteCountKey( session );
    if ( !TrioDM::RedisWrapper::Instance()->del(execute_count_key) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Redis Del Fail, Key: " << execute_count_key;
        return false;
    }
    return true;
}

void CChinaMobileCRSServingHandler::DeleteCoreAgent(TCoreAgentInfo& core_agent_info) {
    VLOG(DATA) << __FUNCTION__ << ", Delete, Session: " << core_agent_info.sSession;
    ReleaseCoreAgent(core_agent_info);
    boost::recursive_mutex::scoped_lock lock(_mutex);
    const std::string& session = core_agent_info.sSession;
    TSession2CoreAgentInfo::iterator f_iter = _session_2_core.find(session);
    if ( f_iter != _session_2_core.end() ) {
        _session_2_core.erase(f_iter);
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", Can not find core_agent_info, session: " << session;
    }
}

void CChinaMobileCRSServingHandler::ReleaseCoreAgent(TCoreAgentInfo& core_agent_info) {
    if ( core_agent_info.pCoreAgent != NULL ) {
        core_agent_info.pCoreAgent->OnDestruction();
        delete core_agent_info.pCoreAgent;
        core_agent_info.pCoreAgent = NULL;
    }
}

void CChinaMobileCRSServingHandler::ReleaseSession(const std::string& session) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    VLOG(DEBUG) << __FUNCTION__ << ", Session: " << session;
    _sessions.erase(session);
}

void CChinaMobileCRSServingHandler::CheckReleaseCoreAgent() {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    VLOG(DEBUG) << __FUNCTION__;
    if ( _session_2_core.size() < _session_num_limit ) {
        VLOG(DEBUG) << __FUNCTION__ << ", Not Need Do, _session_2_core.size: " << _session_2_core.size();
        return ;
    }
    int del_count = 0;
    int64 current_time = common::GetTimeInSecond();
    for( TSession2CoreAgentInfo::iterator iter = _session_2_core.begin(); iter != _session_2_core.end();) {
        if ( _sessions.find(iter->first)  != _sessions.end() ) {
            VLOG(DATA) << __FUNCTION__ << ", OnUse, Session: " << iter->first;
            ++iter;
            continue;
        }
        int64 delta = current_time - iter->second.nCreateTime;
        if ( delta > _must_del_time_delta ) {
            VLOG(DATA) << __FUNCTION__ << ", Must Del, Session: " << iter->first;
            ReleaseCoreAgent(iter->second);
#ifdef PERSISTENCE_REDIS
            DeleCRSFromRedis( iter->first );
#endif
             _session_2_core.erase(iter++);
            del_count += 1;
        }
#ifdef PERSISTENCE_REDIS
        else if ( delta > _can_del_time_delta && !IsSessionExistInRedis(iter->first) ) {
            VLOG(DATA) << __FUNCTION__ << ", Can Del, Del, Session: " << iter->first;
            ReleaseCoreAgent(iter->second);
            _session_2_core.erase(iter++);
            del_count += 1;
        }
#endif
        else {
            ++iter;
        }
    }
    VLOG(DATA) << __FUNCTION__ << ", Delete Session Num: " << del_count;
}

bool CChinaMobileCRSServingHandler::IsSessionExistInRedis(const std::string& session) {
    std::string execute_count_key = GetRedisExecuteCountKey(session);
    if ( TrioDM::RedisWrapper::Instance() != NULL ) {
        if ( TrioDM::RedisWrapper::Instance()->exists(execute_count_key) ) {
            return true;
        }
    }
    return false;
}

bool CChinaMobileCRSServingHandler::FillResponse(ChinaMobileBundle& response, const TrioTDM::COutput& output, const ChinaMobileBundle& request) {
 
    OutParams out_params;

    std::string msg = output.Msg();

    std::vector<std::string> tokens;
    SplitString(msg, '&', &tokens);

    VLOG(DEBUG) << __FUNCTION__ << ", MSG: " << msg;

    for ( size_t i = 0;i < tokens.size(); ++i ) {
        std::string param;
        std::string value;

        if ( TrioTDM::SplitOnFirst(tokens[i], "=", param, value) ) {
            std::map<std::string, SetOutParamsValue>::const_iterator c_iter = _param_2_set_func.find(param);
            if ( c_iter != _param_2_set_func.end() ) {
                (out_params.*(c_iter->second))(value);
            } else {
                if ( param == "outaction" ) {
                    response.__set_outaction(atoi(value.c_str()));
                } else if ( param != "is_helpful" ) {
                    VLOG(FATAL) << __FUNCTION__ << ", Not Param: " << param;
                    return false;
                }
            }
        } else {
            VLOG(FATAL) << __FUNCTION__ << ", Format Error, MSG: " << msg;
            return false;
        }
    }
    
    if ( !response.__isset.outaction ) {
        response.__set_outaction( output.GetOutaction() );
    }
   
    // 填写默认值
    if ( response.outaction == _outaction_interactive ) {
        if ( !out_params.__isset.flow_node_name ) {
            out_params.__set_flow_node_name( output.GetGenAgentName() );
        }

        if ( !out_params.__isset.model_type ) {
            out_params.__set_model_type( output.ModelType() );
        }

        if ( !out_params.__isset.timeout ) {
            out_params.__set_timeout( FLAGS_timeout );
        }

        if ( !out_params.__isset.grammar ) {
            out_params.__set_grammar( request.NLUResult );
        }

        if ( !out_params.__isset.org ) {
            out_params.__set_org( request.NLUResult );
        }

        if ( !out_params.__isset.newsess ) {
            out_params.__set_newsess("");
        }

        if ( !out_params.__isset.delay ) {
            out_params.__set_delay("");
        }

        if ( !out_params.__isset.act_params ) {
            out_params.__set_act_params("");
        }
    } else if ( response.outaction == _outaction_skip ) {
        if ( !out_params.__isset.trans_type ) {
            VLOG(WARNING) << __FUNCTION__ << ", trans_type Can not NULL.";
            return false;
        }
        if ( !out_params.__isset.trans_idx ) {
            out_params.__set_trans_idx("");
        }
        if ( !out_params.__isset.call_sor_id ) {
            out_params.__set_call_sor_id( output.GetTelephone() );
        }
        if ( !out_params.__isset.route_value ) {
            std::map<std::string, std::string>::const_iterator c_iter = _trans_type2route.find( out_params.trans_type );
            if ( c_iter != _trans_type2route.end() ) {
                out_params.__set_route_value( c_iter->second );
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", Not Route, Type: " << out_params.trans_type;
            }
        }
    }
    response.__set_outparams(out_params);   

    return true;
}

std::string CChinaMobileCRSServingHandler::GetRedisExecuteCountKey(const std::string& session) {
    return "EXECUTE.COUNT." + session;
}

std::string CChinaMobileCRSServingHandler::GetRedisAgentKey(const std::string& session) {
    return "CRS.AGENT." + session;
}

std::string CChinaMobileCRSServingHandler::GetRedisConceptKey(const std::string& session) {
    return "CRS.CONCEPT." + session;
}

}
