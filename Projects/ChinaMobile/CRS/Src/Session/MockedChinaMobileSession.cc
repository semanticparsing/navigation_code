#include "ChinaMobileSession.h"
#include "Common/Base/stringUtil.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"
#include "Robot/TCD/FrameTaskDman/Utils/RedisWrapper.h"
#include "Projects/ChinaMobile/ConfigurationManager/ConfigurationManager.h"

#include <string>
#include <map>

using namespace std;

DEFINE_bool(china_mobile_businesslog, true, "业务日志开关");
//Mocked data
DEFINE_int32(IsNight, 1, "");
DEFINE_int32(IsOff, 1, "");
DEFINE_int32(HasInclusivePackageInfo, 1, "");
DEFINE_string(GetOffReason, "强制停机", "");
DEFINE_string(GetTelephoneNumber, "13800000001", "");
DEFINE_string(GetBalance, "0.0", "");

namespace ChinaMobileProvince {

//初始化Session对象
bool ChinaMobileSession::Init(string IVRSessionId, string IVRTelephoneNumber) 
{
    _sessionId = IVRSessionId;
    _telephoneNumber = IVRTelephoneNumber;
    return true;
}

//通过request初始化Session对象
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
			_start_time = "";
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
    return true;
}

//保持到redis数据库
bool ChinaMobileSession::Save() 
{
    return true;
}

//从redis读取
bool ChinaMobileSession::Load(const std::string& inter_idx)
{
    return true;
}

// 获取当前手机号
bool ChinaMobileSession::GetTelephoneNumber(string &telephoneNumber)
{
    _telephoneNumber = FLAGS_GetTelephoneNumber;
    return true;
}

// 查询余额
bool ChinaMobileSession::QueryBalance(string &balance)
{
    return GetStringFromRedis(__FUNCTION__, balance);
}

