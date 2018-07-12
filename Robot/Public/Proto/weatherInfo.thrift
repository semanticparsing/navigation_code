namespace cpp robot
namespace py robot.weatherInfo

include "common.thrift"


enum WeatherType {
  SUNNY = 1;//晴
  CLOUDY = 2;//多云
  SUNNYANDCLOUDY = 3;//晴间多云
  MOSTLYCLOUDY = 4;//大部多云
  OVERCAST = 5;//阴
  SHOWER = 6;//阵雨
  THUNDERSHOWER = 7;//雷阵雨
  THUNDERSHOWERWITHHAIL = 8;//雷阵雨伴有冰雹
  LIGHTRAIN = 9;//小雨
  MODERATERAIN = 10;//中雨
  HEAVYRAIN = 11;//大雨
  RAINSTROM = 12;//暴雨
  HEAVYRAINSTROM = 13;//大暴雨
  EXTRAORDINARYRAINSTROM = 14;//特大暴雨
  FREEZINGRAIN = 15;//冻雨
  RAINWITHSNOW = 16;//雨夹雪
  SNOWSHOWER = 17;//阵雪
  LIGHTSNOW = 18;//小雪
  MODERATESNOW = 19;//中雪
  HEAVYSNOW = 20;//大雪
  HEAVYSNOWFALL = 21;//暴雪
  FLOATINGDUST = 22;//浮尘
  SANDBLOWING = 23;//扬沙
  SANDSTROM = 24;//沙尘暴
  HEAVYSANDSTROM = 25;//强沙尘暴
  FOG = 26;//雾
  HAZE = 27;//霾
  WIND = 28;//风
  HIGHWIND = 29;//大风
  HURRICANE = 30;//飓风
  TROPICALSTROM = 31;//热带风暴
  TORNADO = 32;//龙卷风
}
    

struct weather {
    1: optional WeatherType type;
    2: optional string date;
    3: optional string city;
    4: optional string high;
    5: optional string low;
    6: optional string wind_level;
    7: optional string wind_direction;
    8: optional string humidity;
    9: optional string pm25;
    10: optional string day_of_week;
    11: optional string dressing;
} 
