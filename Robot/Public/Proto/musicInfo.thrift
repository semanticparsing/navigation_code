namespace cpp robot
namespace py robot.music

include "common.thrift"


struct music {
    1: optional string link_url;
    2: optional string name;
    3: optional string singer;
    4: optional string ablum;
    5: optional string lrc_url;
    6: optional string image_url;
    7: optional string duration;
    8: optional string label;
} 
