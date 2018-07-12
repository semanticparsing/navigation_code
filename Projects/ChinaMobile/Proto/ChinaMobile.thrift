namespace cpp chinaMobile
namespace java chinaMobile
namespace py chinaMobile.service

include "ChinaMobileBundle.thrift"

service ChinaMobileCRSServing {
    ChinaMobileBundle.ChinaMobileBundle ChinaMobileCRSProcess(1: ChinaMobileBundle.ChinaMobileBundle bundle);
}

service NLUServing {
    ChinaMobileBundle.ChinaMobileBundle NLUProcess(1: ChinaMobileBundle.ChinaMobileBundle bundle);
}

service CRSBrainServing {
    ChinaMobileBundle.ChinaMobileBundle CRSBrainProcess(1: ChinaMobileBundle.ChinaMobileBundle bundle);
}