// 查询套餐余量
bool ChinaMobileSession::QueryPackageInfo(Package_Info &package_info)
{
    string strnum = "0";
    bool ret = GetStringFromRedis(__FUNCTION__, strnum);
    if (!ret) {
        VLOG(5) << __FUNCTION__ << ", ERROR: " << ", Value: " << strnum;
        return false;
    }
    int num = std::stoi(strnum);
    
    if (num == 0) {
        package_info.TALK.flag = false;
        package_info.FAMILY_TALK.flag = false;
        package_info.GPRS.flag = false;
        package_info.AREA_GPRS.flag = false;
        
    } else if (num == 1){
        package_info.TALK.flag = true;
        package_info.TALK.TALK_TOTAL = "100";
        package_info.TALK.TALK_FREE = "20";
        package_info.TALK.TALK_USED = "80";
        
        package_info.FAMILY_TALK.flag = false;
        
        package_info.GPRS.flag = true;
        package_info.GPRS.GPRS_TOTAL = "1GB";
        package_info.GPRS.GPRS_FREE = "800MB";
        package_info.GPRS.GPRS_USED = "224MB";
        
        package_info.AREA_GPRS.flag = false;
    } else if (num == 2){
        package_info.TALK.flag = false;
        
        package_info.FAMILY_TALK.flag = true;
        package_info.FAMILY_TALK.FAMILY_TALK_TOTAL = "300";
        package_info.FAMILY_TALK.FAMILY_TALK_FREE = "200";
        package_info.FAMILY_TALK.FAMILY_TALK_USED = "100";
        
        package_info.GPRS.flag = false;
        
        package_info.AREA_GPRS.flag = true;
        package_info.AREA_GPRS.AREA_GPRS_TOTAL = "3GB251.32MB";
        package_info.AREA_GPRS.AREA_GPRS_FREE = "3GB";
        package_info.AREA_GPRS.AREA_GPRS_USED = "251.32MB";
    } else if (num == 3){
        package_info.TALK.flag = true;
        package_info.TALK.TALK_TOTAL = "100";
        package_info.TALK.TALK_FREE = "20";
        package_info.TALK.TALK_USED = "80";
        
        package_info.FAMILY_TALK.flag = true;
        package_info.FAMILY_TALK.FAMILY_TALK_TOTAL = "300";
        package_info.FAMILY_TALK.FAMILY_TALK_FREE = "200";
        package_info.FAMILY_TALK.FAMILY_TALK_USED = "100";
        
        package_info.GPRS.flag = true;
        package_info.GPRS.GPRS_TOTAL = "1GB";
        package_info.GPRS.GPRS_FREE = "800MB";
        package_info.GPRS.GPRS_USED = "224MB";
        
        package_info.AREA_GPRS.flag = true;
        package_info.AREA_GPRS.AREA_GPRS_TOTAL = "3GB251.32MB";
        package_info.AREA_GPRS.AREA_GPRS_FREE = "3GB";
        package_info.AREA_GPRS.AREA_GPRS_USED = "251.32MB";
    } else if (num == 4){ 
        package_info.TALK.flag = true;
        package_info.TALK.TALK_TOTAL = "100";
        package_info.TALK.TALK_FREE = "20";
        package_info.TALK.TALK_USED = "80";
        
        package_info.FAMILY_TALK.flag = false;
        
        package_info.GPRS.flag = true;
        package_info.GPRS.GPRS_TOTAL = "1GB";
        package_info.GPRS.GPRS_FREE = "1GB";
        package_info.GPRS.GPRS_USED = "0MB";
    } else if (num == 5){
        package_info.TALK.flag = false;
        
        package_info.FAMILY_TALK.flag = true;
        package_info.FAMILY_TALK.FAMILY_TALK_TOTAL = "300";
        package_info.FAMILY_TALK.FAMILY_TALK_FREE = "200";
        package_info.FAMILY_TALK.FAMILY_TALK_USED = "100";
        
        package_info.GPRS.flag = false;
        
        package_info.AREA_GPRS.flag = true;
        package_info.AREA_GPRS.AREA_GPRS_TOTAL = "3GB251.32MB";
        package_info.AREA_GPRS.AREA_GPRS_FREE = "3GB251.32MB";
        package_info.AREA_GPRS.AREA_GPRS_USED = "0MB";
    } else if (num == 6){
        package_info.TALK.flag = true;
        package_info.TALK.TALK_TOTAL = "100";
        package_info.TALK.TALK_FREE = "20";
        package_info.TALK.TALK_USED = "80";
        
        package_info.FAMILY_TALK.flag = true;
        package_info.FAMILY_TALK.FAMILY_TALK_TOTAL = "300";
        package_info.FAMILY_TALK.FAMILY_TALK_FREE = "200";
        package_info.FAMILY_TALK.FAMILY_TALK_USED = "100";
        
        package_info.GPRS.flag = true;
        package_info.GPRS.GPRS_TOTAL = "1GB";
        package_info.GPRS.GPRS_FREE = "1GB";
        package_info.GPRS.GPRS_USED = "0MB";
        
        package_info.AREA_GPRS.flag = true;
        package_info.AREA_GPRS.AREA_GPRS_TOTAL = "3GB251.32MB";
        package_info.AREA_GPRS.AREA_GPRS_FREE = "3GB251.32MB";
        package_info.AREA_GPRS.AREA_GPRS_USED = "0MB";
    }
    
    return true;
}

// 4G专网和超额流量费
bool ChinaMobileSession::QueryFourGAndNetInfo(FourGAndNetInfo &fourGAndNetInfo)
{
    bool flag = false;
    bool ret = GetBoolFromRedis(__FUNCTION__, flag);
    if (flag && ret)
    {
        fourGAndNetInfo.FourG_FREE = "4GB";
    } 
    else 
    {
        fourGAndNetInfo.FourG_FREE = "0MB";
    }
    fourGAndNetInfo.NETFEE = "4";
    fourGAndNetInfo.AREA_GPRS_FREE = "20MB";
    return true;
}

//判断是否停机
bool ChinaMobileSession::IsOff(bool &flag) //判断是否停机
{
    return GetBoolFromRedis(__FUNCTION__, flag);
}

// 获取停机原因
bool ChinaMobileSession::GetOffReason(string &reason)
{
    return GetStringFromRedis(__FUNCTION__, reason);
}

// 获取主资费套餐信息
bool ChinaMobileSession::GetMaintarInfo(MaintarInfo &maintarInfo)
{
    bool flag = false;
    bool ret = GetBoolFromRedis(__FUNCTION__, flag);
    maintarInfo.prod_prc_name = "全球通158套餐";
    maintarInfo.monthly_rent_fee = "158";
    if (flag)
    {
        maintarInfo.fix_fee_list.push_back("主叫显示来电提醒包月套餐");
        maintarInfo.fix_fee_list.push_back("来电显示");
        maintarInfo.fix_fee_list.push_back("无线音乐特级会员");
    }
    
    ret = true;
    return ret;
}

