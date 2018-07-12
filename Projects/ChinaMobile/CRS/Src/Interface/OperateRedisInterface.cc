#include "HttpOperator.h"
#include "OperateRedisInterface.h"
#include "Common/Base/logging.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"

namespace ChinaMobileProvince {

DEFINE_string(REDIS_SET_URL, "/httpservice/put2Cache", "set_url");
DEFINE_string(REDIS_GET_URL, "/httpservice/getFormCache", "get_url");
DEFINE_string(REDIS_REMOVE_URL, "/httpservice/delFromCache", "remove_url");
DEFINE_string(REDIS_EXIST_URL, "/httpservice/exists", "exist_url");

DEFINE_string(REDIS_HSET_URL, "/httpservice/hset", "hset_url");
DEFINE_string(REDIS_HGET_URL, "/httpservice/hget", "hget_url");
DEFINE_string(REDIS_HDEL_URL, "/httpservice/hdel", "hdel_url");

bool RedisOperator_Set(const string &key, const string &value, const string &timeout)
{
    string outStr = "";
    string url = FLAGS_REDIS_SET_URL;
    Json::Value jsonRequest;
    jsonRequest["jsonKey"] = key;
    jsonRequest["jsonValue"] = value;
    jsonRequest["timeout"] = timeout;
    Json::FastWriter writerTmp;
    string content = writerTmp.write(jsonRequest);
    int ret = SendHttpPost2Java(url, content, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret: " << ret;
        return false;
    }
    if ("true" != outStr)
    {
        VLOG(0) << "redis return is not [true], return: " << outStr;
        return false;
    }
    return true;
}


bool RedisOperator_Exist(const string &key, bool &result)
{
    return RedisOperator_ExistAndRemove(key, FLAGS_REDIS_EXIST_URL, result);
}

bool RedisOperator_ExistAndRemove(const string &key, const string& url, bool &result)
{
    string outStr = "";
    Json::Value jsonRequest;
    jsonRequest["jsonKey"] = key;
    Json::FastWriter writerTmp;
    string content = writerTmp.write(jsonRequest);

    int ret = SendHttpPost2Java(url, content, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret: " << ret;
        return false;
    }
    if ("true" == outStr)
    {
        result = true;
    }
    else
    {
        result = false;
    }
    return true;
}

bool RedisOperator_Remove(const string &key, bool &result)
{
    return RedisOperator_ExistAndRemove(key, FLAGS_REDIS_REMOVE_URL, result);
}

bool RedisOperator_HSet(const string &key, const string &field, const string &value)
{
    string outStr = "";
    string url = FLAGS_REDIS_HSET_URL;
    Json::Value jsonRequest;
    jsonRequest["jsonKey"] = key;
    jsonRequest["jsonValue"] = value;
    jsonRequest["field"] = field;
    Json::FastWriter writerTmp;
    string content = writerTmp.write(jsonRequest);
    int ret = SendHttpPost2Java(url, content, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret: " << ret;
        return false;
    }
    if ("true" != outStr)
    {
        VLOG(0) << "redis return is not [true], return: " << outStr;
        return false;
    }
    return true;
}

bool RedisOperator_HGet(const string &key, const string &field, string &value)
{
    string outStr = "";
    string url = FLAGS_REDIS_HGET_URL;
    Json::Value jsonRequest;
    jsonRequest["jsonKey"] = key;
    jsonRequest["field"] = field;
    Json::FastWriter writerTmp;
    string content = writerTmp.write(jsonRequest);
    int ret = SendHttpPost2Java(url, content, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret:"<<ret;
        return false;
    }
    value = outStr;
    return true;
}

bool RedisOperator_HDel(const string &key, const string &field, bool &result)
{
    string outStr = "";
    string url = FLAGS_REDIS_HDEL_URL;
    Json::Value jsonRequest;
    jsonRequest["jsonKey"] = key;
    jsonRequest["field"] = field;
    Json::FastWriter writerTmp;
    string content = writerTmp.write(jsonRequest);
    int ret = SendHttpPost2Java(url, content, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret: " << ret;
        return false;
    }
    if("true" == outStr)
    {
        result = true;
    }
    else
    {
        result = false;
    }
    return true;
}

bool RedisOperator_ExecuteNoQuery(const string &command, const string &jsonStr)
{
    string outStr = "";
    string url = "/httpservice/" + command;

    int ret = SendHttpPost2Java(url, jsonStr, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret: " << ret;
        return false;
    }
    if ("true" != outStr)
    {
        VLOG(0) << "redis return is not [true], return: " << outStr << ", command: " << command;
        return false;
    }
    return true;
}

bool RedisOperator_ExecuteQuery(const string &command, const string &inputJsonStr, string &outputJsonStr)
{
    string outStr = "";
    string url = "/httpservice/" + command;
    int ret = SendHttpPost2Java(url, inputJsonStr, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret: " << ret;
        return false;
    }
    outputJsonStr = outStr;
    return true;
}

bool RedisOperator_Get(const string &url, const string &key, string &value)
{
    string outStr = "";
    Json::Value jsonRequest;
    jsonRequest["jsonKey"] = key;
    Json::FastWriter writerTmp;
    string content = writerTmp.write(jsonRequest);
    int ret = SendHttpPost2Java(url, content, outStr);
    if (0 != ret)
    {
        VLOG(0) << "call Http Operator error, ret: " << ret;
        return false;
    }
    value = outStr;
    return true;
}

bool RedisOperator_Get(const string &key, string &value)
{
    return RedisOperator_Get(FLAGS_REDIS_GET_URL, key, value);
}
}
