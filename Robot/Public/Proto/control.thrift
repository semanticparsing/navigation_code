namespace cpp robot
namespace py robot.control

include "common.thrift"

// IOT指令
enum Instruction {
  // 空气净化器指令
  UNKNOWN = 0;
  TURN_ON = 1;//打开
  TURN_OFF = 2;//关闭
  MODE_MUTE = 3;//静音
  MODE_AUTO = 4;//自动
  MODE_SLEEP = 5;//睡眠
  MODE_HIGH = 6;//高
  MODE_MIDDLE = 7;//中
  MODE_LOW = 8;//低
  NET_IONIC_ON = 9;//净离子开
  NET_IONIC_OFF = 10;//净离子关
  CHILD_LOCK_ON = 11;//儿童锁开
  CHILD_LOCK_OFF = 12;//儿童锁关
  TURN_UP = 13;//调亮/调高/调大/开大
  TURN_DOWN = 14;//调暗/调低/调小/开小
  ASK = 15;//询问质量

  // 窗帘指令
  TURN_STOP = 16;//停止/静止
  SET = 17;//位置

  // 空调指令
  MODE_COLD = 18;//制冷
  MODE_DEWET = 19;//制湿
  MODE_HOT = 20;//制热
  MODE_TOP = 21;//最高
  MODE_BOTTOM = 22;//最低

  // 风扇指令
  MODE_SWING = 23;

  // 电视指令
  TV_RETRIEVE = 24;//恢复
  TV_VOLUME = 25;//音量
  TV_PAUSE = 26;//暂定
  TV_GO_ON = 27;//继续
  TV_STOP = 28; //停止
  TV_MUTE = 29;//静音
  TV_FAST_FORWARD = 30;//快进
  TV_REWIND = 31;//快退
  TV_MENU = 32;//菜单
  TV_HOMEPAGE = 33;//主页
  TV_BACK = 34;//上一步
  TV_VOLUME_UP = 35;//音量调高
  TV_VOLUME_DOWN = 36;// 音量调低
  TV_VOLUME_TOP = 37;//最高
  TV_VOLUME_BOTTOM = 38;//最低
  TV_VOLUME_UP_VALUE = 39;//调高多少
  TV_VOLUME_DOWN_VALUE = 40;//调低多少
  TV_VOLUME_VALUE = 41;//具体

  // 插座指令

  // 空气盒子指令

  // 电灯指令
  MODE_LIFE = 42;
  MODE_READ = 43;
  MODE_CINEMA = 44;
  MODE_SOFT = 45;

  // 加湿器指令
  MODE_NORMAL = 46;

  // 电热水器指令

  // 扫地机指令
  MODE_SIDE = 47;
  MODE_TIMING = 48;
  MODE_RETURN = 49;
  FRONT = 50;
  BACK = 51;

  WIND_AUTO = 52;

  // 运动控制
  SPORT_FRONT = 53;
  SPORT_BACK = 54;
  SPORT_LEFT = 55;
  SPORT_RIGHT = 56;
  SPORT_CLOCKWISE = 57;
  SPORT_ANTICLOCKWISE = 58;
  SPORT_DANCE = 59;
  SPORT_STOP = 60;

  //  控制命令
  VOLUME_UP   = 100;
  VOLUME_DOWN = 101;
  VOLUME_SET  = 102;

  PLAY_PAUSE  = 110;
  PLAY_RESUME = 111;
  PLAY_PRE    = 112;
  PLAY_NEXT   = 113;
  PLAY_STOP   = 114;
  PLAY_QUIT   = 115;
  PLAY_FF     = 116;  // Fast Forward
  PLAY_REW    = 117;  // Rewind
  PLAY_SHARE  = 118;
  PLAY_FAVOR  = 119;
  PLAY_SINGLE_CYCLE = 120;
  PLAY_ORDER_PLAY = 121;

  // 收藏控制
  FAVOR = 200;

  // 充电控制
  CHARGE = 300;

