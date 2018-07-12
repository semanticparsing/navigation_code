cc_library(
    name = 'queryRewriter',
    srcs = [	
		'autoCompleteProcessor.cc',
		'sentenceProcessor.cc',
		'segmentProcessor.cc',
		'commonCharRewriter.cc',
		'pinyinRewriter.cc',
		'duplicateRemoveRewriter.cc',
		'emotionRemoveRewriter.cc',
		'nicknameRemoveRewriter.cc',
		'truncateRewriter.cc',
		'puncRemoveRewriter.cc',
		'synonymRewriter.cc',
		'queryRewriter.cc',
        ],
    deps = [
        '#crypto',
        '//Common/Base:Base',
        '//Common/Util/Registerer:registerer',
        '//Common/Util/File:simpleLineReader',
        '//Common/Util/Thread:thread',
        '//Common/Util/Trie:jumpTrie',
        '//Common/Util/Encode:encodeUtil',
        '//Robot/Public/Utils:singletonWordSegTag',
        '//Robot/Public/Utils:singletonTran2Simp',
        '//Robot/Public/Declare:declearHead',
        '//Common/ThirdParty/re2:re2',
        '//Robot/Public/UserContext:userContext',
        '//Robot/Public/Utils:pinyinConvert',
        '//Common/Util/ZKThriftManager:zkthriftManager'
        ],
    optimize = ['-std=c++11',],
    link_all_symbols = True,
    )

cc_test(
    name = 'queryRewriterUnittest',
    srcs = ['queryRewriterUnittest.cc'],
    deps = [
    '//Common/Base:Base',
    '//Common/Util/File:simpleLineReader',
    ':queryRewriter',
    ],
    testdata = ['test/data'],
    dynamic_link = False,
)
