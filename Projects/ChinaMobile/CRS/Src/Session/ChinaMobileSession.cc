#include "ChinaMobileSession.h"
#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Common/Base/stringUtil.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"
#include "Common/Util/HttpClient/curlWrapper.h"
#include "Projects/ChinaMobile/ConfigurationManager/ConfigurationManager.h"
#include "Projects/ChinaMobile/CRS/Src/Interface/OperateRedisInterface.h"
#include "Projects/ChinaMobile/CRS/Src/Interface/HttpOperator.h"
#include "Projects/ChinaMobile/CRS/Src/CRSLogger/CSFInterfaceLogger.h"

#include <memory>
#include <string>
#include <map>
#include <algorithm>

using namespace std;
using namespace ConfigurationManager;

DEFINE_string(CSF_URL, "/httpservice/csf", "csf_url");
DEFINE_string(SERVICECODE_FOURGANDNET, "NGCRMPF_JL_FOURGONLYANDNETOUTSERVICE_POST", "4G专网和超额流量费");
DEFINE_string(SERVICECODE_MSSEAGEOUTSERVICE, "NGCRMPF_JL_MSSEAGEOUTSERVICE_POST", "本卡资费短信发送");
DEFINE_string(SERVICECODE_SMSOUTSERVICE, "NGCRMPF_JL_SMSOUTSERVICE_POST", "短信发送");
DEFINE_string(SERVICECODE_BALANCERECORD, "NGCRMPF_JL_FEEINFOSERVICE_POST", "话费查询");
DEFINE_string(SERVICECODE_PASSWORDLOCK, "NGCRMPF_JL_PASSWORDSTATUSOUTSERVICE_POST", "密码锁定查询");
DEFINE_string(SERVICECODE_PASSWORDOUTSERVICE, "NGCRMPF_JL_PASSWORDSERVICE_POST", "密码校验");
DEFINE_string(SERVICECODE_PASSWORDINFOOUTSERVICE, "NGCRMPF_JL_PASSWORDINFOSERVICE_POST", "密码修改");
DEFINE_string(SERVICECODE_IDNOINFOSERVICE, "NGCRMPF_JL_IDNOINFOSERVICE_POST", "身份证校验");
DEFINE_string(SERVICECODE_USERTOFSERVICE, "NGCRMPF_JL_USERTOFSERVICE_POST", "实名制校验");
DEFINE_string(SERVICECODE_NUMREMAINOUTSERVICE, "NGCRMPF_JL_NUMREMAINOUTSERVICE_POST", "套餐余量查询");
DEFINE_string(SERVICECODE_MOBILESTATUSSERVICE, "NGCRMPF_JL_MOBILESTATUSSERVICE_POST", "停机信息查询");
DEFINE_string(SERVICECODE_ALREADYOUTSERVICE, "NGCRMPF_JL_ALREADYOUTSERVICE_POST", "已开通业务查询短信发送");
DEFINE_string(SERVICECODE_BILLMSGOUTSERVICE, "NGCRMPF_JL_BILLMSGOUTSERVICE_POST", "账单查询");
DEFINE_string(SERVICECODE_MAINTAROUTSERVICE, "NGCRMPF_JL_MAINTAROUTSERVICE_POST", "主资费等相关信息");
DEFINE_string(SERVICECODE_RESET_PWD_SEND_SMS, "NGCRMPF_JL_PASSWORDSMSSERVICE_POST", "密码重置发送短信");

DEFINE_bool(china_mobile_businesslog, false, "业务日志开关");
DEFINE_string(Redis_SatisfactionSMS_Timeout, "864000", "满意度短信发送间隔时间，单位:秒，默认10天内不允许再次发送");

