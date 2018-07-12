#ifndef CRSBrain_SERVING_HANDLER_H_
#define CRSBrain_SERVING_HANDLER_H_

//#include "Projects/ChinaMobile/Proto/ChinaMobileCRSServing.h"
#include "Projects/ChinaMobile/Proto/CRSBrainServing.h"
#include "Projects/ChinaMobile/Proto/NLUServing.h"
#include "Projects/ChinaMobile/Proto/ChinaMobileServiceHead.h"
//#include "Projects/ChinaMobile/CRS/Src/Interface/HttpOperator.h"
//#include "Projects/ChinaMobile/CRS/Src/Interface/OperateLogInterface.h"
//#include "Projects/ChinaMobile/CRS/Src/Interface/OperateRedisInterface.h"

//#include <map>
//#include <string>

//#include "Common/Base/trio.h"
#include "Common/Util/Thread/threadPool.h"

using namespace robot;
namespace chinaMobile {

class CRSBrainServingHandler: public CRSBrainServingIf {
public:
    CRSBrainServingHandler() {
        //caller_pool_.reset(NULL);
    }
    virtual ~CRSBrainServingHandler() {
        ;
    }

    bool Init();
    virtual void CRSBrainProcess( ChinaMobileBundle &response, const ChinaMobileBundle &request);

private:
    DEFINE_CALL_CHINAMOBILE_SERVICE(NLU);
    DEFINE_CALL_CHINAMOBILE_SERVICE(ChinaMobileCRS);
    void PrintRequest(const ChinaMobileBundle &request);
    void PrintResponse(const ChinaMobileBundle &response);

private:
    //scoped_ptr<ThreadPool> caller_pool_;
    scoped_ptr<ThriftClientManager<NLUServingClient> > nlu_client_mgr_;
    scoped_ptr<ThriftClientManager<ChinaMobileCRSServingClient> > chinaMobileCRS_client_mgr_;
};

} // namespace robot
#endif  // CRSBrain_SERVING_HANDLER_H_
