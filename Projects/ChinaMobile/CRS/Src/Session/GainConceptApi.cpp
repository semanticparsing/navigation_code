#include "GainConceptApi.h"
#include "Common/Base/stringUtil.h"

using namespace ChinaMobileProvince;

namespace TrioTDM {

#define CHECK_CHINA_SESSION_IS_NULL do{ if (_china_mobile_session == NULL) { VLOG(FATAL) << __FUNCTION__ << "\t_china_mobile_session is NULL."; return false;} } while(0)
#define CHECK_DATA_TYPE(data, type) do{ if (data.GetConceptType() != type) { VLOG(FATAL) << __FUNCTION__ << "\t"#data << " is not "#type << ", Type: " << data.GetConceptType(); return false;} } while(0)

CGainConceptApi::CGainConceptApi()
: _china_mobile_session(NULL) 
{}

CGainConceptApi::CGainConceptApi(ChinaMobileProvince::ChinaMobileSession* china_mobile_session)
: _china_mobile_session( china_mobile_session )
{}

CGainConceptApi::~CGainConceptApi() {}

void CGainConceptApi::SetChinaMobileSession(ChinaMobileProvince::ChinaMobileSession* mobile_session)
{
    _china_mobile_session = mobile_session;
}

bool CGainConceptApi::IsOff(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctBool);
    bool valueTmp = true;
    if ( !_china_mobile_session->IsOff(valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call IsOff fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

bool CGainConceptApi::GetOutOfServiceReason(CConcept& ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctString);

    std::string reason;
    if ( !_china_mobile_session->GetOffReason(reason) )
    {
        VLOG(FATAL) << __FUNCTION__ << ", call GetOffReason fail.";
        return false;
    }
    ret = reason;
    return true;
}

bool CGainConceptApi::IsNight(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctBool);
    bool valueTmp = true;
    if ( !_china_mobile_session->IsNight(valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call IsNight fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

bool CGainConceptApi::GetBalance(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["remain_fee"], ctString);

    string valueTmp = "0";
    if ( !_china_mobile_session->QueryBalance(valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call QueryBalance fail.";
        return false;
    }
    ret["remain_fee"] = valueTmp;
    return true;
}

bool CGainConceptApi::GetCurrentTimeRange(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctString);

    string valueTmp = "0";
    if ( !_china_mobile_session->GetCurrentTimeRange(valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call GetCurrentTimeRange fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 获取主资费套餐
bool CGainConceptApi::GetMaintarInfo_ProdPrcName(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["prod_prc_name"], ctString);
    MaintarInfo maintarInfo;
    if ( !_china_mobile_session->GetMaintarInfo(maintarInfo) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call GetMaintarInfo fail.";
        return false;
    }
    ret["prod_prc_name"] = maintarInfo.prod_prc_name;
    return true;
}

bool CGainConceptApi::SendAlreadyInfoSMS(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctBool);
    string lowest_spend = "";
    if ( !_china_mobile_session->GetAlreadyInfo(lowest_spend) )
    {
        ret = false;
        VLOG(FATAL) << __FUNCTION__ << "\t call GetAlreadyInfo fail.";
        return false;
    }
    ret = true;
    return true;
}

bool CGainConceptApi::MsseageOutService(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctBool);
    if ( !_china_mobile_session->MsseageOutService() )
    {
        ret = false;
        VLOG(FATAL) << __FUNCTION__ << "\t call MsseageOutService fail.";
        return false;
    }
    ret = true;
    return true;
}

// 剩余语音通话分钟数
bool CGainConceptApi::GainRemainingTalkMinutes(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["TALK_TOTAL"], ctString);
    CHECK_DATA_TYPE(ret["TALK_FREE"], ctString);
    CHECK_DATA_TYPE(ret["TALK_USED"], ctString);
    CHECK_DATA_TYPE(ret["FAMILY_TALK_TOTAL"], ctString);
    CHECK_DATA_TYPE(ret["FAMILY_TALK_FREE"], ctString);
    CHECK_DATA_TYPE(ret["FAMILY_TALK_USED"], ctString);

    Package_Info cPackage_info;
    if ( !_china_mobile_session->QueryPackageInfo(cPackage_info) )
    {
        VLOG(FATAL) << __FUNCTION__ << ", call QueryPackageInfo error.";
        return false;
    }
    
    ret["is_exist_record"] = cPackage_info.TALK.flag;
    if (cPackage_info.TALK.flag)
    {
        ret["TALK_TOTAL"] = cPackage_info.TALK.TALK_TOTAL;
        ret["TALK_FREE"] = cPackage_info.TALK.TALK_FREE;
        ret["TALK_USED"] = cPackage_info.TALK.TALK_USED;
    }
    if(cPackage_info.FAMILY_TALK.flag)
    {
        ret["FAMILY_TALK_TOTAL"] = cPackage_info.FAMILY_TALK.FAMILY_TALK_TOTAL;
        ret["FAMILY_TALK_FREE"] = cPackage_info.FAMILY_TALK.FAMILY_TALK_FREE;
        ret["FAMILY_TALK_USED"] = cPackage_info.FAMILY_TALK.FAMILY_TALK_USED;
    }
    return true;
}

// 剩余流量-判断是否有套餐
bool CGainConceptApi::GainRemainingTrafficPackage(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["is_exist_record"], ctBool);
    CHECK_DATA_TYPE(ret["GPRS_TOTAL"], ctString);
    CHECK_DATA_TYPE(ret["GPRS_FREE"], ctString); 
    CHECK_DATA_TYPE(ret["GPRS_USED"], ctString);
    CHECK_DATA_TYPE(ret["AREA_GPRS_TOTAL"], ctString);
    CHECK_DATA_TYPE(ret["AREA_GPRS_FREE"], ctString);
    CHECK_DATA_TYPE(ret["AREA_GPRS_USED"], ctString);

    Package_Info cPackage_info;
    if ( !_china_mobile_session->QueryPackageInfo(cPackage_info) )
    {
        VLOG(FATAL) << __FUNCTION__ << ", call QueryPackageInfo error.";
        return false;
    }
    
    if (cPackage_info.GPRS.flag || cPackage_info.AREA_GPRS.flag)
    {
        ret["is_exist_record"] = true;
    }
    else
    {
        ret["is_exist_record"] = false;
    }
    
    if (cPackage_info.GPRS.flag)
    {
        ret["GPRS_TOTAL"] = cPackage_info.GPRS.GPRS_TOTAL;
        ret["GPRS_FREE"] = cPackage_info.GPRS.GPRS_FREE;
        ret["GPRS_USED"] = cPackage_info.GPRS.GPRS_USED;
    }
    
    if (cPackage_info.AREA_GPRS.flag)
    {
        ret["AREA_GPRS_TOTAL"] = cPackage_info.AREA_GPRS.AREA_GPRS_TOTAL;
        ret["AREA_GPRS_FREE"] = cPackage_info.AREA_GPRS.AREA_GPRS_FREE;
        ret["AREA_GPRS_USED"] = cPackage_info.AREA_GPRS.AREA_GPRS_USED;
    }
    return true;
}

//jl010已开业务-质疑类 是否剩余流量
bool CGainConceptApi::GainRemainingTrafficOppugn(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["is_exist_record"], ctBool);
    CHECK_DATA_TYPE(ret["GPRS_TOTAL"], ctString);
    CHECK_DATA_TYPE(ret["GPRS_FREE"], ctString);
    CHECK_DATA_TYPE(ret["GPRS_USED"], ctString);
    CHECK_DATA_TYPE(ret["AREA_GPRS_TOTAL"], ctString);
    CHECK_DATA_TYPE(ret["AREA_GPRS_FREE"], ctString);
    CHECK_DATA_TYPE(ret["AREA_GPRS_USED"], ctString);