namespace ChinaMobileProvince {

DEFINE_string(Redis_Timeout, "300", "超时时间，单位:秒，默认5分钟");

#define SET_DATA_VALUE(data, value) do \
                                    { \
                                        if (data.isNull() || data.type() != Json::stringValue) \
                                        { \
                                            VLOG(0) << __FUNCTION__ << "\t"#data << "is not string" << ", Type: " << data.type(); \
                                            return false; \
                                        } \
                                        else \
                                        { \
                                            value = data.asString(); \
                                        } \
                                    } while(0)

bool ChinaMobileSession::Init(string IVRSessionId, string IVRTelephoneNumber)
{
    _sessionId = IVRSessionId;
    _telephoneNumber = IVRTelephoneNumber;
    _call_dst_id = ""; // 被叫号码
    _flow = "";
    _auth_token = ""; // 受权信息
    _entrance_id = "";
    _start_time = "";
    _ses_time = "";
    _suilu_region_id = "";
    _cc_id = "";
    _role_label = "";
    _closeReason = "";
    return true;
}

bool ChinaMobileSession::Init(const ChinaMobileBundle &request) //通过request初始化Session对象
{    
    if (request.__isset.inparams)
    {
        if (request.inparams.__isset.call_id)
        {
            _sessionId = request.inparams.call_id;
        }
        else
        {
            _sessionId = "";
        }
        if (request.inparams.__isset.call_sor_id)
        {
            _telephoneNumber = request.inparams.call_sor_id;
        }
        else
        {
            _telephoneNumber = "";
        }
        if (request.inparams.__isset.call_dst_id) 
        {
            _call_dst_id = request.inparams.call_dst_id;
        }
        else
        {
            _call_dst_id = "";
        }
        if (request.inparams.__isset.flow) 
        {
            _flow = request.inparams.flow;
        }
        else
        {
            _flow = "";
        }
        if (request.inparams.__isset.auth_token) 
        {
            _auth_token = request.inparams.auth_token;
        }
        else
        {
            _auth_token = "";
        }
        if (request.inparams.__isset.entrance_id) 
        {
            _entrance_id = request.inparams.entrance_id;
        }
        else
        {
            _entrance_id = "";
        }
        if (request.inparams.__isset.start_time) 
        {
            _start_time = request.inparams.start_time;
        }
        else
        {
            Time c_start_time = Time::NowFromSystemTime();
            _start_time = ChinaMobileProvince::ChinaMobileCSFInterfaceLogger::Time_tToFormatString(c_start_time.ToTimeT());
        }
        if (request.inparams.__isset.ses_time) 
        {
            _ses_time = request.inparams.ses_time;
        }
        else
        {
            _ses_time = "";
        }
        if (request.inparams.__isset.suilu_region_id) 
        {
            _suilu_region_id = request.inparams.suilu_region_id;
        }
        else
        {
            _suilu_region_id = "";
        }
        if (request.inparams.__isset.cc_id) 
        {
            _cc_id = request.inparams.cc_id;
        }
        else
        {
            _cc_id = "";
        }
        if (request.inparams.__isset.role_label) 
        {
            _role_label = request.inparams.role_label;
        }
        else
        {
            _role_label = "";
        }
    }
    _closeReason = "";
    return true;
}

bool ChinaMobileSession::Save()
{
    Json::Value jsonRequest;
    jsonRequest["sessionId"] = _sessionId;
    jsonRequest["telephoneNumber"] = _telephoneNumber;
    
    //ChinaMobileBundle information
    jsonRequest["call_dst_id"] = _call_dst_id;
    jsonRequest["flow"] = _flow;
    jsonRequest["auth_token"] = _auth_token;
    jsonRequest["entrance_id"] = _entrance_id;
    jsonRequest["start_time"] = _start_time;
    jsonRequest["ses_time"] = _ses_time;
    jsonRequest["suilu_region_id"] = _suilu_region_id;
    jsonRequest["cc_id"] = _cc_id;
    jsonRequest["role_label"] = _role_label;
    jsonRequest["closeReason"] = _closeReason;

    // flume info
    jsonRequest["brandId"] = _brandId;
    jsonRequest["region"] = _region;
    jsonRequest["trustLevel"] = _trustLevel;
    jsonRequest["payModeId"] = _payModeId;
    jsonRequest["session_inter_idx"] = _session_inter_idx;

    // flume 日志用的 incommu 信息
    jsonRequest["incommu_info"] = _incommu_info;
    
    Json::FastWriter jsonWriter;
    string redisValue = jsonWriter.write(jsonRequest);
    bool isExist = false;
    bool redisOperatorSucceed = false;
    if (RedisOperator_Exist(_sessionId, isExist))
    {
        if (isExist)
        {
            RedisOperator_Remove(_sessionId, redisOperatorSucceed);
        }
    }
    redisOperatorSucceed = RedisOperator_Set(_sessionId, redisValue, FLAGS_Redis_Timeout);
    VLOG(DEBUG) << __FUNCTION__ << ", redisValue = " << redisValue;
    return redisOperatorSucceed;
}

bool ChinaMobileSession::Load(const std::string& inter_idx)
{
    string value = "";
    _session_inter_idx = inter_idx;
    _i_interface_idx = 0;
    
    if (RedisOperator_Get(_sessionId, value))
    {
        VLOG(DEBUG) << __FUNCTION__ << ", value = " << value;
        unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
        Json::Value jsonResult;
        if (!reader->parse(value, jsonResult))
        {
            VLOG(0) << "session redis json format error!";
            return false;
        }
        SET_DATA_VALUE(jsonResult["sessionId"], _sessionId);
        SET_DATA_VALUE(jsonResult["telephoneNumber"], _telephoneNumber);
        //ChinaMobileBundle information
        SET_DATA_VALUE(jsonResult["call_dst_id"], _call_dst_id);
        SET_DATA_VALUE(jsonResult["flow"], _flow);
        SET_DATA_VALUE(jsonResult["auth_token"], _auth_token);
        SET_DATA_VALUE(jsonResult["entrance_id"], _entrance_id);
        SET_DATA_VALUE(jsonResult["start_time"], _start_time);
        SET_DATA_VALUE(jsonResult["ses_time"], _ses_time);
        SET_DATA_VALUE(jsonResult["suilu_region_id"], _suilu_region_id);
        SET_DATA_VALUE(jsonResult["cc_id"], _cc_id);
        SET_DATA_VALUE(jsonResult["role_label"], _role_label);
        SET_DATA_VALUE(jsonResult["closeReason"], _closeReason);
        // flume info
        SET_DATA_VALUE(jsonResult["brandId"], _brandId);
        SET_DATA_VALUE(jsonResult["region"], _region);
        SET_DATA_VALUE(jsonResult["trustLevel"], _trustLevel);
        SET_DATA_VALUE(jsonResult["payModeId"], _payModeId);
        SET_DATA_VALUE(jsonResult["session_inter_idx"], _session_inter_idx);
        // flume 日志用的 incommu 信息
        _incommu_info = jsonResult["incommu_info"];
        return true;
    }
    return false;
}

bool ChinaMobileSession::QueryBalance(string &balance)
{
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_BALANCERECORD;
    jsonRequest["jsonParam"]["params"]["userMobile"] = _telephoneNumber;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }
    if (jsonResult["object"]["OUT_DATA"].isNull())
    {//数据节点:当respCode输出0时输出业务数据
        VLOG(0) << __FUNCTION__ << " return exception ! OUT_DATA is null";
        return false;
    }

    //REMAIN_FEE : 余额（元）
    SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["REMAIN_FEE"], balance);

    return true;
}

