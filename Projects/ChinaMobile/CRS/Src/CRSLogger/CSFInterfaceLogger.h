#ifndef CHINA_MOBILE_FLUME_LOG_H_
#define CHINA_MOBILE_FLUME_LOG_H_

#include <map>
#include <string>

#include "Projects/ChinaMobile/Proto/ChinaMobileCRSServing.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/ChinaMobileSession.h"

using namespace std;

namespace ChinaMobileProvince {

//ininterLogInfo : ininter日志信息 用于记录 调用CSF接口 的日志信息
/************************************************
序号 源字段名称              类型       宽度  字段描述
1    CHAN                    varchar    50    会话id，每一通会话的唯一标识。可忽略301
2    EVENT                   varchar    50    事件类型    采用固定字段 TB_ICA_IN_INTER_D
3    TIME                    varchar    50    会话开始时间。可忽略302
4    _LOGLIB_LOG_RANDOM_ID_  varchar    50    记录日志的库中生成的id号。可忽略303
5    call_id                 varchar    40    通话唯一标识
6    inter_idx               varchar    30    交互序数
7    interface_ecode         varchar    20    接口原因编码
8    interface_etime         varchar    20    接口调用结束时间    格式 yyyyMMddHH:mm:ss
9    interface_idx           varchar    30    接口序数
10   interface_result        varchar    40    接口输出
11   interface_stime         varchar    20    接口调用开始时间    格式 yyyyMMddHH:mm:ss
12   pid                     varchar    50    写日志进程号。可忽略304
13   sort_time               varchar    50    当前处理时间，内部使用，为了排序，保证有序即可。可忽略305
14   tid                     varchar    50    写日志线程号。可忽略306
15   timestamp               varchar    50    写日志时间戳。可忽略307
16   accept_id               varchar    19    受理号码    即 被叫号码
17   business_name           varchar    80    业务名称    例如 话费查询
18   call_sor_id             varchar    19    主叫号码
19   interface_input         varchar    40    接口输入
20   interface_name          varchar    40    接口名称
21   templateCode            varchar    20    模板编号
22   interface_sendMsg       varchar    50    失败原因
23   interface_sendCode      varchar    20    短信下发状态码
24   smsContent              varchar    200   短信内容
25   call_day_yd             varchar    20    通话时间
26   AB_test                 varchar    20    AB测试，某个版本或为空
27   province_sign           varchar    10    省份地域标签
************************************************/
typedef struct _ininterLogInfo{
    string  CHAN;
    string  EVENT;
    string  TIME;
    string  LOGLIB_LOG_RANDOM_ID;
    string  call_id;
    string  inter_idx;
    string  interface_ecode;
    string  interface_etime;
    string  interface_idx;
    string  interface_result;
    string  interface_stime;
    string  pid;
    string  sort_time;
    string  tid;
    string  timestamp;
    string  accept_id;
    string  business_name;
    string  call_sor_id;
    string  interface_input;
    string  interface_name;
    string  templateCode;
    string  interface_sendMsg;
    string  interface_sendCode;
    string  smsContent;
    string  call_day_yd;
    string  AB_test;
    string  province_sign;
    _ininterLogInfo() {
        CHAN="";
        EVENT="";
        TIME="";
        LOGLIB_LOG_RANDOM_ID="";
        call_id="";
        inter_idx="";
        interface_ecode="";
        interface_etime="";
        interface_idx="";
        interface_result="";
        interface_stime="";
        pid="";
        sort_time="";
        tid="";
        timestamp="";
        accept_id="";
        business_name="";
        call_sor_id="";
        interface_input="";
        interface_name="";
        templateCode="";
        interface_sendMsg="";
        interface_sendCode="";
        smsContent="";
        call_day_yd="";
        AB_test="";
        province_sign="";
    }
}IninterLogInfo;

//IncommuLogInfo : incommu 日志信息 用于记录 CRS与IVR 交互的日志信息
/************************************************
序号 源字段名称              类型   宽度  字段描述
1    CHAN                    varchar 50   会话id，每一通会话的唯一标识。可忽略201
2    EVENT                   varchar 50   事件类型    采用固定字段 TB_ICA_IN_COMMU_D
3    TIME                    varchar 50   会话开始时间。可忽略202
4    _LOGLIB_LOG_RANDOM_ID_  varchar 50   记录日志的库中生成的id号。可忽略203
5    begin_play              varchar 20   提示音开始播放时间
6    busi_word_result        varchar 200  语义结果映射出的业务名称
7    business_name           varchar 50   业务结果
8    call_id                 varchar 40   通话唯一标识
9    crs_result              varchar 100  识别结果
10   end_play                varchar 20   提示音结束播放时间
11   flow_node_name          varchar 50   流程节点名称。可忽略204
12   flow_result             varchar 50   流程执行结果。可忽略205
13   flow_result_time        varchar 20   识别结果返回时间
14   flow_result_type        varchar 8    识别输入方式
15   flow_type               varchar 8    节点类型
16   grammar                 varchar 200  语法
17   inter_idx               varchar 30   交互序数
18   inter_no                varchar 30   识别序号
19   pid                     varchar 20   写日志进程号。可忽略206
20   prompt_text             varchar 2000 提示音文本
21   prompt_type             varchar 20   提示音类型
22   prompt_wav              varchar 200  提示音名称
23   simp_word_result        varchar 500  精简语义结果
24   sort_time               varchar 50   当前处理时间，内部使用，为了排序，保证有序即可。可忽略207
25   tid                     varchar 30   写日志线程号。可忽略208
26   timestamp               varchar 30   写日志时间戳。可忽略209
27   call_day_yd             varchar 20   通话时间
28   AB_test                 varchar 20   AB测试，某个版本或为空
29   province_sign           varchar 10   省份地域标签
************************************************/
typedef struct _incommuLogInfo {
    string CHAN;
    string EVENT;
    string TIME;
    string _LOGLIB_LOG_RANDOM_ID_;
    string begin_play;
    string busi_word_result;
    string business_name;
    string call_id;
    string crs_result;
    string end_play;
    string flow_node_name;
    string flow_result;
    string flow_result_time;
    string flow_result_type;
    string flow_type;
    string grammar;
    string inter_idx;
    string inter_no;
    string pid;
    string prompt_text;
    string prompt_type;
    string prompt_wav;
    string simp_word_result;
    string sort_time;
    string tid;
    string timestamp;
    string call_day_yd;
    string AB_test;
    string province_sign;
    _incommuLogInfo() {
        CHAN = "";
        EVENT = "";
        TIME = "";
        _LOGLIB_LOG_RANDOM_ID_ = "";
        begin_play = "";
        busi_word_result = "";
        business_name = "";
        call_id = "";
        crs_result = "";
        end_play = "";
        flow_node_name = "";
        flow_result = "";
        flow_result_time = "";
        flow_result_type = "";
        flow_type = "";
        grammar = "";
        inter_idx = "";
        inter_no = "";
        pid = "";
        prompt_text = "";
        prompt_type = "";
        prompt_wav = "";
        simp_word_result = "";
        sort_time = "";
        tid = "";
        timestamp = "";
        call_day_yd = "";
        AB_test = "";
        province_sign = "";
    }
} IncommuLogInfo;

//IncallLogInfo : incall 日志信息 用于记录 挂机 的日志信息
/************************************************
序号 源字段名称             类型   宽度 字段描述
1    CHAN                   varchar 50  会话id，每一通会话的唯一标识。可忽略101
2    EVENT                  varchar 50  事件类型
3    TIME                   varchar 50  会话开始时间。可忽略102
4    _LOGLIB_LOG_RANDOM_ID_ varchar 50  记录日志的库中生成的id号。可忽略103
5    brand_id               varchar 15  品牌
6    call_dst_id            varchar 19  被叫号码
7    call_id                varchar 40  通话唯一标识
8    call_sor_id            varchar 19  主叫号码
9    end_time               varchar 20  通话结束时间
10   entrance_id            varchar 8   入口
11   exit_id                varchar 8   出口
12   level_id               varchar 20  星级
13   pid                    varchar 50  写flume日志进程号。可忽略104
14   region_id              varchar 50  归属地
15   sort_time              varchar 50  当前处理时间，内部使用，为了排序，保证有序即可。可忽略105
16   start_time             varchar 20  通话开始时间
17   suilu_region_id        varchar 10  呼叫地
18   tid                    varchar 50  写flume日志线程号。可忽略106
19   timestamp              varchar 50  写flume日志时间戳。可忽略107
20   user_type_id           varchar 50  预付费标志
21   call_day_yd            varchar 20  通话时间
22   panoramic_wav          varchar 200 全景音频文件名
23   AB_plan                varchar 50  AB方案名称
24   AB_version             varchar 20  AB方案版本
25   AB_test                varchar 20  AB测试，某个版本或为空
26   province_sign          varchar 10  省份地域标签
************************************************/
typedef struct _incallLogInfo {
    string CHAN;
    string EVENT;
    string TIME;
    string _LOGLIB_LOG_RANDOM_ID_;
    string brand_id;
    string call_dst_id;
    string call_id;
    string call_sor_id;
    string end_time;
    string entrance_id;
    string exit_id;
    string level_id;
    string pid;
    string region_id;
    string sort_time;
    string start_time;
    string suilu_region_id;
    string tid;
    string timestamp;
    string user_type_id;
    string call_day_yd;
    string panoramic_wav;
    string AB_plan;
    string AB_version;
    string AB_test;
    string province_sign;
    _incallLogInfo() {
        CHAN = "";
        EVENT = "";
        TIME = "";
        _LOGLIB_LOG_RANDOM_ID_ = "";
        brand_id = "";
        call_dst_id = "";
        call_id = "";
        call_sor_id = "";
        end_time = "";
        entrance_id = "";
        exit_id = "";
        level_id = "";
        pid = "";
        region_id = "";
        sort_time = "";
        start_time = "";
        suilu_region_id = "";
        tid = "";
        timestamp = "";
        user_type_id = "";
        call_day_yd = "";
        panoramic_wav = "";
        AB_plan = "";
        AB_version = "";
        AB_test = "";
        province_sign = "";
    }
} IncallLogInfo;

class ChinaMobileCSFInterfaceLogger{
public:
    ChinaMobileCSFInterfaceLogger();
    virtual ~ChinaMobileCSFInterfaceLogger();
    // 根据IncallLogInfo结构体,发送flume日志信息
    static bool IninterLogRecorder(IninterLogInfo ininter_log_info);
    // 根据IncommuLogInfo结构体,发送flume日志信息
    static bool IncommuLogRecorder(IncommuLogInfo incommu_log_info);
    // 根据IncallLogInfo结构体,发送flume日志信息
    static bool IncallLogRecorder(IncallLogInfo incall_log_info);

