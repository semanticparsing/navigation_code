cc_library(name = 'CRSBrainServingHandler',
    srcs = [
        'CRSBrain_serving_handler.cc',
        ],
    deps = [
        '//Common/Util/Thread:thread',
        '//Common/Util/ThriftClientWrapper:backendConfigManager',
        '//Robot/Public/Proto:serviceThrift',
        '//Projects/ChinaMobile/CRS/Src/Interface:ChinaMobileInterface',
        '//Projects/ChinaMobile/Proto:ChinaMobile_thrift',
    ],
)

cc_binary(
    name = 'CRS_brain_server',
    srcs = [
        'CRSBrain_server.cc',
    ],
    deps = [
        ':CRSBrainServingHandler',
        '//Common/ThirdParty/thrift-1.0.0-dev:thriftnb',
        '//Common/ThirdParty/thrift-1.0.0-dev:thrift_c_glib',
        '//Common/ThirdParty/thrift-1.0.0-dev:thriftz',
        '//Common/ThirdParty/thrift-1.0.0-dev:thrift',
        '//Common/ThirdParty/libevent-2.0.11-stable:event',
    ],
    extra_linkflags = [
        '-ltensorflow_nn',
        '-LCommon/ThirdParty/tensorflow/lib64_release',
    ],

)

