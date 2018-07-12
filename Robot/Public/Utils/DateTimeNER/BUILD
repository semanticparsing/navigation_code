cc_binary(
  name = 'ner_test',
  srcs = [
    'ner_test.cc',
  ],
  deps = [
    #'//Common/Util/FrameSemanticParser:parser',
    '//Robot/Public/FrameDman/SLU/frame_semantic_parser:parser_without_encode',
    '//Common/ThirdParty/segmenter:all',
    ':dateTimeNER',
  ],
  extra_cppflags = ['-std=c++11']
)

cc_library(
  name = 'dateTimeNER',
  srcs = [
    'dateTimeNER.cc',
  ],
  deps = [
    '//Common/Base:Base',
    '//Common/Util/File:simpleLineReader',
    '//Robot/Public/FrameDman/SLU/frame_semantic_parser:parser_without_encode',
    '//Common/ThirdParty/segmenter:all',
  ],
)