// 获取最低消费信息
bool ChinaMobileSession::GetAlreadyInfo(string &lowest_spend)
{
    lowest_spend = "18";
    return true;
}

// 证件号码是否正确服务器端
bool ChinaMobileSession::IsLegalCertificateInputServer(string input, bool &flag)
{
    return GetBoolFromRedis(__FUNCTION__, flag);
}

// 是否实名制
bool ChinaMobileSession::IsRealName(bool &flag)
{
    return GetBoolFromRedis(__FUNCTION__, flag);
}

// 获取账单信息(上网费)
bool ChinaMobileSession::GetBillMsgInfo(string &strNetFee)
{
    return GetStringFromRedis(__FUNCTION__, strNetFee);
}

// 本卡资费短信发送
bool ChinaMobileSession::MsseageOutService()
{
    return true;
}

// 短信下发
bool ChinaMobileSession::SendMessage(string message) //短信下发
{
   return true;
}

// 密码重置短信发送
bool ChinaMobileSession::ResetTelephonePasswordSendSMS(bool &flag)
{
    return GetBoolFromRedis(__FUNCTION__, flag);
}

// 密码输入是否匹配 服务器端
bool ChinaMobileSession::IsMatchedPassword(string input, bool &flag)
{
    return GetBoolFromRedis(__FUNCTION__, flag);
}

// 密码是否锁定
bool ChinaMobileSession::IsPasswordLock(bool &flag)
{
    return GetBoolFromRedis(__FUNCTION__, flag);
}

// 密码修改后台处理
bool ChinaMobileSession::ModifyTelephonePassword(string oldPassword, string newPassword, string& flag)
{
    return GetStringFromRedis(__FUNCTION__, flag);
}

// 获取系统时间
bool ChinaMobileSession::GetSystemTime(tm &systemTime)
{
    string strTime = "";
    GetStringFromRedis(__FUNCTION__, strTime);
    if(strTime.size() == 19)
    {
        char *cha = (char*)strTime.data();
        int year, month, day, hour, minute, second;
        sscanf(cha, "%d/%d/%d/%d/%d/%d", &year, &month, &day, &hour, &minute, &second);
        systemTime.tm_year = year - 1900;
        systemTime.tm_mon = month - 1;
        systemTime.tm_mday = day;
        systemTime.tm_hour = hour;
        systemTime.tm_min = minute;
        systemTime.tm_sec = second;
        systemTime.tm_isdst = 0;
    }
    else
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
    }
    
    return true;
}

//当前时间段  0:(0点到8点]    1:(8点到11点]    2:(11点到14点]    3:(14点到18点]    4:(18点到24点]
bool ChinaMobileSession::GetCurrentTimeRange(string &currentTimeRange) //判断时间段
{
    tm t;
    GetSystemTime(t);
    int min = t.tm_sec + (t.tm_min * 60) + (t.tm_hour * 3600);
    if(min>0 && min<=28800){
        currentTimeRange = "0";
    }else if(min>28800 && min<=39600){
        currentTimeRange = "1";
    }else if(min>39600 && min<=50400){
        currentTimeRange = "2";
    }else if(min>50400 && min<=64800){
        currentTimeRange = "3";
    }else if(min>64800 || min==0){
        currentTimeRange = "4";
    }
    return true;
}

// 取年月（格式：201805）
bool ChinaMobileSession::GetCurrentYearMonth(string &currentYearMonth) //取年月201805
{
    tm t;
    GetSystemTime(t);
    
    char acYearMonth[64] = {0};
    sprintf(acYearMonth, "%d%02d", t.tm_year, t.tm_mon);
    currentYearMonth = acYearMonth;
    
    return true;
}

//判断是否是夜间服务时间段
bool ChinaMobileSession::IsNight(bool &flag) //判断是否是夜间服务时间段
{
    tm t;
    GetSystemTime(t);
    int min = t.tm_min + (t.tm_hour * 60);
    flag = (min <= 420 || min >= 1380) ? true : false; // 23:00到次日07:00为夜间时间
    return true;
}

