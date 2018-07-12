#include "HttpOperator.h"
#include "OperateLogInterface.h"
#include "Common/Base/logging.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"

namespace ChinaMobileProvince {

DEFINE_string(LOG_BUSINESS_WRITE_URL, "/httpservice/produceBusiLogs", "logwrite_url");
DEFINE_string(LOG_BUSINESS_FLUME_WRITE_URL, "/httpservice/proFlumeBusiLogs", "bizlogwrite_flume_url");

bool BusinessLogWrite(const string &msg, string &result)
{
    string outStr = "";
    string url = FLAGS_LOG_BUSINESS_FLUME_WRITE_URL;

    int ret = SendHttpPost2Java(url, msg, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret: "<< ret;
        return false;
    }
    if ("true" != outStr)
    {
        VLOG(0) << "redis return is not [true], return: "<< outStr;
        result = outStr;
        return false;
    }
    return true;
}

bool BusinessLogWrite_kafka(const string &topic, const string &msg, string &result, string key/* = "-1"*/)
{
    string outStr = "";
    string url = FLAGS_LOG_BUSINESS_WRITE_URL;
    Json::Value jsonRequest;
    jsonRequest["topic"] = topic;
    jsonRequest["key"] = key;
    jsonRequest["msg"] = msg;
    Json::FastWriter writerTmp;
    string content = writerTmp.write(jsonRequest);
    int ret = SendHttpPost2Java(url, content, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret: " << ret;
        return false;
    }
    result = outStr;
    return true;
}

}
