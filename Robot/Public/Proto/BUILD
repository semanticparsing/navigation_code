thrift_library(
  name = 'commonThrift',
  srcs = ['common.thrift'],
)

proto_library(
  name = 'commonProto',
  srcs = ['common.proto'],
)

proto_library(
  name = 'intentSelectionProto',
  srcs = ['intentSelection.proto'],
  deps = [':commonProto'],
)

thrift_library(
     name = 'controlThrift',
     srcs = ['control.thrift'],
)

thrift_library(
     name = 'remindThrift',
     srcs = ['remind.thrift'],
)

thrift_library(
     name = 'weatherInfoThrift',
     srcs = ['weatherInfo.thrift'],
)

thrift_library(
     name = 'musicInfoThrift',
     srcs = ['musicInfo.thrift'],
)

thrift_library(
     name = 'audioInfoThrift',
     srcs = ['audioInfo.thrift'],
)

thrift_library(
    name = 'takeoutThrift',
    srcs = ['takeout.thrift'],
)
thrift_library(
    name = 'newsInfoThrift',
    srcs = ['newsInfo.thrift'],
)
thrift_library(
     name = 'skillIntentThrift',
     srcs = ['skill_intent.thrift'],
)
thrift_library(
    name = 'structThrift',
    srcs = ['struct.thrift'],
    deps = [
        '//Robot/Public/Proto:controlThrift',
        '//Robot/Public/Proto:commonThrift',
        '//Robot/Public/Proto:remindThrift',
        '//Robot/Public/Proto:weatherInfoThrift',
        '//Robot/Public/Proto:musicInfoThrift',
        '//Robot/Public/Proto:audioInfoThrift',
        '//Robot/Public/Proto:takeoutThrift',
        '//Robot/Public/Proto:newsInfoThrift', 
        ':trioUserProfileThrift',
    ],
)

thrift_library(
  name = 'trioUserProfileThrift',
  srcs = ['userProfile.thrift'],
  deps = [':commonThrift'],
)

thrift_library(
    name = 'serviceThrift',
    srcs = ['service.thrift'],
    deps = [
        '//Robot/Public/Proto:structThrift'
    ],
)

proto_library(
    name = 'serviceParamsProto',
    srcs = ['serviceParams.proto'],
)

proto_library(
    name = 'serviceConfigProto',
    srcs = ['serviceConfig.proto'],
    deps = [
        '//Robot/Public/Proto:serviceParamsProto',
    ],
)

proto_library(
    name = 'logParamsProto',
    srcs = ['logParams.proto'],
)
