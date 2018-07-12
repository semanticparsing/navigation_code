cc_library(name = 'queryUnderstandingServingHandler',
    srcs = [
        'query_understanding_serving_handler.cc',
        ],
    deps = [
        #':normQuery',
        #'//Common/Base:Base',
        '//Common/Util/Thread:thread',
        #'//Common/Util/Serialize:base64',
        #'//Common/ThirdParty/hiredis:hiredis',
        '//Common/Util/Encode:encodeUtil',
        '//Common/Util/ThriftClientWrapper:backendConfigManager',
        #'//Common/ThirdParty/jsoncpp-0.5.0/src/lib_json:jsoncpp',
        #'//Common/Util/JSON/Src:JSON',
        #'//Common/Util/ZKThriftManager:zkthriftManager',
        #'//Robot/FAQ/Src:faqReduxHandler',
        '//Robot/Public/Proto:serviceThrift',
        '//Robot/Public/QueryPreprocess:queryProcessManager',
    ],
)

cc_binary(
    name = 'query_understanding_server',
    srcs = [
        'query_understanding_server.cc',
    ],
    deps = [
        ':queryUnderstandingServingHandler',
        #'//Common/Base:Base',
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