  // 基本设备操作
  DEV_OPEN = 1000;
  DEV_CLOSE = 1001;
  DEV_PAUSE = 1002;
  DEV_CANCLE = 1003;
  DEV_STOP = 1004;
  DEV_MODE_OPEN = 1005;
  DEV_MODE_CLOSE = 1006;
  DEV_MODE_SET = 1007;
  DEV_PROP_UP = 1008;
  DEV_PROP_DOWN = 1009;
  DEV_PROP_SET = 1010;
  DEV_PROP_UP_VALUE = 1011;
  DEV_PROP_DOWN_VALUE = 1012;
  DEV_STATUS = 1013;
}

//智能设备类型
enum IotDeviceType {
  UNKNOWN = 0;
  AIR_CLEANER = 1;      // 空气净化器
  CURTAIN = 2;          // 窗帘
  AIR_CONDITIONER = 3;  // 空调
  FAN = 4;              // 电风扇
  TV_INSTRUCTION = 5;   // 电视
  SOCKET = 6;           // 插座
  AIR_BOX = 7;          // 空气盒子(空气检测仪)
  LIGHT = 8;            // 灯具
  HUMIDIFIER = 9;       // 加湿器
  WATER_HEATER = 10;    // 热水器(电|燃气)
  SWEEPER = 11;         // 扫地机器人
  SPORT_CONTROL = 12;
  BUILDIN  = 13;
  NOTYPE = 14; // has no type, jd device has no type
  COOKER = 15;          // 电饭煲
  REFRIGERATOR = 16;    // 冰箱
  WATER_CLEANER = 17;   // 净水器
  DISH_WASHER = 18;     // 洗碗机
  WASH_MACHINE = 19;    // 洗衣机
  GATE_LOCK = 20;       // 门锁
  HEATER = 21;          // 取暖器
  DEHUMIDIFIER = 22;    // 除湿器
  SCALES = 23;          // 智能秤
  SET_TOP_BOX = 24;     // 机顶盒
  PRESSURE_COOKER = 25; // 电压力锅
  KETTLE = 26;          // 电水壶
  STEWPAN = 27;         // 电炖锅
  OVEN = 28;            // 电烤箱
  STEAM_BOX = 29;       // 电蒸箱
  BREAD_MACHINE = 30;   // 面包机
  MILK_MACHINE = 31;    // 酸奶机
  MICROWAVE_OVEN = 32;  // 微波炉
  HEMOPIEZOMETER = 33;  // 血压计
  BROKEN_MACHINE = 34;  // 破壁料理机
  GAS_TUBINE = 35;      // 烟机
  CLOTHES_HORSE = 36;   // 智能晾衣架
  JUICE_MACHINE = 37;   // 榨汁机
  STERILIZER = 38;      // 消毒柜
  COOKING_UTENSILS = 39;// 灶具
  FRID_MACHINE = 40;    // 炒菜机
  DRYER = 41;           // 干衣机
  SWITCH = 42;          // 开关
  PRINTER = 43;         // 打印机
  PROJECTION = 44;      // 投影仪
  CAMERA = 45;          // 摄像头
  FOOT_PLATE = 46;      // 足浴盆
  ROUTER = 47;          // 路由器
  HEALTH_POT = 48;      // 养生壶
  CAKE_PAN = 49;        // 电饼铛
  AIR_FRIER = 50;       // 空气炸锅
  SOYMILK_MAKER = 51;   // 豆浆机
  JD_SCENE = 52;        // 京东场景
}

#enum FrontendStatus {
#    IDLE = 0;
#    PLAYING = 1;
#    DANCING = 2;
#}

// 回传晓乐的用户设备控制的参数
struct ControlInstruction {
  // 设备控制指令
  1: optional Instruction instruction;
  // 控制设备类型
  2: optional IotDeviceType iot_device_type;
  // 控制指令参数(比如：1/2)
  3: optional string value;
  // 控制指令参数(比如：卧室、客厅)
  4: optional string position;
  // 控制指令参数(给晓乐自有的BUILDIN控制用)
  6: optional common.Domain domain;
  7: optional list<common.Domain> domains;
}

struct ControlFeatures {
  1: optional ControlInstruction ci;
  2: optional string control_reply;
  3: optional map<string, string> control_slots;
  4: optional string control_instruction_link;
  5: optional string control_parser_line;
}