bool ChinaMobileSession::QueryPackageInfo(Package_Info &package_info)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_NUMREMAINOUTSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }
    if (jsonResult["object"]["OUT_DATA"].isNull())
    {//数据节点:当respCode输出0时输出业务数据
        VLOG(0) << __FUNCTION__ << " return exception ! OUT_DATA is null";
        return false;
    }

    //语音数据
    if (jsonResult["object"]["OUT_DATA"]["TALK"].isNull())
    {
        VLOG(5) << __FUNCTION__ << " TALK is null";
        package_info.TALK.flag = false;
    }
    else
    {
        package_info.TALK.flag = true;
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["TALK"]["TALK_TOTAL"], package_info.TALK.TALK_TOTAL);
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["TALK"]["TALK_USED"], package_info.TALK.TALK_USED);
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["TALK"]["TALK_FREE"], package_info.TALK.TALK_FREE);
    }

    //幸福家庭语音数据
    if (jsonResult["object"]["OUT_DATA"]["FAMILY_TALK"].isNull())
    {
        VLOG(5) << __FUNCTION__ << " FAMILY_TALK is null";
        package_info.FAMILY_TALK.flag = false;
    }
    else
    {
        package_info.FAMILY_TALK.flag = true;
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["FAMILY_TALK"]["FAMILY_TALK_TOTAL"], package_info.FAMILY_TALK.FAMILY_TALK_TOTAL);
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["FAMILY_TALK"]["FAMILY_TALK_USED"], package_info.FAMILY_TALK.FAMILY_TALK_USED);
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["FAMILY_TALK"]["FAMILY_TALK_FREE"], package_info.FAMILY_TALK.FAMILY_TALK_FREE);
    }

    //国内流量数据
    if (jsonResult["object"]["OUT_DATA"]["GPRS"].isNull())
    {
        VLOG(5) << __FUNCTION__ << " GPRS is null";
        package_info.GPRS.flag = false;
    }
    else
    {
        package_info.GPRS.flag = true;
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["GPRS"]["GPRS_TOTAL"], package_info.GPRS.GPRS_TOTAL);
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["GPRS"]["GPRS_USED"], package_info.GPRS.GPRS_USED);
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["GPRS"]["GPRS_FREE"], package_info.GPRS.GPRS_FREE);
    }

    //省内流量数据
    if (jsonResult["object"]["OUT_DATA"]["AREA_GPRS"].isNull())
    {
        VLOG(5) << __FUNCTION__ << " AREA_GPRS is null";
        package_info.AREA_GPRS.flag = false;
    }
    else
    {
        package_info.AREA_GPRS.flag = true;
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["AREA_GPRS"]["AREA_GPRS_TOTAL"], package_info.AREA_GPRS.AREA_GPRS_TOTAL);
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["AREA_GPRS"]["AREA_GPRS_USED"], package_info.AREA_GPRS.AREA_GPRS_USED);
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["AREA_GPRS"]["AREA_GPRS_FREE"], package_info.AREA_GPRS.AREA_GPRS_FREE);
    }

    return true;
}

bool ChinaMobileSession::GetMaintarInfo(MaintarInfo &maintarInfo)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_MAINTAROUTSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }
    if (jsonResult["object"]["OUT_DATA"].isNull())
    {//数据节点:当respCode输出0时输出业务数据
        VLOG(0) << __FUNCTION__ << " return exception ! OUT_DATA is null";
        return false;
    }
    SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["PROD_PRC_NAME"], maintarInfo.prod_prc_name);
    SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["FEE1"], maintarInfo.monthly_rent_fee);
    const Json::Value& feeList = jsonResult["object"]["OUT_DATA"]["PROD_PRC_NAMEarry1"];
    if (feeList.isArray())
    {
        for (size_t i = 0; i < feeList.size(); i++)
        {
            string prod_prc_name = "";
            SET_DATA_VALUE(feeList[i]["PROD_PRC_NAME"], prod_prc_name);
            maintarInfo.fix_fee_list.push_back(prod_prc_name);
        }
    }

    return true;
}

bool ChinaMobileSession::GetBillMsgInfo(string &strNetFee)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_BILLMSGOUTSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    string currentYearMonth = "";
    GetCurrentYearMonth(currentYearMonth);
    jsonRequest["jsonParam"]["params"]["Month"] = currentYearMonth;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }
    if (jsonResult["object"]["OUT_DATA"].isNull())
    {//数据节点:当respCode输出0时输出业务数据
        VLOG(0) << __FUNCTION__ << " return exception ! OUT_DATA is null";
        return false;
    }

    strNetFee = "0";
    if (!jsonResult["object"]["OUT_DATA"]["NETFEE"].isNull())
    {
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["NETFEE"], strNetFee);
    }

    return true;
}

bool ChinaMobileSession::GetAlreadyInfo(string &strlowest)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_ALREADYOUTSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }
    if (jsonResult["object"]["OUT_DATA"].isNull())
    {//数据节点:当respCode输出0时输出业务数据
        VLOG(0) << __FUNCTION__ << " return exception ! OUT_DATA is null";
        return false;
    }
    SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["lowest"], strlowest);

    return true;
}

bool ChinaMobileSession::GetOffReason(string &strReason)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_MOBILESTATUSSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }
    if (jsonResult["object"]["OUT_DATA"].isNull())
    {//数据节点:当respCode输出0时输出业务数据
        VLOG(0) << __FUNCTION__ << " return exception ! OUT_DATA is null";
        return false;
    }
    SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["USR_CODE"], strReason);

    return true;
}

bool ChinaMobileSession::IsOff(bool &flag)
{
    string strReason = "";
    bool ret = GetOffReason(strReason);
    if (!ret)
    {
        return false;
    }
    if (strReason.compare("0") == 0)
    {
        flag = false;
    }
    else
    {
        flag = true;
    }
    return ret;
}

bool ChinaMobileSession::IsRealName(bool &flag)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_USERTOFSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }
    if (jsonResult["object"]["OUT_DATA"].isNull())
    {//数据节点:当respCode输出0时输出业务数据
        VLOG(0) << __FUNCTION__ << " return exception ! OUT_DATA is null";
        return false;
    }
    string strFlag = "";
    SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["TrueFlag"], strFlag);
    if (strFlag == "Y")
    {
        flag = true;
    }
    else if (strFlag == "N")
    {
        flag = false;
    }
    else
    {
        VLOG(0) << __FUNCTION__ << " return exception ! Real Name FLAG : "<< strFlag;
        return false;
    }

    return true;
}

bool ChinaMobileSession::IsLegalCertificateInputServer(string input, bool &flag)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_IDNOINFOSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    jsonRequest["jsonParam"]["params"]["IdentNo"] = input;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示校验失败
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode == "0")
    {       
        flag = true;
    }
    else
    {
         //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(5) << __FUNCTION__ << " id number error ! respCode : " << respCode << " respDesc : " << respDesc;
        flag = false;
    }

    return true;
}