// 证件号码是否正确
bool ChinaMobileSession::IsValidIDNumber(string id, bool &flag) //判断是否是合法身份证号码
{
    if (id == "*")
    {
        flag = true;
        return true;
    }
    if (id.length() != 18) {
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

//是否简单密码
bool ChinaMobileSession::IsSimplePassword(string userPassword, bool &flag) //判断是否是简单密码
{
    if (userPassword.length() != 6) {
        flag = false;
        return true; // 判断长度为6
    }
    const char *pwd = userPassword.c_str();
    for (int i=0; i<6; i++) //判断前6位是否是数字
    {
        if (pwd[i] < '0' || pwd[i] > '9') {
            flag = false;
            return true;
        }
    }
    bool isSame = true;
    for (int i=1; i<6; i++) //判断是否完全相同
    {
        if (pwd[i] != pwd[0]) {
            isSame = false;
            break;
        }
    }
    bool isSequentialIncrement = true;
    for (int i=0; i<5; i++) //判断连续递增
    {
        if (pwd[i] != pwd[i+1] - 1) {
            isSequentialIncrement = false;
            break;
        }
    }
    bool isSequentialDecremental = true;
    for (int i=0; i<5; i++) //判断连续递减
    {
        if (pwd[i] != pwd[i+1] + 1) {
            isSequentialDecremental = false;
            break;
        }
    }
    bool isPhoneNumContain = true;
    if(string::npos == _telephoneNumber.find(userPassword)){ // 判定手机号是否包含密码
        isPhoneNumContain = false;
    }

    flag = isSame || isSequentialIncrement || isSequentialDecremental || isPhoneNumContain;
    return true;
}

// 密码输入是否合理
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

bool ChinaMobileSession::GetBoolFromRedis(const std::string& func, bool& ret)
{
    VLOG(2) << __FUNCTION__ << ", FUNC: " << func;
    if ( TrioDM::RedisWrapper::Instance() != NULL )
    {
        std::string key = _sessionId + ":" + func;
        std::string value = TrioDM::RedisWrapper::Instance()->Get(key);
        if ( value.empty() )
        {
            VLOG(2) << __FUNCTION__ << ", Vlaue.empty., Key: " << key; 
            return false;
        }
        else
        {
            ret = (value == "1");
            VLOG(5) << __FUNCTION__ << ", ret: " << ret;
            return true;
        }
    }
    return false;
}

bool ChinaMobileSession::GetStringFromRedis(const std::string& func, std::string& ret)
{
    if ( TrioDM::RedisWrapper::Instance() != NULL )
    {
        std::string key = _sessionId + ":" + func;
        std::string value = TrioDM::RedisWrapper::Instance()->Get(key);
        VLOG(5) << __FUNCTION__ << ", func: " <<  func << ", Value: " << value;
        if ( value.empty() )
        {
            return false;
        }
        else
        {
            ret = value;
        }
        return true;
    }
    return false;
}

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

bool ChinaMobileSession::GetCloseReason(string &userCloseReason)
{
    userCloseReason = _closeReason;
    return true;
}

bool ChinaMobileSession::SetCloseReason(const string &userCloseReason)
{
    if ( _closeReason.empty() ) {
        _closeReason = userCloseReason;
    }
    return true;
}

bool ChinaMobileSession::IsNeedSendSatisfactionSMS(bool &ret)
{
    ret = false;
    return true;
}

bool ChinaMobileSession::SendSatisfactionMessage(string callId, string content) {
    return false;
}

bool ChinaMobileSession::GetBrandId(string &userBrandId)
{
    bool result = true;

    userBrandId = "zyzxBrandId";

    return result;
}

bool ChinaMobileSession::GetRegion(string &userRegion)
{
    bool result = true;

    userRegion = "zyzxRegion";

    return result;
}

bool ChinaMobileSession::GetTrustLevel(string &userRegion)
{
    bool result = true;

    userRegion = "0";

    return result;
}

bool ChinaMobileSession::GetPayModeId(string &userPayModeId)
{
    bool result = true;

    userPayModeId = "GetPayModeId";

    return result;
}

bool ChinaMobileSession::SetIncommuInfo(const Json::Value &userIncommuLogInfo)
{
    _incommu_info = userIncommuLogInfo;
    return true;
}
bool ChinaMobileSession::GetIncommuInfo(Json::Value &userIncommuLogInfo) {
    bool result = true;
    userIncommuLogInfo = _incommu_info;
    return result;
}

}
