#ifndef CHINA_MOBILE_SESSION_H_
#define CHINA_MOBILE_SESSION_H_

#include <map>
#include <string>
#include <vector>
#include <time.h>
#include "Projects/ChinaMobile/Proto/ChinaMobileCRSServing.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"

using namespace std;
using namespace chinaMobile;
namespace ChinaMobileProvince {

// 主资费套餐信息
struct MaintarInfo
{
    //主资费名称
    string prod_prc_name;
    //月租
    string monthly_rent_fee;
    //固定费业务
    vector<string> fix_fee_list;
    MaintarInfo()
    {
        prod_prc_name = "";
        monthly_rent_fee = "";
        fix_fee_list.clear();
    }
};

struct TALK_INFO
{
    bool flag;
    string TALK_TOTAL;
    string TALK_FREE;
    string TALK_USED;
    TALK_INFO()
    {
        flag = false;
        TALK_TOTAL = "";
        TALK_FREE = "";
        TALK_USED = "";
    }
};

struct FAMILY_TALK_INFO
{
    bool flag;
    string FAMILY_TALK_TOTAL;
    string FAMILY_TALK_FREE;
    string FAMILY_TALK_USED;
    FAMILY_TALK_INFO()
    {
        flag = false;
        FAMILY_TALK_TOTAL = "";
        FAMILY_TALK_FREE = "";
        FAMILY_TALK_USED = "";
    }
};

struct GPRS_INFO
{
    bool flag;
    string GPRS_TOTAL;
    string GPRS_FREE;
    string GPRS_USED;
    GPRS_INFO()
    {
        flag = false;
        GPRS_TOTAL = "";
        GPRS_FREE = "";
        GPRS_USED = "";
    }
};

struct AREA_GPRS_INFO
{
    bool flag;
    string AREA_GPRS_TOTAL;
    string AREA_GPRS_FREE;
    string AREA_GPRS_USED;
    AREA_GPRS_INFO()
    {
        flag = false;
        AREA_GPRS_TOTAL = "";
        AREA_GPRS_FREE = "";
        AREA_GPRS_USED = "";
    }
};

struct Package_Info
{
    TALK_INFO TALK;
    FAMILY_TALK_INFO FAMILY_TALK;
    GPRS_INFO GPRS;
    AREA_GPRS_INFO AREA_GPRS;
};

struct FourGAndNetInfo
{
    string NETFEE;
    string FourG_FREE;
    string AREA_GPRS_FREE;
    FourGAndNetInfo()
    {
        NETFEE = "";
        FourG_FREE = "";
        AREA_GPRS_FREE = "";
    }
};

/*
 * ChinaMobileSession用于保存一个唯一的对话
 * 每个对话有唯一的_sessionId进行标识
 * session的中间状态保存在redis中
 */
class ChinaMobileSession
{
public:
    ChinaMobileSession()
    {
        _sessionId = "";
        _telephoneNumber = "";
    }
    virtual ~ChinaMobileSession() {}

    bool Init(string IVRSessionId, string IVRTelephoneNumber); //初始化Session对象
    bool Init(const ChinaMobileBundle &request); //通过request初始化Session对象
    
    // 将当前session相关信息保存到redis中
    // redis key为_sessionId，value为成员变量构成的json序列化string
    bool Save(); //保持到redis数据库

    //从redis中获取当前session保存的信息，反序列化后赋值成员变量
    bool Load(const std::string& inter_idx); //从redis读取

    bool GetTelephoneNumber(string &telephoneNumber); // 获取当前手机号

