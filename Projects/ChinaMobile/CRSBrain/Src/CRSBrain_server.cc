#include "CRSBrain_serving_handler.h"
#include "Common/Base/Internal/versionPrint.h"
#include "Common/ThirdParty/thrift/include/concurrency/PosixThreadFactory.h"
#include "Common/ThirdParty/thrift/include/concurrency/ThreadManager.h"
#include "Common/ThirdParty/thrift/include/protocol/TBinaryProtocol.h"
#include "Common/ThirdParty/thrift/include/server/TNonblockingServer.h"
#include "Common/ThirdParty/thrift/include/Thrift.h"
#include "Common/Base/globalInit.h"

DEFINE_int32(port, 20916, "serving port");
DEFINE_int32(threads, 24, "serving threads");

using apache::thrift::TProcessor;
using apache::thrift::protocol::TProtocolFactory;
using apache::thrift::protocol::TBinaryProtocolFactory;
using apache::thrift::concurrency::ThreadManager;
using apache::thrift::concurrency::PosixThreadFactory;
using apache::thrift::server::TNonblockingServer;

int main(int argc, char** argv) {
    InitFlagsAndVersion(&argc, &argv);
    global_init::InitGlog(argv[0]);

    uint16 port = FLAGS_port;
    uint16 threads = FLAGS_threads;

    boost::shared_ptr<chinaMobile::CRSBrainServingHandler> handler(new chinaMobile::CRSBrainServingHandler());
    if ( handler.get() == NULL ) {
        return -1;
    }
    if ( !handler->Init() ) {
        return -1;
    }

    boost::shared_ptr<TProcessor> processor( new chinaMobile::CRSBrainServingProcessor(handler) );
    boost::shared_ptr<TProtocolFactory> protocol_factory( new TBinaryProtocolFactory() );

    boost::shared_ptr<ThreadManager> thread_manager = ThreadManager::newSimpleThreadManager( threads );
    boost::shared_ptr<PosixThreadFactory> thread_factory( new PosixThreadFactory() );
    thread_manager->threadFactory( thread_factory );
    thread_manager->start();

    TNonblockingServer server(processor, protocol_factory, port, thread_manager);

    server.serve();

    return 0;
}
