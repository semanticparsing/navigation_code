cc_library(
    name = "ConfigurationManager",
    srcs = [
        "ConfigurationManager.cc",
    ],
    deps = [
        #'#crypto',
        '//Common/Base:Base',
    ],
    link_all_symbols = True,
)

cc_binary(
    name = "Tester",
    srcs=[
        "ConfigurationManagerTester.cc",
    ],
    deps = [
        #'#crypto',
        #'//Common/Base:Base',
        '//Projects/ChinaMobile/ConfigurationManager:ConfigurationManager',
    ],
)