    Package_Info cPackage_info;
    if ( !_china_mobile_session->QueryPackageInfo(cPackage_info) )
    {
        VLOG(FATAL) << __FUNCTION__ << ", call QueryPackageInfo error.";
        return false;
    }

    ret["is_exist_record"] = false;

    if (cPackage_info.GPRS.flag)
    {
        ret["GPRS_TOTAL"] = cPackage_info.GPRS.GPRS_TOTAL;
        ret["GPRS_FREE"] = cPackage_info.GPRS.GPRS_FREE;
        ret["GPRS_USED"] = cPackage_info.GPRS.GPRS_USED;
        if (cPackage_info.GPRS.GPRS_FREE != "" && cPackage_info.GPRS.GPRS_FREE != "0MB")
        {
            ret["is_exist_record"] = true;
        }
    }

    if (cPackage_info.AREA_GPRS.flag)
    {
        ret["AREA_GPRS_TOTAL"] = cPackage_info.AREA_GPRS.AREA_GPRS_TOTAL;
        ret["AREA_GPRS_FREE"] = cPackage_info.AREA_GPRS.AREA_GPRS_FREE;
        ret["AREA_GPRS_USED"] = cPackage_info.AREA_GPRS.AREA_GPRS_USED;
        if (cPackage_info.AREA_GPRS.AREA_GPRS_FREE != "" && cPackage_info.AREA_GPRS.AREA_GPRS_FREE != "0MB")
        {
            ret["is_exist_record"] = true;
        }
    }
    return true;
}

