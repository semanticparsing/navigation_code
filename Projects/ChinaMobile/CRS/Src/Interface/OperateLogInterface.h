#ifndef _OPERATE_LOG_INTERFACE_H_
#define _OPERATE_LOG_INTERFACE_H_

#include <string>

namespace ChinaMobileProvince {

using namespace std;
    // 通过flume写入日志
    bool BusinessLogWrite(const string &msg, string &result);

    // 通过kafka写入日志
    // topic 由中移提供，每个topic对应一张表
    // key 表示kafka中插入分区 "-1"时JAVA取随机数
    // msg 要输入的日志的json串
    bool BusinessLogWrite_kafka(const string &topic, const string &msg, string &result, string key = "-1");

} // namespace ChinaMobileProvince
#endif //_OPERATE_LOG_INTERFACE_H_
