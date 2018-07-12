cc_library(name = 'puncOper',
    srcs = [
        'puncOper.cc'
    ],
    deps = [
        '#crypto',
        '//Common/Base:Base',
        '//Common/Util/File:simpleLineReader',
        '//Robot/Public/Proto:serviceThrift',
        '//Common/Util/Serialize:base64',
    ],
)

cc_library(name = 'pinyinConvert',
    srcs = [
        'pinyinConvert.cc'
    ],
    deps = [
        '#crypto',
        '//Common/Base:Base',
        '//Common/ThirdParty/gtest:gtest',
        '//Robot/Public/Proto:serviceThrift',
        '//Common/Util/Serialize:base64',
        '//Robot/Public/Utils:puncOper',
    ],
)

cc_library(name = 'singletonWordSegTag',
    srcs = [
        'singletonWordSegTag.cc'
    ],
    deps = [
        '#crypto',
        '//Common/Base:Base',
        '//Common/Base:baseutf8',
        '//NLP/WordSegTag/src:wordsegtag',
        '//Common/Util/File:simpleLineReader',
        '//Robot/Public/Proto:serviceThrift',
        '//Common/Util/Serialize:base64',
    ],
)

cc_library(name = 'singletonTran2Simp',
    srcs = [
        'singletonTran2Simp.cc'
    ],
    deps = [
        '#crypto',
        '//Common/Base:Base',
        '//Common/Util/File:simpleLineReader',
    ],
)

cc_library(name = 'debugHelper',
    srcs = [
        'debugHelper.cc'
    ],
    deps = [
        '#crypto',
        '//Common/Base:Base',
        '//Robot/Public/Proto:serviceThrift',
    ],
)

cc_library(name = 'trioDataBundleHelper',
    srcs = [
        'trioDataBundleHelper.cc'
    ],
    deps = [
        '#crypto',
        '//Common/Base:Base',
        '//Robot/Public/Proto:serviceThrift',
    ],
)

cc_library(name = 'rewriteQueryHelper',
    srcs = [
        'rewriteQueryHelper.cc'
    ],
    deps = [
        '#crypto',
        '//Common/Base:Base',
        '//Robot/Public/Proto:serviceThrift',
    ],
)

cc_library(name = 'trioFlags',
    srcs = [
        'trioFlags.cc'
    ],
    deps = [
        '//Common/Base:Base',
    ],
)

cc_library(name = 'searchResultConverter',
    srcs = [
        'searchResultConverter.cc',
    ],
    deps = [
        '//Common/Base:Base',
    ],
)

cc_library(name = "protoBufferLoader",
    srcs = [
        'protoBufferLoader.cc',
    ],
    deps = [
        '//Common/ThirdParty/file_monitor:fileMonitor',
    ],
)

