#include "CSFInterfaceLogger.h"
#include "Projects/ChinaMobile/CRS/Src/Interface/OperateLogInterface.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"
//#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Projects/ChinaMobile/CRS/Src/Session/ChinaMobileSession.h"

DEFINE_string(province_sign, "sd", "省份地域标签");

namespace ChinaMobileProvince {

bool ChinaMobileCSFInterfaceLogger::IninterLogRecorder(IninterLogInfo ininter_log_info) {
    string result;
    string anyLog = IninterLogInfoToAnyLog(ininter_log_info);
    VLOG(1) << "IninterLogRecorder anyLog :" << anyLog;
    if (!BusinessLogWrite(anyLog, result)) {
        VLOG(0) << __FUNCTION__ << " BusinessLogWrite error!";
        return false;
    }
    VLOG(DEBUG) << "IninterLogRecorder BusinessLogWrite success!";
    return true;
}

bool ChinaMobileCSFInterfaceLogger::IncommuLogRecorder(IncommuLogInfo incommu_log_info) {
    string result;
    string anyLog = IncommuLogInfoToAnyLog(incommu_log_info);
    VLOG(DEBUG) << "IncommuLogRecorder anyLog :" << anyLog;
    if (!BusinessLogWrite(anyLog, result)) {
        VLOG(0) << __FUNCTION__ << " BusinessLogWrite error!";
        return false;
    }
    VLOG(DEBUG) << "IncommuLogRecorder BusinessLogWrite success!";
    return true;
}

bool ChinaMobileCSFInterfaceLogger::IncallLogRecorder(IncallLogInfo incall_log_info) {
    string result;
    string anyLog = IncallLogInfoToAnyLog(incall_log_info);
    VLOG(1) << "IncallLogRecorder anyLog :" << anyLog;
    if (!BusinessLogWrite(anyLog, result)) {
        VLOG(0) << __FUNCTION__ << " BusinessLogWrite error!";
        return false;
    }
    VLOG(DEBUG) << "IncallLogRecorder BusinessLogWrite success!";
    return true;
}

bool ChinaMobileCSFInterfaceLogger::IninterLogRecorder(const string& csfRequest, const string& interface_ecode, const string& business_name,
                                                       const tm& tm_interface_stime, const tm& tm_interface_etime,
                                                       const string& sessionId, const string& telephoneNumber, const string& res_code, const string& smsContent,
                                                       const string& inter_idx, const string& interface_idx,
                                                       const string& accept_id, const string& templateCode, const string& interface_sendMsg, const string& call_day_yd) {
    IninterLogInfo ininterloginfo;
    ininterloginfo.EVENT = "TB_ICA_IN_INTER_D";
    ininterloginfo.call_id = sessionId;
    ininterloginfo.inter_idx = inter_idx;
    ininterloginfo.interface_etime = TmToFormatString(tm_interface_etime);
    ininterloginfo.interface_idx = interface_idx;
    ininterloginfo.interface_result = res_code;
    ininterloginfo.interface_stime = TmToFormatString(tm_interface_stime);
    ininterloginfo.accept_id = accept_id;
    ininterloginfo.call_sor_id = telephoneNumber;
    ininterloginfo.interface_input = csfRequest;
    ininterloginfo.templateCode = templateCode;
    ininterloginfo.interface_sendMsg = interface_sendMsg;
    ininterloginfo.interface_sendCode = "";
    // ininterloginfo.smsContent = "";
    ininterloginfo.call_day_yd = call_day_yd;
    ininterloginfo.AB_test = "";
    ininterloginfo.province_sign = FLAGS_province_sign;

    ininterloginfo.interface_ecode = interface_ecode;
    ininterloginfo.business_name = business_name;
    ininterloginfo.interface_name = business_name;
    ininterloginfo.smsContent = smsContent;
    if (ininterloginfo.smsContent != "") {
        ininterloginfo.interface_sendCode = res_code;
    }

    if (!IninterLogRecorder(ininterloginfo)) {
        VLOG(0) << __FUNCTION__ << "IninterLogRecorder error! ";
        return false;
    }
    return true;
}

bool ChinaMobileCSFInterfaceLogger::IncommuLogRecorder(const chinaMobile::ChinaMobileBundle& response, const TrioTDM::COutput& output, const string& start_time) {
    IncommuLogInfo incommuloginfo;
    incommuloginfo.CHAN                   = "";
    incommuloginfo.EVENT                  = "TB_ICA_IN_COMMU_D";
    incommuloginfo.TIME                   = "";
    incommuloginfo._LOGLIB_LOG_RANDOM_ID_ = "";
    //incommuloginfo.begin_play             = UtcStringToFormatString(response.inparams.begin_play);
    incommuloginfo.begin_play             = response.inparams.begin_play;
    incommuloginfo.busi_word_result       = GetBusinessNameByGenAgentName(output.GetGenAgentName());
    incommuloginfo.business_name          = GetBusinessNameByGenAgentName(output.GetGenAgentName());
    incommuloginfo.call_id                = response.inparams.call_id;
    incommuloginfo.crs_result             = response.ASRResult;
    //incommuloginfo.end_play               = UtcStringToFormatString(response.inparams.end_play);
    incommuloginfo.end_play               = response.inparams.end_play;
    incommuloginfo.flow_node_name         = response.outparams.flow_node_name;
    incommuloginfo.flow_result            = response.inparams.input;
    //incommuloginfo.flow_result_time       = UtcStringToFormatString(response.inparams.result_time);
    incommuloginfo.flow_result_time       = response.inparams.result_time;
    incommuloginfo.flow_result_type       = response.inparams.flow_result_type;
    incommuloginfo.flow_type              = response.outparams.flow_type;
    incommuloginfo.grammar                = response.outparams.grammar;
    //incommuloginfo.inter_idx              = response.outparams.inter_idx;
    //outparams.inter_idx = inparams.inter_idx + 1
    int idx = 0;
    if ( response.__isset.inparams && response.inparams.__isset.inter_idx ) {
        idx = StringToInt( response.inparams.inter_idx ) + 1;
    }
    incommuloginfo.inter_idx              = IntToString(idx);
    incommuloginfo.inter_no               = response.inparams.inter_no;
    incommuloginfo.pid                    = "";
    incommuloginfo.prompt_text            = response.outparams.prompt_text;
    incommuloginfo.prompt_type            = response.outparams.prompt_type;
    incommuloginfo.prompt_wav             = response.outparams.prompt_wav;
    incommuloginfo.simp_word_result       = response.NLUResult;
    incommuloginfo.sort_time              = "";
    incommuloginfo.tid                    = "";
    incommuloginfo.timestamp              = "";
    incommuloginfo.call_day_yd            = start_time;
    incommuloginfo.AB_test                = "";
    incommuloginfo.province_sign          = FLAGS_province_sign;

    if (!IncommuLogRecorder(incommuloginfo)) {
        VLOG(0) << __FUNCTION__ << "IncommuLogRecorder error! ";
        return false;
    }
    return true;
}

bool ChinaMobileCSFInterfaceLogger::IncallLogRecorder(const chinaMobile::ChinaMobileBundle &response, ChinaMobileSession *session, const Time &c_end_time,
                                                      const string &exit_id, const string &panoramic_wav) {
    IncallLogInfo incallloginfo;
    incallloginfo.CHAN                   = "";
    incallloginfo.EVENT                  = "TB_ICA_IN_CALL_D";
    incallloginfo.TIME                   = "";
    incallloginfo._LOGLIB_LOG_RANDOM_ID_ = "";
    incallloginfo.brand_id               = "";    //get from session
    incallloginfo.call_dst_id            = response.outparams.call_dst_id;
    incallloginfo.call_id                = response.inparams.call_id;
    incallloginfo.call_sor_id            = response.outparams.call_sor_id;
    string end_time = Time_tToFormatString(c_end_time.ToTimeT());
    incallloginfo.end_time               = end_time;
    incallloginfo.entrance_id            = response.outparams.entrance_id;
    incallloginfo.exit_id                = exit_id;
    incallloginfo.level_id               = "";
    incallloginfo.pid                    = "";
    incallloginfo.region_id              = "";    //get from session
    incallloginfo.sort_time              = "";
    incallloginfo.start_time             = response.outparams.start_time;
    incallloginfo.suilu_region_id        = response.outparams.suilu_region_id;
    incallloginfo.tid                    = "";
    incallloginfo.timestamp              = "";
    incallloginfo.user_type_id           = "";    //get from session
    incallloginfo.call_day_yd            = response.outparams.start_time;
    incallloginfo.panoramic_wav          = "";
    incallloginfo.AB_plan                = "";
    incallloginfo.AB_version             = "";
    incallloginfo.AB_test                = "";
    incallloginfo.province_sign          = FLAGS_province_sign;

    if (session != NULL) {
        string telephoneNumber = "";
        session->GetTelephoneNumber(telephoneNumber);
        session->GetCloseReason(incallloginfo.exit_id);
        //session->GetCallDstId(incallloginfo.call_dst_id);
        //session->GetTelephoneNumber(incallloginfo.call_sor_id);
        //session->GetEntranceId(incallloginfo.entrance_id);
        //session->GetStartTime(incallloginfo.start_time);
        //session->GetSuiluRegionId(incallloginfo.suilu_region_id);
        //incallloginfo.call_day_yd = incallloginfo.start_time;
        if ("" != telephoneNumber) {
            session->GetBrandId(incallloginfo.brand_id);
            session->GetTrustLevel(incallloginfo.level_id);
            session->GetRegion(incallloginfo.region_id);
            //session->GetPayType(incallloginfo.user_type_id);
            // [用户信息查询]的PayModeId替代[余额查询]的PayType
            session->GetPayModeId(incallloginfo.user_type_id);
        } else {
            VLOG(0) << __FUNCTION__ << "IncallLogRecorder telephoneNumber NULL! ";
        }
    }

    if (!IncallLogRecorder(incallloginfo)) {
        VLOG(0) << __FUNCTION__ << "IncallLogRecorder error! ";
        return false;
    }
    return true;
}

string ChinaMobileCSFInterfaceLogger::IninterLogInfoToAnyLog(IninterLogInfo ininterloginfo) {
    Json::Value jsonAnyLog;
    jsonAnyLog["busiLog"]["CHAN"]                   = LimitVarchar(ininterloginfo.CHAN                , 50 );
    jsonAnyLog["busiLog"]["EVENT"]                  = LimitVarchar(ininterloginfo.EVENT               , 50 );
    jsonAnyLog["busiLog"]["TIME"]                   = LimitVarchar(ininterloginfo.TIME                , 50 );
    jsonAnyLog["busiLog"]["_LOGLIB_LOG_RANDOM_ID_"] = LimitVarchar(ininterloginfo.LOGLIB_LOG_RANDOM_ID, 50 );
    jsonAnyLog["busiLog"]["call_id"]                = LimitVarchar(ininterloginfo.call_id             , 40 );
    jsonAnyLog["busiLog"]["inter_idx"]              = LimitVarchar(ininterloginfo.inter_idx           , 30 );
    jsonAnyLog["busiLog"]["interface_ecode"]        = LimitVarchar(ininterloginfo.interface_ecode     , 20 );
    jsonAnyLog["busiLog"]["interface_etime"]        = LimitVarchar(ininterloginfo.interface_etime     , 20 );
    jsonAnyLog["busiLog"]["interface_idx"]          = LimitVarchar(ininterloginfo.interface_idx       , 30 );
    jsonAnyLog["busiLog"]["interface_result"]       = LimitVarchar(ininterloginfo.interface_result    , 40 );
    jsonAnyLog["busiLog"]["interface_stime"]        = LimitVarchar(ininterloginfo.interface_stime     , 20 );
    jsonAnyLog["busiLog"]["pid"]                    = LimitVarchar(ininterloginfo.pid                 , 50 );
    jsonAnyLog["busiLog"]["sort_time"]              = LimitVarchar(ininterloginfo.sort_time           , 50 );
    jsonAnyLog["busiLog"]["tid"]                    = LimitVarchar(ininterloginfo.tid                 , 50 );
    jsonAnyLog["busiLog"]["timestamp"]              = LimitVarchar(ininterloginfo.timestamp           , 50 );
    jsonAnyLog["busiLog"]["accept_id"]              = LimitVarchar(ininterloginfo.accept_id           , 19 );
    jsonAnyLog["busiLog"]["business_name"]          = LimitVarchar(ininterloginfo.business_name       , 80 );
    jsonAnyLog["busiLog"]["call_sor_id"]            = LimitVarchar(ininterloginfo.call_sor_id         , 19 );
    jsonAnyLog["busiLog"]["interface_input"]        = LimitVarchar(ininterloginfo.interface_input     , 40 );
    jsonAnyLog["busiLog"]["interface_name"]         = LimitVarchar(ininterloginfo.interface_name      , 40 );
    jsonAnyLog["busiLog"]["templateCode"]           = LimitVarchar(ininterloginfo.templateCode        , 20 );
    jsonAnyLog["busiLog"]["interface_sendMsg"]      = LimitVarchar(ininterloginfo.interface_sendMsg   , 50 );
    jsonAnyLog["busiLog"]["interface_sendCode"]     = LimitVarchar(ininterloginfo.interface_sendCode  , 20 );
    jsonAnyLog["busiLog"]["smsContent"]             = LimitVarchar(ininterloginfo.smsContent          , 200);
    jsonAnyLog["busiLog"]["call_day_yd"]            = LimitVarchar(ininterloginfo.call_day_yd         , 20 );
    jsonAnyLog["busiLog"]["AB_test"]                = LimitVarchar(ininterloginfo.AB_test             , 20 );
    jsonAnyLog["busiLog"]["province_sign"]          = LimitVarchar(ininterloginfo.province_sign       , 10 );

    Json::FastWriter writer;
    return writer.write(jsonAnyLog);
}

string ChinaMobileCSFInterfaceLogger::IncommuLogInfoToAnyLog(IncommuLogInfo incommu_log_info) {
    Json::Value jsonAnyLog;
    jsonAnyLog["busiLog"]["CHAN"]                   = LimitVarchar(incommu_log_info.CHAN                  , 50  );
    jsonAnyLog["busiLog"]["EVENT"]                  = LimitVarchar(incommu_log_info.EVENT                 , 50  );
    jsonAnyLog["busiLog"]["TIME"]                   = LimitVarchar(incommu_log_info.TIME                  , 50  );
    jsonAnyLog["busiLog"]["_LOGLIB_LOG_RANDOM_ID_"] = LimitVarchar(incommu_log_info._LOGLIB_LOG_RANDOM_ID_, 50  );
    jsonAnyLog["busiLog"]["begin_play"]             = LimitVarchar(incommu_log_info.begin_play            , 20  );
    jsonAnyLog["busiLog"]["busi_word_result"]       = LimitVarchar(incommu_log_info.busi_word_result      , 200 );
    jsonAnyLog["busiLog"]["business_name"]          = LimitVarchar(incommu_log_info.business_name         , 50  );
    jsonAnyLog["busiLog"]["call_id"]                = LimitVarchar(incommu_log_info.call_id               , 40  );
    jsonAnyLog["busiLog"]["crs_result"]             = LimitVarchar(incommu_log_info.crs_result            , 100 );
    jsonAnyLog["busiLog"]["end_play"]               = LimitVarchar(incommu_log_info.end_play              , 20  );
    jsonAnyLog["busiLog"]["flow_node_name"]         = LimitVarchar(incommu_log_info.flow_node_name        , 50  );
    jsonAnyLog["busiLog"]["flow_result"]            = LimitVarchar(incommu_log_info.flow_result           , 50  );
    jsonAnyLog["busiLog"]["flow_result_time"]       = LimitVarchar(incommu_log_info.flow_result_time      , 20  );
    jsonAnyLog["busiLog"]["flow_result_type"]       = LimitVarchar(incommu_log_info.flow_result_type      , 8   );
    jsonAnyLog["busiLog"]["flow_type"]              = LimitVarchar(incommu_log_info.flow_type             , 8   );
    jsonAnyLog["busiLog"]["grammar"]                = LimitVarchar(incommu_log_info.grammar               , 200 );
    jsonAnyLog["busiLog"]["inter_idx"]              = LimitVarchar(incommu_log_info.inter_idx             , 30  );
    jsonAnyLog["busiLog"]["inter_no"]               = LimitVarchar(incommu_log_info.inter_no              , 30  );
    jsonAnyLog["busiLog"]["pid"]                    = LimitVarchar(incommu_log_info.pid                   , 20  );
    jsonAnyLog["busiLog"]["prompt_text"]            = LimitVarchar(incommu_log_info.prompt_text           , 2000);
    jsonAnyLog["busiLog"]["prompt_type"]            = LimitVarchar(incommu_log_info.prompt_type           , 20  );
    jsonAnyLog["busiLog"]["prompt_wav"]             = LimitVarchar(incommu_log_info.prompt_wav            , 200 );
    jsonAnyLog["busiLog"]["simp_word_result"]       = LimitVarchar(incommu_log_info.simp_word_result      , 500 );
    jsonAnyLog["busiLog"]["sort_time"]              = LimitVarchar(incommu_log_info.sort_time             , 50  );
    jsonAnyLog["busiLog"]["tid"]                    = LimitVarchar(incommu_log_info.tid                   , 30  );
    jsonAnyLog["busiLog"]["timestamp"]              = LimitVarchar(incommu_log_info.timestamp             , 30  );
    jsonAnyLog["busiLog"]["call_day_yd"]            = LimitVarchar(incommu_log_info.call_day_yd           , 20  );
    jsonAnyLog["busiLog"]["AB_test"]                = LimitVarchar(incommu_log_info.AB_test               , 20  );
    jsonAnyLog["busiLog"]["province_sign"]          = LimitVarchar(incommu_log_info.province_sign         , 10  );

    Json::FastWriter writer;
    return writer.write(jsonAnyLog);
}

string ChinaMobileCSFInterfaceLogger::IncallLogInfoToAnyLog(IncallLogInfo incall_log_info) {
    Json::Value jsonAnyLog;
    jsonAnyLog["busiLog"]["CHAN"]=                   LimitVarchar(incall_log_info.CHAN                   , 50 );
    jsonAnyLog["busiLog"]["EVENT"]=                  LimitVarchar(incall_log_info.EVENT                  , 50 );
    jsonAnyLog["busiLog"]["TIME"]=                   LimitVarchar(incall_log_info.TIME                   , 50 );
    jsonAnyLog["busiLog"]["_LOGLIB_LOG_RANDOM_ID_"]= LimitVarchar(incall_log_info._LOGLIB_LOG_RANDOM_ID_ , 50 );
    jsonAnyLog["busiLog"]["brand_id"]=               LimitVarchar(incall_log_info.brand_id               , 15 );
    jsonAnyLog["busiLog"]["call_dst_id"]=            LimitVarchar(incall_log_info.call_dst_id            , 19 );
    jsonAnyLog["busiLog"]["call_id"]=                LimitVarchar(incall_log_info.call_id                , 40 );
    jsonAnyLog["busiLog"]["call_sor_id"]=            LimitVarchar(incall_log_info.call_sor_id            , 19 );
    jsonAnyLog["busiLog"]["end_time"]=               LimitVarchar(incall_log_info.end_time               , 20 );
    jsonAnyLog["busiLog"]["entrance_id"]=            LimitVarchar(incall_log_info.entrance_id            , 8  );
    jsonAnyLog["busiLog"]["exit_id"]=                LimitVarchar(incall_log_info.exit_id                , 8  );
    jsonAnyLog["busiLog"]["level_id"]=               LimitVarchar(incall_log_info.level_id               , 20 );
    jsonAnyLog["busiLog"]["pid"]=                    LimitVarchar(incall_log_info.pid                    , 50 );
    jsonAnyLog["busiLog"]["region_id"]=              LimitVarchar(incall_log_info.region_id              , 50 );
    jsonAnyLog["busiLog"]["sort_time"]=              LimitVarchar(incall_log_info.sort_time              , 50 );
    jsonAnyLog["busiLog"]["start_time"]=             LimitVarchar(incall_log_info.start_time             , 20 );
    jsonAnyLog["busiLog"]["suilu_region_id"]=        LimitVarchar(incall_log_info.suilu_region_id        , 10 );
    jsonAnyLog["busiLog"]["tid"]=                    LimitVarchar(incall_log_info.tid                    , 50 );
    jsonAnyLog["busiLog"]["timestamp"]=              LimitVarchar(incall_log_info.timestamp              , 50 );
    jsonAnyLog["busiLog"]["user_type_id"]=           LimitVarchar(incall_log_info.user_type_id           , 50 );
    jsonAnyLog["busiLog"]["call_day_yd"]=            LimitVarchar(incall_log_info.call_day_yd            , 20 );
    jsonAnyLog["busiLog"]["panoramic_wav"]=          LimitVarchar(incall_log_info.panoramic_wav          , 200);
    jsonAnyLog["busiLog"]["AB_plan"]=                LimitVarchar(incall_log_info.AB_plan                , 50 );
    jsonAnyLog["busiLog"]["AB_version"]=             LimitVarchar(incall_log_info.AB_version             , 20 );
    jsonAnyLog["busiLog"]["AB_test"]=                LimitVarchar(incall_log_info.AB_test                , 20 );
    jsonAnyLog["busiLog"]["province_sign"]=          LimitVarchar(incall_log_info.province_sign          , 10 );
    Json::FastWriter writer;
    return writer.write(jsonAnyLog);
}

string ChinaMobileCSFInterfaceLogger::LimitVarchar(string varchars, unsigned int limit_num) {
    if (varchars.length() <= limit_num) {
        return varchars;
    } else {
        if (1 > limit_num) {
            VLOG(0) << __FUNCTION__ << "limit_num error : " << limit_num;
            return "";
        }
        return varchars.substr(0, limit_num - 1) + "+";
    }
}

string ChinaMobileCSFInterfaceLogger::TmToFormatString(tm interface_time) {
    // format is yyyyMMddHH:mm:ss
    string format_time;
    SStringPrintf(&format_time, "%04d%02d%02d%02d:%02d:%02d",
                  interface_time.tm_year + 1900, interface_time.tm_mon + 1, interface_time.tm_mday,
                  interface_time.tm_hour, interface_time.tm_min, interface_time.tm_sec);
    return format_time;
}

string ChinaMobileCSFInterfaceLogger::Time_tToFormatString(time_t interface_time_t) {
    tm* interface_time_tm = localtime(&interface_time_t);
    return TmToFormatString(*interface_time_tm);
}

string ChinaMobileCSFInterfaceLogger::UtcStringToFormatString(string utc_string) {
    time_t interface_time_t = (time_t)atof(utc_string.c_str());
    return Time_tToFormatString(interface_time_t);
}

string ChinaMobileCSFInterfaceLogger::GetBusinessName(const string &agents_flow) {
    std::vector<std::string> agents;
    SplitString(agents_flow, '/', &agents);
    string business_name = "";
    if (agents.size() >= 3) {
        business_name = AgentNameToBusinessName(agents[2]);
    }
    return business_name;
}

string ChinaMobileCSFInterfaceLogger::GetBusinessNameByGenAgentName(const string &gen_agent_name) {
    string business_name = gen_agent_name;
    std::vector<std::string> genAgentSubs;
    Tokenize(gen_agent_name, " .", &genAgentSubs);
    if (genAgentSubs.size() >= 2) {
        business_name = AgentNameToBusinessName(genAgentSubs[1]);
    }
    return business_name;
}

string ChinaMobileCSFInterfaceLogger::AgentNameToBusinessName(const string &agent_name) {
    map<string, string>::const_iterator finder = m_mapBusinessName.find(agent_name);
    if (finder != m_mapBusinessName.end()) {
        return finder->second;
    } else {
        return agent_name;
    }
}

const map<string, string> ChinaMobileCSFInterfaceLogger::m_mapBusinessName = ChinaMobileCSFInterfaceLogger::CreateBusinessNameMap();

const map<string, string> ChinaMobileCSFInterfaceLogger::CreateBusinessNameMap() {
    map<string, string> mapBusinessName;
    mapBusinessName["Welcome"]                              = "导航欢迎语";
    mapBusinessName["PrimaryMenu"]                          = "主菜单";
    mapBusinessName["nt_main"]                              = "主菜单";
    mapBusinessName["OtherMean"]                            = "主菜单集外说法处理";
    mapBusinessName["FriendlyGreetings"]                    = "友好问候";
    mapBusinessName["Thanks"]                               = "感谢";
    mapBusinessName["Finish"]                               = "结束语";
    mapBusinessName["Tiresome"]                             = "批评,抱怨,脏话";
    mapBusinessName["Doubt"]                                = "机器人";
    mapBusinessName["NotActivateBusiness"]                  = "集外业务咨询";
    mapBusinessName["ShortRejection"]                       = "短拒识";
    mapBusinessName["TrasferArtificialConfirm"]             = "zrg转人工确认";
    mapBusinessName["TrasferIVRConfirm"]                    = "转IVR确认";
    mapBusinessName["OutOfServiceJudge"]                    = "停机判断";
    mapBusinessName["SystemException"]                      = "系统异常";
    mapBusinessName["BalanceInquiry"]                       = "余额查询";
    mapBusinessName["BillInquiry"]                          = "账单查询";
    mapBusinessName["BillInquiryBroadcast"]                 = "账单查询";
    mapBusinessName["BusinessInquiry"]                      = "本机业务查询";
    mapBusinessName["MonthFeeDeductionInquiry"]             = "月初扣费查询";
    mapBusinessName["MonthFeeDeductionInquiryBroadcast"]    = "月初扣费查询";
    mapBusinessName["TelephoneNumberInquiry"]               = "本机号码查询";
    mapBusinessName["PackageInfoRemainderInquiry"]          = "套餐剩余量查询";
    mapBusinessName["PackageInfoRemainderBroadcast"]        = "套餐剩余量查询";
    mapBusinessName["RemainingTrafficInquiry"]              = "流量剩余查询";
    mapBusinessName["RemainingSMSInquiry"]                  = "短信剩余查询";
    mapBusinessName["RemainingSpeechSoundInquiry"]          = "语音剩余查询";
    mapBusinessName["RechargeRecordsInquiry"]               = "充值缴费记录查询";
    mapBusinessName["ScoreBusiness"]                        = "积分业务";
    mapBusinessName["PasswordService"]                      = "密码服务";
    mapBusinessName["PasswordServiceSelect"]                = "密码服务";
    mapBusinessName["PasswordInquiry"]                      = "查询密码";
    mapBusinessName["PasswordChange"]                       = "密码修改";
    mapBusinessName["PasswordChangeSelect"]                 = "密码修改";
    mapBusinessName["PasswordReset"]                        = "密码重置";
    mapBusinessName["PasswordResetSelect"]                  = "密码重置";
    mapBusinessName["TelephonePassword"]                    = "手机密码";
    mapBusinessName["TelephonePasswordChoose"]              = "手机密码";
    mapBusinessName["TelephonePasswordReset"]               = "手机密码重置";
    mapBusinessName["CertificateVerification"]              = "证件验证";
    mapBusinessName["TelephonePasswordModify"]              = "手机密码修改";
    mapBusinessName["OriginalPasswordVerification"]         = "原密码验证";
    mapBusinessName["NewPasswordInput"]                     = "新密码输入";
    mapBusinessName["PasswordDoubleConfirm"]                = "密码二次确认";
    mapBusinessName["WideBandBusiness"]                     = "宽带业务";
    mapBusinessName["WideBandBusinessSelect"]               = "宽带业务";
    mapBusinessName["WideBandChargesIntroduce"]             = "宽带资费介绍";
    mapBusinessName["WideBandChargesSelect"]                = "宽带资费介绍";
    mapBusinessName["SingleBroadband"]                      = "单品宽带介绍";
    mapBusinessName["4GConvergedWideBandIntroduce"]         = "4G融合宽带介绍";
    mapBusinessName["GroupWideBandIntroduce"]               = "群组宽带介绍";
    mapBusinessName["WideBandOrder"]                        = "宽带预约";
    mapBusinessName["WideBandPassword"]                     = "宽带密码 ";
    mapBusinessName["WideBandPasswordSelect"]               = "宽带密码";
    mapBusinessName["WideBandPasswordReset"]                = "宽带密码重置";
    mapBusinessName["WideBandPasswordModify"]               = "宽带密码修改";
    mapBusinessName["WideBandOriginalPasswordVerification"] = "宽带原密码输入";
    mapBusinessName["WideBandNewPasswordInput"]             = "宽带新密码输入";
    mapBusinessName["WideBandNewPasswordDoubleConfirm"]     = "宽带新密码二次确认";
    mapBusinessName["TelephoneRecharge"]                    = "手机充值";
    mapBusinessName["BankCardRecharge"]                     = "银行卡充值";
    mapBusinessName["RechargeCardRecharge"]                 = "充值卡充值";
    mapBusinessName["ChargesPackageHandle"]                 = "资费套餐办理";
    mapBusinessName["PackageSelect"]                        = "套餐选择";
    mapBusinessName["FEnjoyPackageHandle"]                  = "4G飞享套餐办理";
    mapBusinessName["FEnjoyPackage18Yuan"]                  = "4G飞享套餐18元";
    mapBusinessName["FEnjoyPackageOpenConfirm"]             = "飞享套餐开通确认";
    mapBusinessName["FEnjoyPackage28Yuan"]                  = "4G飞享套餐28元";
    mapBusinessName["FEnjoyPackage38Yuan"]                  = "4G飞享套餐38元";
    mapBusinessName["FEnjoyPackage48Yuan"]                  = "4G飞享套餐48元";
    mapBusinessName["FEnjoyPackage58Yuan"]                  = "4G飞享套餐58元";
    mapBusinessName["FEnjoyPackage88Yuan"]                  = "4G飞享套餐88元";
    mapBusinessName["FEnjoyPackage138Yuan"]                 = "4G飞享套餐138元";
    mapBusinessName["FEnjoyPackage158Yuan"]                 = "4G飞享套餐158元";
    mapBusinessName["FEnjoyPackage238Yuan"]                 = "4G飞享套餐238元";
    mapBusinessName["FEnjoyPackage268Yuan"]                 = "4G飞享套餐268元";
    mapBusinessName["FEnjoyPackage338Yuan"]                 = "4G飞享套餐338元";
    mapBusinessName["FEnjoyPackage588Yuan"]                 = "4G飞享套餐588元";
    mapBusinessName["TrafficBusiness"]                      = "流量业务";
    mapBusinessName["TrafficBusinessSelect"]                = "流量业务";
    mapBusinessName["GPRSOpen"]                             = "GPRS功能开通";
    mapBusinessName["GPRSOpenConfirm"]                      = "GPRS功能开通";
    mapBusinessName["TrafficCancel"]                        = "流量取消";
    mapBusinessName["TrafficCancelSelect"]                  = "流量取消";
    mapBusinessName["TrafficPackCancelConfirm"]             = "流量取消";
    mapBusinessName["BaseTrafficPackCancel"]                = "基础流量包取消";
    mapBusinessName["BaseTrafficPackCancelSelect"]          = "基础流量包取消";
    mapBusinessName["TenYuanNightTrafficPackCancel"]        = "10元夜间流量包取消";
    mapBusinessName["FuelChargingPackCancel"]               = "加油包取消";
    mapBusinessName["DayTrafficPackCancel"]                 = "日流量包取消";
    mapBusinessName["HourTrafficPackCancel"]                = "小时流量包取消";
    mapBusinessName["WeekendTrafficPackCancel"]             = "周末流量包取消";
    mapBusinessName["TenYuanTrafficPackCancel"]             = "10元流量包取消";
    mapBusinessName["TwentyYuanTrafficPackCancel"]          = "20元流量包取消";
    mapBusinessName["TrafficOpen"]                          = "流量开通";
    mapBusinessName["TrafficOpenSelect"]                    = "流量开通";
    mapBusinessName["MoneyTrafficOpenSelect"]               = "流量开通";
    mapBusinessName["MultiMoneyTrafficOpenSelect"]          = "流量开通";
    mapBusinessName["BaseTrafficPack"]                      = "基础流量包开通";
    mapBusinessName["BaseTrafficPackSelect"]                = "基础流量包开通";
    mapBusinessName["BaseTrafficPack30Open"]                = "30元基础流量包开通";
    mapBusinessName["BaseTrafficPackOpenConfirm"]           = "基础流量包开通";
    mapBusinessName["BaseTrafficPack50Open"]                = "50元基础流量包开通";
    mapBusinessName["BaseTrafficPack70Open"]                = "70元基础流量包开通";
    mapBusinessName["BaseTrafficPack100Open"]               = "100元基础流量包开通";
    mapBusinessName["BaseTrafficPack150Open"]               = "150元基础流量包开通 ";
    mapBusinessName["BaseTrafficPack200Open"]               = "200元基础流量包开通";
    mapBusinessName["BaseTrafficPack300Open"]               = "300元基础流量包开通";
    mapBusinessName["TenYuanNightTrafficPackOpen"]          = "10元夜间流量包开通";
    mapBusinessName["TenYuanNightTrafficPackOpenConfirm"]   = "10元夜间流量包开通";
    mapBusinessName["TrafficFuelPack"]                      = "流量加油包";
    mapBusinessName["TrafficFuelPackSelect"]                = "流量加油包";
    mapBusinessName["TrafficFuelPackOpenTen"]               = "10元国内流量加油包开通";
    mapBusinessName["TrafficFuelPackOpenConfirm"]           = "流量加油包";
    mapBusinessName["TrafficFuelPackOpenTwenty"]            = "20元国内流量加油包开通";
    mapBusinessName["TrafficFuelPackOpenThirty"]            = "30元国内流量加油包开通";
    mapBusinessName["TrafficFuelPackOpenFifty"]             = "50元国内流量加油包开通";
    mapBusinessName["HourTrafficPack"]                      = "小时流量包";
    mapBusinessName["HourTrafficPackOpenSelect"]            = "小时流量包";
    mapBusinessName["SixYuanHourPackOpen"]                  = "6元国内流量4小时套餐开通";
    mapBusinessName["SixYuanHourPackOpenConfirm"]           = "6元国内流量4小时套餐开通";
    mapBusinessName["NineYuanHourPackOpen"]                 = "9元国内流量4小时套餐开通";
    mapBusinessName["NineYuanHourPackOpenConfirm"]          = "9元国内流量4小时套餐开通";
    mapBusinessName["TenYuanDayPackOpen"]                   = "10元1G国内流量日套餐开通";
    mapBusinessName["TenYuanDayPackOpenConfirm"]            = "10元1G国内流量日套餐开通";
    mapBusinessName["FiveYuanShortPackOpen"]                = "5元500M省内流量短时包开通";
    mapBusinessName["FiveYuanShortPackOpenConfirm"]         = "5元500M省内流量短时包开通";
    return mapBusinessName;
}
}