bool ChinaMobileSession::GetTelephoneNumber(string &userTelephoneNumber)
{
    userTelephoneNumber = _telephoneNumber;
    return true;
}

bool ChinaMobileSession::CallCSF(string csfRequest, string& csfResponse)
{
    bool ret = false;

    string res_code = "0"; // 0-成功 1-失败
    tm tm_stime;
    GetSystemTime(tm_stime);
    int64 st_time = common::GetTimeInMs();
    VLOG(1) << __FUNCTION__ << ", start, call_id: " << _sessionId;
    int iRet = SendHttpPost2Java(FLAGS_CSF_URL, csfRequest, csfResponse);
    VLOG(1) << __FUNCTION__ << ", over, call_id: " << _sessionId << " call time : " << common::GetTimeInMs() - st_time;
    if ( 0 == iRet) {
        ret = true;
    }
    else {
        res_code = "1";
        ret = false;
        VLOG(0) << "Send Http Failed, msg:"<<csfResponse;
    }
    tm tm_etime;
    GetSystemTime(tm_etime);

    if (FLAGS_china_mobile_businesslog) {
        string interface_ecode = "";
        string business_name = "";
        string smsContent = "";
        GetBusinessInfoFromCsfRequest(csfRequest, business_name, smsContent);
        GetininterInfoFromCsfResponse(csfRequest, csfResponse, interface_ecode);
        string inter_idx = _session_inter_idx;
        string interface_idx = IntToString(_i_interface_idx);
        string accept_id = _call_dst_id;
        string templateCode = "";
        string interface_sendMsg = "";
        if ("0" != res_code) {
            interface_sendMsg = csfResponse;
        }
        string call_day_yd = _start_time;
        int64 st_time_ininter = common::GetTimeInMs();
        VLOG(1) << __FUNCTION__ << ", ininter start, call_id: " << _sessionId;
        bool logret = ChinaMobileCSFInterfaceLogger::IninterLogRecorder(csfRequest, interface_ecode, business_name, tm_stime, tm_etime,
                                                                            _sessionId, _telephoneNumber, res_code, smsContent,
                                                                            inter_idx, interface_idx, accept_id, templateCode, interface_sendMsg, call_day_yd);
        VLOG(1) << __FUNCTION__ << ", ininter over, call_id: " << _sessionId << " call time : " << common::GetTimeInMs() - st_time_ininter;
        _i_interface_idx++;
        if (!logret) {
            VLOG(0) << __FUNCTION__ << " CSFInterfaceLogger Fail!";
        }
    }

    return ret;
}

//4G专网和超额流量费
bool ChinaMobileSession::QueryFourGAndNetInfo(FourGAndNetInfo &fourGAndNetInfo)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_FOURGANDNET;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    string month = "";
    GetCurrentYearMonth(month);
    jsonRequest["jsonParam"]["params"]["Month"] = month;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }
    if (jsonResult["object"]["OUT_DATA"].isNull())
    {//数据节点:当respCode输出0时输出业务数据
        VLOG(0) << __FUNCTION__ << " return exception ! OUT_DATA is null";
        return false;
    }

    fourGAndNetInfo.FourG_FREE = "0MB";
    if (!jsonResult["object"]["OUT_DATA"]["FourG_FREE"].isNull())
    {//4G流量费
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["FourG_FREE"], fourGAndNetInfo.FourG_FREE);
    }
    fourGAndNetInfo.NETFEE = "0";
    if (!jsonResult["object"]["OUT_DATA"]["NETFEE"].isNull())
    {//超额流量费
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["NETFEE"], fourGAndNetInfo.NETFEE);
    }
    fourGAndNetInfo.AREA_GPRS_FREE = "0MB";
    if (!jsonResult["object"]["OUT_DATA"]["AREA_GPRS_FREE"].isNull())
    {//省内剩余流量
        SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["AREA_GPRS_FREE"], fourGAndNetInfo.AREA_GPRS_FREE);
    }

    return true;
}

//本卡资费短信发送
bool ChinaMobileSession::MsseageOutService()
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_MSSEAGEOUTSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }

    return true;
}

//短信发送
bool ChinaMobileSession::SendMessage(string message)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_SMSOUTSERVICE;
    jsonRequest["jsonParam"]["params"]["smsCode"] = _telephoneNumber;
    jsonRequest["jsonParam"]["params"]["smsContent"] = message;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }

    return true;
}

//密码锁定查询
bool ChinaMobileSession::IsPasswordLock(bool &flag)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_PASSWORDLOCK;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }
    if (jsonResult["object"]["OUT_DATA"].isNull())
    {//数据节点:当respCode输出0时输出业务数据
        VLOG(0) << __FUNCTION__ << " return exception ! OUT_DATA is null";
        return false;
    }
    
    string PWDLOCK_FLAG = "";//密码是否锁定
    SET_DATA_VALUE(jsonResult["object"]["OUT_DATA"]["PWDLOCK_FLAG"], PWDLOCK_FLAG);
    if (PWDLOCK_FLAG == "Y")
    {
        flag = true;
    }
    else if (PWDLOCK_FLAG == "N")
    {
        flag = false;
    }
    else
    {
        VLOG(0) << __FUNCTION__ << " return exception ! PWDLOCK_FLAG : "<< PWDLOCK_FLAG;
        return false;
    }
    
    return true;
}

//密码校验
bool ChinaMobileSession::IsMatchedPassword(string input, bool &isCheckOK)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_PASSWORDOUTSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    jsonRequest["jsonParam"]["params"]["pwd"] = input;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是校验成功，其他表示校验失败 
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode == "0")
    {
        isCheckOK = true;
    }
    else
    {
        isCheckOK = false;
    }

    return true;
}

//密码修改
bool ChinaMobileSession::ModifyTelephonePassword(string oldPassword, string newPassword, string &modifyResult)
{
    //请求报文
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_PASSWORDINFOOUTSERVICE;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    jsonRequest["jsonParam"]["params"]["oldpwd"] = oldPassword;
    jsonRequest["jsonParam"]["params"]["newpwd"] = newPassword;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是校验成功，其他表示校验失败 
    SET_DATA_VALUE(jsonResult["object"]["respCode"], modifyResult);

    return true;
}