// 已开业务查询
bool CGainConceptApi::GainOpenedBusinessList(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["PROD_PRC_NAME"], ctString);
    //CConcept* item = ((CArrayConcept&)ret["PROD_PRC_NAMEarry1"]).CreateElement();
    scoped_ptr<CConcept> item ( ((CArrayConcept&)ret["PROD_PRC_NAMEarry1"]).CreateElement() );
    CHECK_DATA_TYPE((*item)["PROD_NAME"], ctString);

    MaintarInfo maintarInfo;
    if ( !_china_mobile_session->GetMaintarInfo(maintarInfo) )
    {
        VLOG(FATAL) << __FUNCTION__ << ", call GetBusinessInfo_t error.";
        return false;
    }
    ret["PROD_PRC_NAME"] = maintarInfo.prod_prc_name;
    if (maintarInfo.fix_fee_list.size() > 0)
    {
        for(int i = 0; i < maintarInfo.fix_fee_list.size(); i++)
        {
            (*item)["PROD_NAME"] = maintarInfo.fix_fee_list[i];
            ((CArrayConcept&)ret["PROD_PRC_NAMEarry1"]).InsertAt(ret["PROD_PRC_NAMEarry1"].GetSize(), *item);
        }
    }
    
    return true;
}

// 是否实名制
bool CGainConceptApi::IsRealName(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctBool);
    bool valueTmp = true;
    if ( !_china_mobile_session->IsRealName(valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call IsRealName fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 证件号码是否正确
bool CGainConceptApi::IsLegalCertificateInput(CConcept& input, CConcept& ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(input, ctString);
    CHECK_DATA_TYPE(ret, ctBool);

    bool valueTmp = true;
    if ( !_china_mobile_session->IsValidIDNumber((std::string)input, valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call IsValidIDNumber fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 证件号码是否正确服务器端
bool CGainConceptApi::IsLegalCertificateInputServer(CConcept& input, CConcept& ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(input, ctString);
    CHECK_DATA_TYPE(ret, ctBool);

    bool valueTmp = true;
    if ( !_china_mobile_session->IsLegalCertificateInputServer((std::string)input, valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call IsLegalCertificateInputServer fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 密码重置短信发送
bool CGainConceptApi::ResetTelephonePassword(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctBool);
    bool valueTmp = true;
    if ( !_china_mobile_session->ResetTelephonePasswordSendSMS(valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call ResetTelephonePasswordSendSMS fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 密码修改后台处理
bool CGainConceptApi::ModifyTelephonePassword(CConcept &passwordInfo, CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(passwordInfo, ctFrame);
    CHECK_DATA_TYPE(passwordInfo["old_password"], ctString);
    CHECK_DATA_TYPE(passwordInfo["new_password"], ctString);
    CHECK_DATA_TYPE(ret, ctString);
    std::string valueTmp = "";
    if ( !_china_mobile_session->ModifyTelephonePassword((std::string)passwordInfo["old_password"], (std::string)passwordInfo["new_password"], valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call ModifyPhonePassword fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 密码是否锁定
bool CGainConceptApi::IsPasswordLock(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctBool);
    bool valueTmp = true;
    if ( !_china_mobile_session->IsPasswordLock(valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call IsPasswordLock fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 密码输入是否合理
bool CGainConceptApi::IsLegalPasswordInput(CConcept& input, CConcept& ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(input, ctString);
    CHECK_DATA_TYPE(ret, ctBool);

    bool valueTmp = false;
    if ( !_china_mobile_session->IsValidPassword((std::string)input, valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call IsValidPassword fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 密码输入是否匹配服务器端
bool CGainConceptApi::IsMatchedPassword(CConcept& input, CConcept& ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(input, ctString);
    CHECK_DATA_TYPE(ret, ctBool);

    bool valueTmp = false;
    if ( !_china_mobile_session->IsMatchedPassword((std::string)input, valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call IsMatchedPassword fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

//是否回主菜单，*键
bool CGainConceptApi::IsInputReturnPrimaryMenu(CConcept& input, CConcept& ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(input, ctString);
    CHECK_DATA_TYPE(ret, ctBool);
    
    ret = (std::string)input == "*";
    VLOG(DEBUG) << __FUNCTION__ << ", ret.IsUpdated: " << 
        ret.IsUpdated() << ", ret: " << (bool)ret << ", name: " << ret.GetName();
    
    return true;
}

//是否简单密码
bool CGainConceptApi::IsSimplePassword(CConcept& input, CConcept& ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(input, ctString);
    CHECK_DATA_TYPE(ret, ctBool);
    
    bool valueTmp = true;
    if ( !_china_mobile_session->IsSimplePassword((std::string)input, valueTmp) )
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call IsSimplePassword fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 获取套餐外流量费
bool CGainConceptApi::GetExcessTrafficFee(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret, ctString);
    string valueTmp = "";
    if (!_china_mobile_session->GetBillMsgInfo(valueTmp))
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call GetBillMsgInfo fail.";
        return false;
    }
    ret = valueTmp;
    return true;
}

// 获取4G专网信息
bool CGainConceptApi::GetFourGExclusiveNetResult(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["net_fee"], ctString);
    CHECK_DATA_TYPE(ret["four_g_traffic"], ctString);
    CHECK_DATA_TYPE(ret["area_traffic"], ctString);
    FourGAndNetInfo fourGAndNetInfo;
    if (!_china_mobile_session->QueryFourGAndNetInfo(fourGAndNetInfo))
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call QueryFourGAndNetInfo fail.";
        return false;
    }
    ret["net_fee"] = fourGAndNetInfo.NETFEE;
    ret["four_g_traffic"] = fourGAndNetInfo.FourG_FREE;
    ret["area_traffic"] = fourGAndNetInfo.AREA_GPRS_FREE;
    return true;
}

// 获取月租套餐
bool CGainConceptApi::GetMaintarInfo_MonthlyRentFee(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["monthly_rent_fee"], ctString);
    MaintarInfo maintarInfo;
    if (!_china_mobile_session->GetMaintarInfo(maintarInfo))
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call GetMaintarInfo fail.";
        return false;
    }
    ret["monthly_rent_fee"] = maintarInfo.monthly_rent_fee;
    return true;
}

//获取固定业务
bool CGainConceptApi::GetMaintarInfo_FixedFee(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["has_fixed_fee_business"], ctBool);
    CHECK_DATA_TYPE(ret["business_list"], ctArray);
    //CConcept* item = ((CArrayConcept&)ret["business_list"]).CreateElement();
    scoped_ptr<CConcept> item ( ((CArrayConcept&)ret["business_list"]).CreateElement() );
    CHECK_DATA_TYPE((*item)["prod_prc_name_array"], ctString);
    MaintarInfo maintarInfo;
    if (!_china_mobile_session->GetMaintarInfo(maintarInfo))
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call GetMaintarInfo fail.";
        return false;
    }
    ret["has_fixed_fee_business"] = (maintarInfo.fix_fee_list.size() != 0);
    for (auto iter = maintarInfo.fix_fee_list.begin(); iter != maintarInfo.fix_fee_list.end(); ++iter)
    {
        (*item)["prod_prc_name_array"] = *iter;
        ((CArrayConcept&)ret["business_list"]).InsertAt(ret["business_list"].GetSize(), *item);
    }
    return true;
}

// 获取最低消费信息
bool CGainConceptApi::GetAlreadyInfo_Lowest(CConcept &ret)
{
    VLOG(DEBUG) << __FUNCTION__ << ", call..";
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(ret["lowest_spend"], ctString);
    string lowest_spend = "";
    if (!_china_mobile_session->GetAlreadyInfo(lowest_spend))
    {
        VLOG(FATAL) << __FUNCTION__ << "\t call GetAlreadyInfo fail.";
        return false;
    }
    ret["lowest_spend"] = lowest_spend;
    return true;
}

}
