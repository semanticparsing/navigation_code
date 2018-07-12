cc_library (
    name = "utils",
    srcs = [
        "Utils.cpp",
        "Config.cpp",
        "CRSRedisWrapper.cpp",
        #"RedisWrapper.cpp",
    ],
    deps = [
        '//Common/ThirdParty/hiredis:hiredis',
        "//Common/ThirdParty/jsoncpp-0.5.0/src/lib_json:jsoncpp",
        "//Projects/ChinaMobile/CRS/Src/Interface:ChinaMobileInterface",
    ],
)

cc_library (
    name = "raw_utils",
    srcs = [
        "Utils.cpp",
        "Config.cpp",
        "RedisWrapper.cpp",
    ],
    deps = [
        '//Common/ThirdParty/hiredis:hiredis',
        "//Common/ThirdParty/jsoncpp-0.5.0/src/lib_json:jsoncpp",
        "//Projects/ChinaMobile/CRS/Src/Interface:ChinaMobileInterface",
    ],
)
