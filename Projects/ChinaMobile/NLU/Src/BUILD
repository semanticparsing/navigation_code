cc_library(name = 'NLU_serving_handler',
    srcs = [
        'NLU_serving_handler.cc',
        ],
    deps = [
        '//Common/Util/Thread:thread',
        '//Common/Util/ThriftClientWrapper:backendConfigManager',
        '//Common/Util/FrameSemanticParser/QueryProcessor:CalcQueryProcessor',
        '//Robot/Public/Proto:serviceThrift',
        '//Robot/Public/QueryPreprocess:queryProcessManager',        
        '//Projects/ChinaMobile/Proto:ChinaMobile_thrift',
    ],
)

cc_binary(
    name = 'NLU_server',
    srcs = [
        'NLU_server.cc',
    ],
    deps = [
        ':NLU_serving_handler',
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

