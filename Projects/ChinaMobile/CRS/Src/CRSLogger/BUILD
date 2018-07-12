cc_library(
    name = "ChinaMobileCRSLogger",
    srcs = [
        "CSFInterfaceLogger.cc",
    ],
    optimize =[
        'std=c++0x',
    ],
    deps = [
        #'#crypto',
        '//Common/Base:Base',
        '//Common/ThirdParty/jsoncpp-0.5.0/src/lib_json:jsoncpp',
        '//Projects/ChinaMobile/CRS/Src/Interface:ChinaMobileInterface',
    ],
    link_all_symbols = True,
)