    /**
    * @brief 发送Ininter的flume日志信息
    * @param[in] csfRequest    请求的csf报文,用于取得必要的flume字段信息
    * @param[in] interface_ecode  接口原因编码
    * @param[in] business_name  业务名称
    * @param[in] tm_interface_stime  调用csf接口开始时间
    * @param[in] tm_interface_etime  调用csf接口结束时间
    * @param[in] sessionId  对应的sessionId
    * @param[in] telephoneNumber  主叫号码
    * @param[in] res_code  接口调用返回结果
    * @param[in] smsContent  短信内容
    * @param[in] inter_idx  交互序数
    * @param[in] interface_idx  接口序数
    * @param[in] accept_id  受理号码
    * @param[in] templateCode  模板编号(暂未使用)
    * @param[in] interface_sendMsg  失败原因
    * @param[in] start_time  通话开始时间
    * @return 是否成功发送flume日志信息 true 成功 false 失败
    */
    static bool IninterLogRecorder(const string& csfRequest, const string& interface_ecode, const string& business_name,
                                   const tm& tm_interface_stime, const tm& tm_interface_etime,
                                   const string& sessionId, const string& telephoneNumber, const string& res_code, const string& smsContent,
                                   const string& inter_idx, const string& interface_idx, const string& accept_id,
                                   const string& templateCode, const string& interface_sendMsg, const string& call_day_yd);

