// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-14 20:39
// Description: 

#ifndef _EXECUTE_API_H__
#define _EXECUTE_API_H__

#include "Robot/TCD/FrameTaskDman/DMCore/Concepts/AllConcepts.h"
#include "ChinaMobileSession.h"

namespace TrioTDM {

class CExecuteApi {

public:
    CExecuteApi();
    CExecuteApi(ChinaMobileProvince::ChinaMobileSession* china_mobile_session);
    ~CExecuteApi();
    
    void SetChinaMobileSession(ChinaMobileProvince::ChinaMobileSession* mobile_session);

public:
    bool SendMessage(CConcept& param);
private:
	ChinaMobileProvince::ChinaMobileSession* _china_mobile_session;
};

}

#endif
