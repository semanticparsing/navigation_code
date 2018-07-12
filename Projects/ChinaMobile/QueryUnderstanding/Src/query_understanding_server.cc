#include "query_understanding_serving_handler.h"

//#include "Common/Base/flags.h"
#include "Common/Base/Internal/versionPrint.h"
#include "Common/ThirdParty/thrift/include/concurrency/PosixThreadFactory.h"
#include "Common/ThirdParty/thrift/include/concurrency/ThreadManager.h"
#include "Common/ThirdParty/thrift/include/protocol/TBinaryProtocol.h"
#include "Common/ThirdParty/thrift/include/server/TNonblockingServer.h"
#include "Common/ThirdParty/thrift/include/Thrift.h"

DEFINE_int32(port, 10818, "serving port");
DEFINE_int32(threads, 24, "serving threads");

using apache::thrift::TProcessor;
using apache::thrift::protocol::TProtocolFactory;
using apache::thrift::protocol::TBinaryProtocolFactory;
using apache::thrift::concurrency::ThreadManager;
using apache::thrift::concurrency::PosixThreadFactory;
using apache::thrift::server::TNonblockingServer;

int main(int argc, char** argv) {
    InitFlagsAndVersion(&argc, &argv);

    uint16 port = FLAGS_port;
    uint16 threads = FLAGS_threads;

    boost::shared_ptr<robot::QueryUnderstandingServingHandler> handler(new robot::QueryUnderstandingServingHandler());
    if ( handler.get() == NULL ) {
        return -1;
    }
    if ( !handler->Init() ) {
        return -1;
    }

    boost::shared_ptr<TProcessor> processor( new robot::QueryUnderstandingServingProcessor(handler) );
    boost::shared_ptr<TProtocolFactory> protocol_factory( new TBinaryProtocolFactory() );

    boost::shared_ptr<ThreadManager> thread_manager = ThreadManager::newSimpleThreadManager( threads );
    boost::shared_ptr<PosixThreadFactory> thread_factory( new PosixThreadFactory() );
    thread_manager->threadFactory( thread_factory );
    thread_manager->start();

    TNonblockingServer server(processor, protocol_factory, port, thread_manager);

    server.serve();

    return 0;
}