    /**
    * @brief 发送Incommu的flume日志信息
    * @param[in] response    请求的ChinaMobileBundle,用于取得必要的flume字段信息
    * @param[in] output      交互的节点,用于取得所在的业务名称
    * @param[in] start_time  通话开始时间
    * @return 是否成功发送flume日志信息 true 成功 false 失败
    */
    static bool IncommuLogRecorder(const chinaMobile::ChinaMobileBundle& response, const TrioTDM::COutput& output, const string& start_time);
    /**
    * @brief 发送Incall的flume日志信息
    * @param[in] response    请求的ChinaMobileBundle,用于取得必要的flume字段信息
    * @param[in] *session    对应的session指针, 用于取得session所保存的字段
    * @param[in] c_end_time  挂机时间
    * @param[in] exit_id     出口维度id,此处未使用,实际从session中所取得
    * @param[in] panoramic_wav    全景音频文文件名(暂未使用)
    * @return 是否成功发送flume日志信息 true 成功 false 失败
    */
    static bool IncallLogRecorder(const chinaMobile::ChinaMobileBundle &response, ChinaMobileProvince::ChinaMobileSession *session, const Time &c_end_time,
                                  const string &exit_id, const string &panoramic_wav);
    // 根据time_t格式时间 格式化为string; 格式为 yyyyMMddHH:mm:ss
    static string Time_tToFormatString(time_t interface_time_t);

private:
    // 将IninterLogInfo结构体转换为Json字符串(flume发送格式)
    static string IninterLogInfoToAnyLog(IninterLogInfo ininter_log_info);
    // 将IncommuLogInfo结构体转换为Json字符串(flume发送格式)
    static string IncommuLogInfoToAnyLog(IncommuLogInfo incommu_log_info);
    // 将IncallLogInfo结构体转换为Json字符串(flume发送格式)
    static string IncallLogInfoToAnyLog(IncallLogInfo incall_log_info);
    // 限制字符最大长度，采取截断方式。目的是符合flume字符长度限制要求
    static string LimitVarchar(string varchars, unsigned int limit_num);
    // 根据tm格式时间 格式化为string; 格式为 yyyyMMddHH:mm:ss
    static string TmToFormatString(tm interface_time);
    static string UtcStringToFormatString(string utc_string);
    // 根据节点名字 取得对应的业务名称
    static string GetBusinessName(const string &agents_flow);
    // 根据GenAgent名字 取得对应的业务名称
    static string GetBusinessNameByGenAgentName(const string &gen_agent_name);
    // 根据Agent的名字(根节点) 取得对应的业务名称
    static string AgentNameToBusinessName(const string &agent_name);
    // 初始化 m_mapBusinessName
    const static map<string, string> CreateBusinessNameMap();

private:
    // 流程树根节点名称 与 业务名称的映射
    static const map<string, string> m_mapBusinessName;
};

//const map<string, string> ChinaMobileCSFInterfaceLogger::m_mapBusinessName = ChinaMobileCSFInterfaceLogger::CreateBusinessNameMap();

} // namespace ChinaMobileProvince

#endif  // CHINA_MOBILE_FLUME_LOG_H_
