cc_library(
    name = "china_mobile_crs_serving_handler_redis",
    srcs = [
        "china_mobile_crs_serving_handler.cc",
    ],
    defs = [ 'PERSISTENCE_REDIS'],
    deps = [
        #'//Common/Base:Base',
        #'//Common/Util/Thread:thread',
        '//Projects/ChinaMobile/Proto:ChinaMobile_thrift',
        '//Robot/Public/Proto:serviceThrift',
        '//Common/Util/ThriftClientWrapper:backendConfigManager',
        '//Robot/TCD/FrameTaskDman:frame_task_dman',
        '//Projects/ChinaMobile/CRS/Src/Session:ChinaMobileSession',
        '//Projects/ChinaMobile/CRS/Src/CRSLogger:ChinaMobileCRSLogger',
        #'//Common/Util/Thread:thread',
    ],
    link_all_symbols = True,
)

cc_binary(
    name = 'china_mobile_crs_server',
    srcs = [
        'china_mobile_crs_server.cc',
    ],
    deps = [
        ':china_mobile_crs_serving_handler_redis',
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
