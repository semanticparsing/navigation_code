namespace cpp chinaMobile
namespace java chinaMobile
namespace py chinaMobile.struct

struct InParams {
    1: optional string call_id; // 呼叫唯一标识，通常和userid相同
    2: optional string inter_idx; // 交互序号 (CRS返回的内容, 原样返回即可)

    // whe inaction == 8
    3: optional string call_sor_id; // 主叫号码
    4: optional string call_dst_id; // 被叫号码
    5: optional string flow;
    6: optional string auth_token; // 受权信息
    7: optional string entrance_id;
    8: optional string start_time;
    9: optional string ses_time;
    10: optional string suilu_region_id;
    11: optional string cc_id;
    12: optional string role_label;

    // when inaction == 9
    13: optional string begin_play; // 放音开始时间
    14: optional string end_play; // 放音结束时间
    15: optional string result_time; // 获取结果时间
    16: optional string flow_result_type; // 结果类型
    17: optional string input;
    //当上一轮对话模板前三位为100时，即只放音不识别时，有4种可能：1-hangup  2-timeout  3-sys_err  4-playover
    //当上一轮对话模板前三位为110时，即放音+语音识别时，有5种可能：1-hangup  2-timeout 3-nomatch 4-<结果> 5-sys_err
    //当上一轮对话模板前三位为101时，即放音+按键识别时，有4种可能：1-hangup  2-timeout  3-sys_err  4-按键的结果
    24: optional string inter_no; // ASR识别结果
    18: optional string org; // 语义资源包(CRS返回内容, 原样返回即可)
    19: optional string newsess; // 清空上下文标志(CRS返回内容, 原样返回即可)

    // when inaction == 11
    20: optional string trans_idx; // 转移序号 (CRS返回的内容, 原样返回即可)
    21: optional string begin_trans; // 转移的开始时间
    22: optional string end_trans; // 转移的结束时间
    23: optional string trans_result; // 转移成功与否
}

struct OutParams {
    1: optional string call_id; // 呼叫唯一标识，通常和userid相同
    2: optional string inter_idx; // 交互序号 (IVR的值+1)

    // when outaction  == 9
    3: optional string flow_type; // 交互类型(日志字段)
    4: optional string flow_node_name; // 节点名称(日志字段)
    5: optional string model_type; // 模板类型
    //model_type 前三位 101 分别代表是否支持  放音、识别、按键,   1代表支持
    //model_type 中间两位 13 代表支持按键的情况下, 最大按键长度是多少
    //model_type 最后两位 11  代表 * # 是否为功能键
    //如1011311    代表本次交互  支持放音、按键,  最大的按键长度为13位,   *号为取消键,  #号为确认键
    //如1100000    代表本次交互支持放音、识别， 不支持按键
    6: optional string prompt_type; // 放音类型，1-播放指定提示音，2-播放TTS动态文本
    7: optional string prompt_wav; // 语音文件名(不包含路径和后缀)，当prompt_type =1 时, 播放该语音文件, 需要拼接文件路径 和后缀名
    8: optional string prompt_text; // TTS动态文本内容，当prompt_type=2 时, 使用TTS播放这个变量的值
    9: optional string timeout; // 超时时长，指在支持识别 or 按键的时候,  放音结束之后等待用户响应的时长，默认为5
    10: optional string grammar; // 语法文件名(不包含路径和后缀)，发起识别时使用的语法文件名
    11: optional string org; // 语义资源
    12: optional string newsess; // 上下文清空标志
    13: optional string delay; // 占位符
    14: optional string act_params; // 占位符

    // when outaction  == 11 / 10
    15: optional string call_sor_id; // 主叫号码
    // when outaction = 11
    16: optional string trans_idx; // 跳转序数，默认为空
    17: optional string trans_type; // 转移类型: 1 转人工, 2转按键, ... (可扩展)
    18: optional string route_value; // 路由值信息
    // when outaction = 10
    19: optional string call_dst_id; // 被叫号码
    20: optional string start_time; // 呼叫开始时间
    21: optional string end_time; // 呼叫结束时间
    22: optional string region_id; //用户归属地
    23: optional string entrance_id; //入口维度
    24: optional string exit_id; //出口维度
    25: optional string user_type_id; //用户类型
    26: optional string suilu_region_id; //呼叫接入地
}

struct ChinaMobileBundle {
    1: optional string userid; // 呼叫唯一标emainingSpeechSoundInquiry

    // input part start
    2: optional i32 inaction;
    3: optional InParams inparams;
    // input part end

    // output part start
    4: optional i32 ret;
    5: optional i32 outaction;
    6: optional OutParams outparams;
    // output part end

    //interal part start
    7: optional string authResult;
    8: optional string NLUResult;
    9: optional string ASRResult;
    //interal part end

    //本次请求的唯一标识
    10: optional string trace_id;
}
