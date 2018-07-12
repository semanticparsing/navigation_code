// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-14 11:19
// Description: 

#ifndef _GAIN_CONCEPT_API_H__
#define _GAIN_CONCEPT_API_H__

#include "Robot/TCD/FrameTaskDman/DMCore/Concepts/AllConcepts.h"
#include "ChinaMobileSession.h"

namespace TrioTDM {

class CGainConceptApi {

public:
    CGainConceptApi();
    CGainConceptApi(ChinaMobileProvince::ChinaMobileSession* china_mobile_session);
    virtual ~CGainConceptApi();

    void SetChinaMobileSession(ChinaMobileProvince::ChinaMobileSession* mobile_session);

public:
    // 是否停机
    bool IsOff(CConcept &ret);
    // 获取停机原因
    bool GetOutOfServiceReason(CConcept& ret);
    // 是否为夜间服务时间
    bool IsNight(CConcept &ret);
    bool GetBalance(CConcept &ret);
    bool GetCurrentTimeRange(CConcept &ret);

    // 获取套餐外流量费
    bool GetExcessTrafficFee(CConcept &ret);
    // 获取4G专网信息
    bool GetFourGExclusiveNetResult(CConcept &ret);
    // 获取主资费套餐信息
    bool GetMaintarInfo(CConcept &ret);
    // 获取主资费套餐
    bool GetMaintarInfo_ProdPrcName(CConcept &ret);
    // 获取月租套餐
    bool GetMaintarInfo_MonthlyRentFee(CConcept &ret);
    // 获取固定业务
    bool GetMaintarInfo_FixedFee(CConcept &ret);
    // 获取最低消费信息
    bool GetAlreadyInfo_Lowest(CConcept &ret);
	// 剩余语音通话分钟数
    bool GainRemainingTalkMinutes(CConcept &ret);
    // 剩余流量
    bool GainRemainingTrafficPackage(CConcept &ret);
    // 剩余流量
    bool GainRemainingTrafficOppugn(CConcept &ret);
    // 已开业务查询
    bool GainOpenedBusinessList(CConcept &ret);
    // 本卡资费短信下发
    bool MsseageOutService(CConcept &ret);
    // 已开业务短信下发
    bool SendAlreadyInfoSMS(CConcept &ret);
    
    // 是否实名制
    bool IsRealName(CConcept &ret);
    // 证件号码是否正确
    bool IsLegalCertificateInput(CConcept& input, CConcept& ret);
    // 证件号码是否正确服务器端
    bool IsLegalCertificateInputServer(CConcept& input, CConcept& ret);
    // 密码重置短信发送
    bool ResetTelephonePassword(CConcept &ret);
    // 密码修改后台处理
    bool ModifyTelephonePassword(CConcept &passwordInfo, CConcept &ret);
    // 密码是否锁定
    bool IsPasswordLock(CConcept &ret);
    // 密码输入是否合理
    bool IsLegalPasswordInput(CConcept& input, CConcept& ret);
    // 密码输入是否匹配 服务器端
    bool IsMatchedPassword(CConcept& input, CConcept& ret);
    //是否回主菜单，*键
    bool IsInputReturnPrimaryMenu(CConcept& input, CConcept& ret);
    //是否简单密码
    bool IsSimplePassword(CConcept& input, CConcept& ret);

private:

    ChinaMobileProvince::ChinaMobileSession* _china_mobile_session;
};

}

#endif