    // 查询余额
    bool QueryBalance(string &balance);
    // 查询套餐余量
    bool QueryPackageInfo(Package_Info &package_info);
    //4G专网和超额流量费
    bool QueryFourGAndNetInfo(FourGAndNetInfo &fourGAndNetInfo);
    //判断是否停机
	bool IsOff(bool &flag);
    // 获取停机原因
    bool GetOffReason(string &reason);
	// 获取主资费套餐信息
    bool GetMaintarInfo(MaintarInfo &maintarInfo);
    // 获取已开业务查询_最低消费
    bool GetAlreadyInfo(string &strlowest);
	// 证件号码是否正确服务器端
    bool IsLegalCertificateInputServer(string input, bool &flag);
	// 是否实名制
    bool IsRealName(bool &flag);
	// 获取账单信息(上网费)
    bool GetBillMsgInfo(string &strNetFee);
	// 本卡资费短信发送
    bool MsseageOutService();
    // 短信下发
    bool SendMessage(string message);
    // 密码重置短信发送
    bool ResetTelephonePasswordSendSMS(bool &flag);
    // 密码输入是否匹配 服务器端
    bool IsMatchedPassword(string input, bool &flag);
    // 密码是否锁定
    bool IsPasswordLock(bool &flag);
    // 密码修改后台处理
    bool ModifyTelephonePassword(string oldPassword, string newPassword, string& flag); 
    // 获取系统时间
    bool GetSystemTime(tm &systemTime);
    // 当前时间段
    bool GetCurrentTimeRange(string &currentTimeRange);
    // 取年月（格式：201805）
    bool GetCurrentYearMonth(string &currentYearMonth);
    //判断是否是夜间服务时间段
    bool IsNight(bool &flag);
    // 证件号码是否正确
    bool IsValidIDNumber(string input, bool &flag);
    //是否简单密码
    bool IsSimplePassword(string input, bool &flag);
    // 密码输入是否合理
    bool IsValidPassword(string userPassword, bool &flag);
	
    // 是否运行发送满意度短信
    bool IsNeedSendSatisfactionSMS(bool &ret);
	
    //----------------------获取相关接入信息----------------------------
	bool GetCallDstId(string &callDstId); // 被叫号码
	bool GetFlow(string &flow);
	bool GetAuthToken(string &authToken); // 受权信息
	bool GetEntranceId(string &entranceId);
	bool GetStartTime(string &startTime);
	bool GetSesTime(string &sesTime);
	bool GetSuiluRegionId(string &suiluRegionId);
	bool GetCCId(string &ccId);
	bool GetRoleLabel(string &roleLabel);

    // 获取挂机原因
    bool GetCloseReason(string &userCloseReason);
    /**
    * @brief 设置挂机原因
    * @param [in] userCloseReason 只能输入以下参数作为挂机原因:
    * -# 1:主动转人工
    * -# 2:业务转人工
    * -# 3:交易转人工
    * -# 4:转IVR按键
    * -# 4001:转IVR首层
    * -# 4002:转XX业务按键
    * -# 5:三次错误转按键
    * -# 6:挂机
    * -# -2:未识别
    */
    bool SetCloseReason(const string &userCloseReason);

    //--------------供上层调用：办理类----------------------------
    bool SendSatisfactionMessage(string callId, string content); // 满意度短信下发
    
    //-----------------------获取flume需要的相关通话信息-------------------------
    bool GetBrandId(string &userBrandId);
    bool GetRegion(string &userRegion);
    bool GetTrustLevel(string &userTrustLevel);
    bool GetPayModeId(string &userPayModeId);
    
    bool SetIncommuInfo(const Json::Value &userIncommuLogInfo);
    bool GetIncommuInfo(Json::Value &userIncommuLogInfo);

private:
    //---------------------请求报文编写及应答报文解析------------------------------
    /**
    * @brief 用户信息查询
    * @return true 调用成功 false 调用失败
    */
    bool CallQryUserInfo();
    /**
    * @brief 发送满意度调查短信
    * @param[in] string callId 本通电话的通过ID
    * @param[in] string message 满意度调查收条短信的短信内容
    * @param[in] string channelId 通道ID，据文档约定填写"uvn_tps"
    * @return true 调用成功 false 调用失败
    */
    bool CallSendSatisfactionSMSInfo(string callId, string message, string channelId);

private:
    // --------------------------------通过Http与底层进行交互-----------------------------
    /**
    * @brief 将报文通过http的post方式发送给底层java服务，java服务调用csf接口
    * @param[in] string csfRequest 要发送的报文
    * @param[out] string csfResponse 底层返回的报文
    * @return true 调用成功 false 调用失败
    */
    bool CallCSF(string csfRequest, string& csfResponse);

