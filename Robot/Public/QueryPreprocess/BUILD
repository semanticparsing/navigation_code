cc_library(
    name = 'removeStopWordHandler',
    srcs = ['removeStopWordHandler.cc',
            ],
    deps = [
        '#crypto',
        '//Common/Base:Base',
        '//Common/Util/Registerer:registerer',
        '//Common/Util/File:simpleLineReader',
        '//Common/Util/Trie:jumpTrie',
        '//Common/Util/Encode:encodeUtil',
        '//Common/ThirdParty/segmenter/SegWrapper:segWrapper',
        '//Common/ThirdParty/segmenter:segment',
        '//Common/ThirdParty/segmenter:odict',
        '//Common/ThirdParty/segmenter:crf',
        '//Common/ThirdParty/segmenter:ulccode',
        '//Common/ThirdParty/segmenter:uldict',
        '//Common/ThirdParty/segmenter:ullib',
        '//Common/ThirdParty/segmenter:ullib_ex',
        '//Robot/Public/Declare:declearHead'
        ],
    link_all_symbols=True,
    warning = False,
    )

cc_library(
    name = 'queryProcessManager',
    srcs = ['queryProcessManager.cc',
            ],
    deps = [
        ':removeStopWordHandler',
        #':name_recognition_and_substitution',
        ],
    )

cc_test(
  name = 'queryPreprocessUnittest',
  srcs = ['queryPreprocessUnittest.cc',
  ],
  deps =[
   ':removeStopWordHandler',
    '//Common/Base:Base',
    '//Common/Util/File:simpleLineReader',
   ]
)

cc_binary(
    name = 'Convert2PinyinMain',
    srcs = ['Convert2PinyinMain.cc',
    ],
    deps = [
    '//Common/Base:Base',
    '//Common/Util/File:simpleLineReader',
    '//Common/Util/Trie:jumpTrie',
    ]
)
