namespace cpp robot
namespace py robot.audioInfo

include "common.thrift"


struct audio {
    1: optional string source_type;
    2: optional string name;
    3: optional string author;
    4: optional string ablum;
    6: optional string ablum_url;
    7: optional string play_url;
} 
