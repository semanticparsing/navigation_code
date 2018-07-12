cc_library(
    name = "ChinaMobileInterface",
    srcs = [
        "HttpOperator.cc",
        "OperateRedisInterface.cc",
        "OperateLogInterface.cc",
    ],
    optimize =[
        'std=c++0x',
    ],
    deps = [
        #'//Common/Base:Base',
        #'//Common/ThirdParty/hiredis:hiredis',
        '//Common/ThirdParty/jsoncpp-0.5.0/src/lib_json:jsoncpp',
        '//Common/Util/HttpClient:curlWrapper',
    ],
    link_all_symbols = True,
)
