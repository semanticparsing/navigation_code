cc_library(
    name = 'query_parser_serving_handler',
    srcs = [
        'query_parser_serving_handler.cc',
    ],
    deps = [
        '//Robot/Public/Proto:serviceThrift',
        '//Robot/TCD/FrameSemanticParser_2_0/grammarParser:FrameSemanticParser2',
    ],
    link_all_symbols = True,
)

cc_binary(
    name = 'query_parser_server',
    srcs = [
        'query_parser_server.cc',
    ],
    deps = [
        ':query_parser_serving_handler',
        '//Common/ThirdParty/thrift-1.0.0-dev:thriftnb',
        '//Common/ThirdParty/thrift-1.0.0-dev:thrift_c_glib',
        '//Common/ThirdParty/thrift-1.0.0-dev:thriftz',
        '//Common/ThirdParty/thrift-1.0.0-dev:thrift',
        '//Common/ThirdParty/libevent-2.0.11-stable:event',
    ],
    optimize = ['O3'],
    #extra_cppflags = ['-fno-omit-frame-pointer', '-Wl,--eh-frame-hdr'],
    #extra_linkflags = [
        #'-lcurl',
        #'-LCommon/ThirdParty/curl-7.53.1/lib64_release',
    #],
)