bool ChinaMobileSession::GetSystemTime(tm &systemTime)
{
    time_t tt = time(NULL);
    tm* t= localtime(&tt); //t->tm_hour,t->tm_min,t->tm_sec
    systemTime.tm_sec = t->tm_sec;
    systemTime.tm_min = t->tm_min;
    systemTime.tm_hour = t->tm_hour;
    systemTime.tm_mday = t->tm_mday;
    systemTime.tm_mon = t->tm_mon;
    systemTime.tm_year = t->tm_year;
    systemTime.tm_wday = t->tm_wday;
    systemTime.tm_yday = t->tm_yday;
    systemTime.tm_isdst = t->tm_isdst;
    return true;
}

//当前时间段  0:(0点到8点]    1:(8点到11点]    2:(11点到14点]    3:(14点到18点]    4:(18点到24点]
bool ChinaMobileSession::GetCurrentTimeRange(string &currentTimeRange) //判断时间段
{
    tm t;
    GetSystemTime(t);
    int sec = t.tm_sec + (t.tm_min * 60) + (t.tm_hour * 3600);
    if (sec>0 && sec<=28800)
    {
        currentTimeRange = "0";
    }
    else if (sec>28800 && sec<=39600)
    {
        currentTimeRange = "1";
    }
    else if (sec>39600 && sec<=50400)
    {
        currentTimeRange = "2";
    }
    else if (sec>50400 && sec<=64800)
    {
        currentTimeRange = "3";
    }
    else if (sec>64800 || sec==0)
    {
        currentTimeRange = "4";
    }
    return true;
}

bool ChinaMobileSession::GetCurrentYearMonth(string &currentYearMonth) //取年月201805
{
    tm t;
    GetSystemTime(t);
    
    string findingYear = IntToString(t.tm_year + 1900);
    string findingMonth = IntToString(t.tm_mon + 1);
    if (findingMonth.length() <= 1)
    {
        findingMonth = "0" + findingMonth;
    }
    currentYearMonth = findingYear + findingMonth;
    
    return true;
}

bool ChinaMobileSession::IsNight(bool &flag) //判断是否是夜间服务时间段
{
    tm t;
    GetSystemTime(t);
    int min = t.tm_min + (t.tm_hour * 60);
    flag = (min <= 420 || min >= 1380) ? true : false; // 23:00到次日07:00为夜间时间
    return true;
}

bool ChinaMobileSession::IsValidIDNumber(string id, bool &flag) //判断是否是合法身份证号码
{
    if (id == "*")
    {
        flag = true;
        return true;
    }
    if (id.length() != 18)
    {
        flag = false;
        return true;
    }
    for (int i=0; i < id.length() - 1; i++)
    {
        if (id.substr(i, 1) > "9" || id.substr(i,1) < "0")
        {
            flag = false;
            return true;
        }
    }
    if ((id.substr(id.length()-1, 1) > "9" || id.substr(id.length()-1, 1) < "0" ) && id.substr(id.length()-1, 1) != "*")
    {
        flag = false;
        return true;
    }
    flag = true;
    return true;
}

bool ChinaMobileSession::IsSimplePassword(string userPassword, bool &flag) //判断是否是简单密码
{
    if (userPassword.length() != 6)
    {
        flag = false;
        return true; // 判断长度为6
    }
    const char *pwd = userPassword.c_str();
    for (int i=0; i<6; i++) //判断前6位是否是数字
    {
        if (pwd[i] < '0' || pwd[i] > '9')
        {
            flag = false;
            return true;
        }
    }
    bool isSame = true;
    for (int i=1; i<6; i++) //判断是否完全相同
    {
        if (pwd[i] != pwd[0])
        {
            isSame = false;
            break;
        }
    }
    bool isSequentialIncrement = true;
    for (int i=0; i<5; i++) //判断连续递增
    {
        if (pwd[i] != pwd[i+1] - 1)
        {
            isSequentialIncrement = false;
            break;
        }
    }
    bool isSequentialDecremental = true;
    for (int i=0; i<5; i++) //判断连续递减
    {
        if (pwd[i] != pwd[i+1] + 1)
        {
            isSequentialDecremental = false;
            break;
        }
    }
    bool isPhoneNumContain = true;
    if (string::npos == _telephoneNumber.find(userPassword))
    { // 判定手机号是否包含密码
        isPhoneNumContain = false;
    }

    flag = isSame || isSequentialIncrement || isSequentialDecremental || isPhoneNumContain;
    return true;
}

bool ChinaMobileSession::IsValidPassword(string userPassword, bool &flag) //判断是否是合法密码
{
    if (userPassword == "*")
    {
        flag = true;
        return true;
    }
    if (userPassword.length() != 6)
    {
        flag = false;
        return true;
    }
    for (int i=0; i < userPassword.length(); i++)
    {
        if (userPassword.substr(i, 1) > "9" || userPassword.substr(i,1) < "0")
        {
            flag = false;
            return true;
        }
    }
    flag = true;
    return true;
}

//重置密码发送短信
bool ChinaMobileSession::ResetTelephonePasswordSendSMS(bool &flag)
{
    flag = false;
    if ("" == _telephoneNumber)
    {
        VLOG(0) << __FUNCTION__ << " error telephone is empty.";
        return false;
    }
    Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_SERVICECODE_RESET_PWD_SEND_SMS;
    jsonRequest["jsonParam"]["params"]["BillId"] = _telephoneNumber;
    Json::FastWriter writer;
    string request = writer.write(jsonRequest);

    string reply = "";
    //调用CSF接口
    if (!CallCSF(request, reply))
    {
        VLOG(0) << __FUNCTION__ << " CallCSF error -> request : " << request;
        return false;
    }

    //解析返回报文
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if (!reader->parse(reply, jsonResult))
    {
        VLOG(0) << __FUNCTION__ << " json format error!";
        return false;
    }

    //rtnCode : 0是成功，其他表示省端接口平台调用异常
    string rtnCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    if (rtnCode != "0")
    {
        //rtnMsg : 提示信息，当retCode=0时为“成功”，其他情况为错误提示信息
        string rtnMsg = "";
        SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
        VLOG(0) << __FUNCTION__ << " return exception ! rtnCode : " << rtnCode << " rtnMsg : " << rtnMsg;
        return false;
    }
    if (jsonResult["object"].isNull())
    {//业务节点
        VLOG(0) << __FUNCTION__ << " return exception ! object is null";
        return false;
    }

    //respCode : 0是成功，其他表示接口调用异常
    string respCode = "";
    SET_DATA_VALUE(jsonResult["object"]["respCode"], respCode);
    if (respCode != "0")
    {
        //respDesc : 业务结果描述，默认输出success表示成功，异常时要明确返回异常的原因说明
        string respDesc = "";
        SET_DATA_VALUE(jsonResult["object"]["respDesc"], respDesc);
        VLOG(0) << __FUNCTION__ << " return exception ! respCode : " << respCode << " respDesc : " << respDesc;
        return false;
    }

    return true;
}

