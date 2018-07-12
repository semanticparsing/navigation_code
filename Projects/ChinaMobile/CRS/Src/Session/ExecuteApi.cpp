#include "ExecuteApi.h"

namespace TrioTDM {

#define CHECK_CHINA_SESSION_IS_NULL do{ if (_china_mobile_session == NULL) { VLOG(FATAL) << __FUNCTION__ << "\t_china_mobile_session is NULL."; return false;} } while(0)
#define CHECK_DATA_TYPE(data, type) do{ if (data.GetConceptType() != type) { VLOG(FATAL) << __FUNCTION__ << "\t"#data << " is not "#type << ", Type: " << data.GetConceptType(); return false;} } while(0)

CExecuteApi::CExecuteApi() :
    _china_mobile_session(NULL) {
    ;
}

CExecuteApi::CExecuteApi(ChinaMobileProvince::ChinaMobileSession* china_mobile_session) :
    _china_mobile_session( china_mobile_session ) {
}

CExecuteApi::~CExecuteApi() {
    ;
}

void CExecuteApi::SetChinaMobileSession(ChinaMobileProvince::ChinaMobileSession* mobile_session) {
    _china_mobile_session = mobile_session;
}

bool CExecuteApi::SendMessage(CConcept& param) {
    CHECK_CHINA_SESSION_IS_NULL;
    CHECK_DATA_TYPE(param, ctString);
    
    VLOG(DEBUG) << __FUNCTION__ << ", SMS: " << (std::string)param;
    
    return _china_mobile_session->SendMessage((std::string)param);
}

}
