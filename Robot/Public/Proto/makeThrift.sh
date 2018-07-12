t=$1
thrift -gen ${t} common.thrift
thrift -gen ${t} control.thrift
thrift -gen ${t} remind.thrift
thrift -gen ${t} weatherInfo.thrift
thrift -gen ${t} musicInfo.thrift
thrift -gen ${t} service.thrift
thrift -gen ${t} skill_intent.thrift
thrift -gen ${t} struct.thrift
thrift -gen ${t} userProfile.thrift
thrift -gen ${t} audioInfo.thrift
thrift -gen ${t} takeout.thrift
thrift -gen ${t} newsInfo.thrift
blade build