bool ChinaMobileSession::IsNeedSendSatisfactionSMS(bool &ret){

#if 0
    if(false == FLAGS_is_send_satisfaction_sms){
        ret = false;
        return true;
    }
    ret = true;
    string redisKey = "Satisfaction_" + telephoneNumber;
    bool isExist = false;
    if(RedisOperator_Exist(redisKey, isExist)){ // 判断是否10日内发送过短信
        if(isExist){
            ret = false;
            VLOG(3) << "[IsNeedSendSatisfactionSMS] no need send ,because sended in 10 days";
        }
    }
    else {
        VLOG(0) << "IsNeedSendSatisfactionSMS isExist redis failed.";
        return false;
    }
    if (true == ret) { // 判断时间不在 22:00-次日7:00 之间
        tm currentTime;
        if(GetSystemTime(currentTime)) {
            if(currentTime.tm_hour >= 22 || currentTime.tm_hour < 7) {
                VLOG(3) << "[IsNeedSendSatisfactionSMS] no need send ,because it is night";
                ret = false;
            }
        }
        else {
            VLOG(0) << "IsNeedSendSatisfactionSMS get system Time failed.";
            return false;
        }
    }
#endif
    return true;
}

bool ChinaMobileSession::SendSatisfactionMessage(string callId, string content) {
    
    if("" == callId) {
        callId = _sessionId;
    }
    if ("" == content) { // 当无输入时，返回功能需求说明书的内容一
        content = "您的满意是我们不懈的追求。请您回复序号，对本次智能语音服务的整体满意程度进行评价：1为非常满意；2为满意；3为一般；4为不满意。（本短信回复免费）【中国移动】";
    }
    bool ret = CallSendSatisfactionSMSInfo(callId, content, "uvn_tps"); // 根据文档写死为uvn_tps
    if (ret) { // 发送成功在redis中添加标志位，保证10天内不再下发
        string redisKey = "Satisfaction_" + _telephoneNumber;
        bool isExist = false;
        bool redisOperatorSucceed = false;
        if(RedisOperator_Exist(redisKey, isExist)){
            if(isExist){
                RedisOperator_Remove(redisKey, redisOperatorSucceed);
            }
        }
        string redisValue = "sended";
        redisOperatorSucceed = RedisOperator_Set(redisKey, redisValue, FLAGS_Redis_SatisfactionSMS_Timeout);
        if ( true != redisOperatorSucceed ) {
            VLOG(0) << __FUNCTION__ << ", operator redis set failed. ";
        }
    }
    return ret;
}

//ChinaMobileBundle information
bool ChinaMobileSession::GetCallDstId(string &callDstId) // 被叫号码
{
    callDstId = _call_dst_id;
    return true;
}

bool ChinaMobileSession::GetFlow(string &userflow)
{
    userflow = _flow;
    return true;
}

bool ChinaMobileSession::GetAuthToken(string &authToken) // 受权信息
{
    authToken = _auth_token;
    return true;
}

bool ChinaMobileSession::GetEntranceId(string &entranceId)
{
    entranceId = _entrance_id;
    return true;
}

bool ChinaMobileSession::GetStartTime(string &startTime)
{
    startTime = _start_time;
    return true;
}

bool ChinaMobileSession::GetSesTime(string &sesTime)
{
    sesTime = _ses_time;
    return true;
}

bool ChinaMobileSession::GetSuiluRegionId(string &suiluRegionId)
{
    suiluRegionId = _suilu_region_id;
    return true;
}

bool ChinaMobileSession::GetCCId(string &ccId)
{
    ccId = _cc_id;
    return true;
}

bool ChinaMobileSession::GetRoleLabel(string &roleLabel)
{
    roleLabel = _role_label;
    return true;
}

bool ChinaMobileSession::SetCloseReason(const string &userCloseReason)
{
    if ( _closeReason.empty() ) {
        _closeReason = userCloseReason;
    }
    return true;
}

bool ChinaMobileSession::GetCloseReason(string &userCloseReason)
{
    userCloseReason = _closeReason;
    return true;
}

bool ChinaMobileSession::SetIncommuInfo(const Json::Value &userIncommuLogInfo)
{
    _incommu_info = userIncommuLogInfo;
    return true;
}

bool ChinaMobileSession::GetIncommuInfo(Json::Value &userIncommuLogInfo)
{
    userIncommuLogInfo = _incommu_info;
    return true;
}

bool ChinaMobileSession::GetBrandId(string &userBrandId)
{
    if (_brandId != "" )
    {
        userBrandId = _brandId;
        return true;
    }
    bool result = CallQryUserInfo();
    if (result == true)
    {
        if (_brandId != "")
        {
            userBrandId = _brandId;
            return true;
        }
        else return false;
    }
    return result;
}

bool ChinaMobileSession::GetRegion(string &userRegion)
{
    if (_region != "" )
    {
        userRegion = _region;
        return true;
    }
    bool result = CallQryUserInfo();
    if (result == true)
    {
        if (_region != "")
        {
            userRegion = _region;
            return true;
        }
        else return false;
    }
    return result;
}