    //-----------------------------------基础工具-----------------------------------------
    /**
    * @brief 从请求字段中提取flume日志需要的字段
    * @param[in] csfRequest   请求的Json格式报文
    * @param[out] interface_ecode   flume 接口原因编码 字段
    * @param[out] business_name   flume 业务名称 字段
    * @param[out] smsContent   flume 短信内容 字段
    * @return true 提取成功 false 提取失败
    */
    bool GetBusinessInfoFromCsfRequest(const string& csfRequest, string& business_name, string& smsContent);
    /**
    * @brief 从响应字段中提取flume日志需要的字段
    * @param[in] csfRequest   请求的Json格式报文
    * @param[in] csfResponse   响应的Json格式报文
    * @param[out] interface_ecode   flume 接口原因编码 字段
    * @return true 提取成功 false 提取失败
    */
    bool GetininterInfoFromCsfResponse(const string& csfRequest, const string& csfResponse, string& interface_ecode);

private:
    /**
     * @brief MockedChinaMobileSession.cc 用，无CSF环境情况下测试CRS代码
     *        预先设定好一些测试值，通过Redis数据来模拟业务流程的数据
     *        测试时利用python脚本，并通过python设定测试值，模拟业务流程的各个分支
     * @param[in] func    Redis的Key值，调用时与模拟业务流程所用的函数名同名
     * @param[out] ret    测试的函数需要返回bool型，由Redis 保存的值转化而来
     * @return 调用函数是出现异常 true 无异常 false 内部异常
     */
    bool GetBoolFromRedis(const std::string&func,bool&ret);
    /**
     * @brief MockedChinaMobileSession.cc 用，无CSF环境情况下测试CRS代码
     *        预先设定好一些测试值，通过Redis数据来模拟业务流程的数据
     *        测试时利用python脚本，并通过python设定测试值，模拟业务流程的各个分支
     * @param[in] func    Redis的Key值，调用时与模拟业务流程所用的函数名同名
     * @param[out] ret    Redis 保存的string
     * @return 调用函数是出现异常 true 无异常 false 内部异常
     */
    bool GetStringFromRedis(const std::string& func, std::string& ret);

private:
    string _sessionId; // 呼叫唯一标志
    string _telephoneNumber; //手机号码
    //ChinaMobileBundle information
    string _call_dst_id; // 被叫号码
	string _flow; // 当前未使用(初始化操作传入参数)
	string _auth_token; // 授权信息，取接入控制系统返回的token值
	string _entrance_id; // 导航入口，电话进入导航方式
	string _start_time; // 呼叫开始时间
	string _ses_time; // 当前未使用(初始化操作传入参数)
	string _suilu_region_id; // 呼入地
	string _cc_id; // 当前未使用(初始化操作传入参数)
	string _role_label; // 当前未使用(初始化操作传入参数)
    // flume 日志用的 incommu 信息
    Json::Value _incommu_info;
    //Session Status
    string _closeReason; // 挂机原因
    //flume 日志用的用户信息
    string _brandId; // 品牌编码
    string _region;  // 地市
    string _trustLevel; //用户星级
    string _payModeId; // 付费方式编码
    string _session_inter_idx;   // 交互序数 flume 日志用
    int _i_interface_idx;        // 接口序数 flume 日志用
};
} // namespace ChinaMobileProvince
#endif  // CHINA_MOBILE_SESSION_H_