bool ChinaMobileSession::GetTrustLevel(string &userTrustLevel)
{
    if (_trustLevel != "" )
    {
        userTrustLevel = _trustLevel;
        return true;
    }
    bool result = CallQryUserInfo();
    if (result == true)
    {
        if (_trustLevel != "")
        {
            userTrustLevel = _trustLevel;
            return true;
        }
        else return false;
    }
    return result;
}

bool ChinaMobileSession::GetPayModeId(string &userPayModeId)
{
    if (_payModeId != "" )
    {
        userPayModeId = _payModeId;
        return true;
    }
    bool result = CallQryUserInfo();
    if (result == true)
    {
        if (_payModeId != "")
        {
            userPayModeId = _payModeId;
            return true;
        }
        else return false;
    }
    return result;
}

bool ChinaMobileSession::GetBusinessInfoFromCsfRequest(const string& csfRequest, string& business_name, string& smsContent)
{
    Json::Reader reader;
    Json::Value req_object;
    if (!reader.parse(csfRequest, req_object)) {
        VLOG(0) << "csfRequest json format error!" << csfRequest;
        return false;
    }

    if (req_object["serviceCode"] == FLAGS_SERVICECODE_FOURGANDNET) {  //1 4G专网和超额流量费
        business_name = "4G专网和超额流量费";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_MSSEAGEOUTSERVICE) {  //2 本卡资费短信发送
        business_name = "本卡资费短信发送";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_SMSOUTSERVICE) {   //3 短信发送
        business_name = "短信发送";
        SET_DATA_VALUE(req_object["jsonParam"]["params"]["smsContent"], smsContent);
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_BALANCERECORD) {  //4 话费查询
        business_name = "话费查询";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_PASSWORDLOCK) { //5 密码锁定查询
        business_name = "密码锁定查询";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_PASSWORDOUTSERVICE) {   //6 密码校验
        business_name = "密码校验";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_PASSWORDINFOOUTSERVICE) {   //7 密码修改
        business_name = "密码修改";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_IDNOINFOSERVICE) {  //8 身份证校验
        business_name = "身份证校验";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_USERTOFSERVICE) { //9 实名制校验
        business_name = "实名制校验";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_NUMREMAINOUTSERVICE) {  //10 套餐余量查询
        business_name = "套餐余量查询";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_MOBILESTATUSSERVICE) {  //11 停机信息查询
        business_name = "停机信息查询";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_ALREADYOUTSERVICE) { //12 已开通业务查询短信发送
        business_name = "已开通业务查询短信发送";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_BILLMSGOUTSERVICE) {  //13 账单查询
        business_name = "账单查询";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_MAINTAROUTSERVICE) {   //14 主资费等相关信息
        business_name = "主资费等相关信息";
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_RESET_PWD_SEND_SMS) {   //15 密码重置发送短信
        business_name = "密码重置发送短信";
    } else {
        VLOG(0) << "serviceCode error!";
        return false;
    }

    return true;
}
bool ChinaMobileSession::GetininterInfoFromCsfResponse(const string& csfRequest, const string& csfResponse, string& interface_ecode) {
    Json::Reader reader;
    Json::Value req_object;
    if (!reader.parse(csfRequest, req_object)) {
        VLOG(0) << "csfRequest json format error!" << csfRequest;
        return false;
    }
    Json::Value res_object;
    if (!reader.parse(csfResponse, res_object)) {
        VLOG(0) << "csfResponse json format error!" << csfResponse;
        return false;
    }
    interface_ecode = "000";
    if (req_object["serviceCode"] == FLAGS_SERVICECODE_FOURGANDNET) {  //1 4G专网和超额流量费
        // business_name = "4G专网和超额流量费";
        string ininter_rtnCode = "";
        //string ininter_retStatus = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        //SET_DATA_VALUE(res_object["object"]["status"], ininter_retStatus);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }/* else {
            if (ininter_retStatus != "0") {
                interface_ecode = "001";
            }
        }*/
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_MSSEAGEOUTSERVICE) {  //2 本卡资费短信发送
        // business_name = "本卡资费短信发送";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_SMSOUTSERVICE) {   //3 短信发送
        // business_name = "短信发送";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_BALANCERECORD) {  //4 话费查询
        // business_name = "话费查询";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_PASSWORDLOCK) { //5 密码锁定查询
        // business_name = "密码锁定查询";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_PASSWORDOUTSERVICE) {   //6 密码校验
        // business_name = "密码校验";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_PASSWORDINFOOUTSERVICE) {   //7 密码修改
        // business_name = "密码修改";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_IDNOINFOSERVICE) {  //8 身份证校验
        // business_name = "身份证校验";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_USERTOFSERVICE) { //9 实名制校验
        // business_name = "实名制校验";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_NUMREMAINOUTSERVICE) {  //10 套餐余量查询
        // business_name = "套餐余量查询";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_MOBILESTATUSSERVICE) {  //11 停机信息查询
        // business_name = "停机信息查询";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_ALREADYOUTSERVICE) { //12 已开通业务查询短信发送
        // business_name = "已开通业务查询短信发送";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_BILLMSGOUTSERVICE) {  //13 账单查询
        // business_name = "账单查询";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_MAINTAROUTSERVICE) {   //14 主资费等相关信息
        // business_name = "主资费等相关信息";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else if (req_object["serviceCode"] == FLAGS_SERVICECODE_RESET_PWD_SEND_SMS) {   //15 密码重置发送短信
        // business_name = "密码重置发送短信";
        string ininter_rtnCode = "";
        SET_DATA_VALUE(res_object["rtnCode"], ininter_rtnCode);
        if (ininter_rtnCode != "0") {
            interface_ecode = "-1";
        }
    } else {
        VLOG(0) << "serviceCode error!";
        return false;
    }
    // SET_DATA_VALUE(res_object["rtnCode"], interface_ecode);
    VLOG(DEBUG) << "GetininterInfoFromCsfResponse interface_ecode : " << interface_ecode;

    return true;
}

bool ChinaMobileSession::CallQryUserInfo()
{
    return false;
    /*Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_ServiceCode_UserInfo;
    jsonRequest["jsonParam"]["params"]["userMobile"] = telephoneNumber;
    if ( FLAGS_ServiceCode_UserInfo == "NGCRMPF_SD_CSDHQVNUSERINFONEW_POST" ) {
        jsonRequest["jsonParam"]["params"]["crmpfPubInfo"] = "";
    }

    Json::FastWriter writer;
    string request = writer.write(jsonRequest);    
    string reply = "";
    bool ret = CallCSF(request, reply);
    if(!ret){
        VLOG(WARNING) << __FUNCTION__ << ", CallCSF fail.";
        return false;
    }    
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if(!reader->parse(reply, jsonResult))
    {
        VLOG(0) << "json format error!";
        return false;
    }
    string rtnCode;
    string rtnMsg;
    string retStatus = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    SET_DATA_VALUE(jsonResult["rtnMsg"], rtnMsg);
    SET_DATA_VALUE(jsonResult["object"]["status"], retStatus);
    if (rtnCode != "0" || retStatus != "0") {
        VLOG(WARNING) << __FUNCTION__ << ", rtnCode: " << rtnCode << ", rtnMsg: " << rtnMsg;
        return false;
    }
    if ( FLAGS_ServiceCode_UserInfo == "NGCRMPF_SD_CSDHQVNUSERINFONEW_POST" ) {
        string userStatus;
        SET_DATA_VALUE(jsonResult["object"]["result"]["osState"], userStatus);
        string realNameType;
        SET_DATA_VALUE(jsonResult["object"]["result"]["realNameInfo"], realNameType);
        SET_DATA_VALUE(jsonResult["object"]["result"]["brandId"], brandId);
        SET_DATA_VALUE(jsonResult["object"]["result"]["regionId"], region);
        SET_DATA_VALUE(jsonResult["object"]["result"]["starLevel"], trustLevel);
        SET_DATA_VALUE(jsonResult["object"]["result"]["payType"], payModeId);
        SET_DATA_VALUE(jsonResult["object"]["result"]["prodName"], prodName);
        string telNumCreateTimeStr = "";
        SET_DATA_VALUE(jsonResult["object"]["result"]["userBegin"], telNumCreateTimeStr);
        memset(&telNumCreateTime, 0, sizeof(telNumCreateTime));
        if("" != telNumCreateTimeStr){
            strptime(telNumCreateTimeStr.c_str(), "%Y%m%d%H%M%S", &telNumCreateTime);
        }

        //停机原因
        if(realNameType == "未实名制认证" && userStatus == "US24") offReason = "强制停机";
        else if(userStatus == "US30") offReason = "欠费停机";
        else if(userStatus == "US20") offReason = "申请停机";
        else if(userStatus == "US10") offReason = "正常状态";
        else offReason = "其他原因";
        //实名制 1 已实名认证 0 未认证实名 2 非实名制
        if (realNameType == "已实名制认证") isRealName = "true";
        else isRealName = "false";
    } else {
        string userStatus;
        SET_DATA_VALUE(jsonResult["object"]["data"]["userStatus"], userStatus);
        string realNameType;
        SET_DATA_VALUE(jsonResult["object"]["data"]["realNameType"], realNameType);
        SET_DATA_VALUE(jsonResult["object"]["data"]["brandId"], brandId);
        SET_DATA_VALUE(jsonResult["object"]["data"]["region"], region);
        SET_DATA_VALUE(jsonResult["object"]["data"]["trustLevel"], trustLevel);
        SET_DATA_VALUE(jsonResult["object"]["data"]["PayModeId"], payModeId);
        SET_DATA_VALUE(jsonResult["object"]["data"]["prodIdName"], prodName);
        string telNumCreateTimeStr = "";
        SET_DATA_VALUE(jsonResult["object"]["data"]["createDate"], telNumCreateTimeStr);
        memset(&telNumCreateTime, 0, sizeof(telNumCreateTime));
        if("" != telNumCreateTimeStr){
            strptime(telNumCreateTimeStr.c_str(), "%Y-%m-%d %H:%M:%S", &telNumCreateTime);
        }

        //停机原因
        if(realNameType == "2" && userStatus == "8") offReason = "强制停机";
        else if(userStatus == "3" || userStatus == "4" || userStatus == "5" || userStatus == "6" || userStatus == "30") offReason = "欠费停机";
        else if(userStatus == "2") offReason = "申请停机";
        else if(userStatus == "1") offReason = "正常状态";
        else offReason = "其他原因";
        //实名制 1 已实名认证 0 未认证实名 2 非实名制
        if (realNameType == "1") isRealName = "true";
        else isRealName = "false";
    }
    return true;*/
}

bool ChinaMobileSession::CallSendSatisfactionSMSInfo(string callId, string message, string channelId) {
    return true;
    /*Json::Value jsonRequest;
    jsonRequest["serviceCode"] = FLAGS_ServiceCode_SendSatisfactionSMSInfo;
    jsonRequest["jsonParam"]["params"]["smsCode"] = telephoneNumber;
    jsonRequest["jsonParam"]["params"]["callId"] = callId;
    jsonRequest["jsonParam"]["params"]["content"] = message;
    jsonRequest["jsonParam"]["params"]["channelId"] = channelId;
    
    if( "none" != FLAGS_use_test_telephonenum ) { 
        jsonRequest["jsonParam"]["params"]["smsCode"] = FLAGS_use_test_telephonenum;
    }

    Json::FastWriter writer;
    string request = writer.write(jsonRequest);    
    string reply = "";
    bool ret = CallCSF(request, reply);
    if(!ret){
        return false;
    }
    
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    Json::Value jsonResult;
    if(!reader->parse(reply, jsonResult))
    {
        VLOG(0) << "json format error!";
        return false;
    }
    string rtnCode = "";
    string resultCode = "";
    SET_DATA_VALUE(jsonResult["rtnCode"], rtnCode);
    SET_DATA_VALUE(jsonResult["bean"]["result"], resultCode);
    if ("0" != rtnCode || "1" != resultCode) {
        VLOG(0) << "CallSendSatisfactionSMSInfo failed, rtnCode:" << rtnCode << ", result:" << resultCode;
        return false;
    }
    
    return true;*/
}

}
