#include "HttpOperator.h"
#include "Common/Base/logging.h"
#include "Common/Util/HttpClient/curlWrapper.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"
#include "Common/Base/time.h"
#include <memory>

DEFINE_string(java_http, "http://127.0.0.1:28079", "java tomcat url");

namespace ChinaMobileProvince {

enum
{
    ERR_SUCCEED = 0,
    ERR_RESPONSE_FORMAT = -1,
    ERR_RESPONSE_JAVA = -2,
    ERR_SENDHTTP = -3,
    ERR_RETTYPE = -4,
    ERR_NOINIT = -5,
};

#define SET_DATA_VALUE(data, value) do \
                                    { \
                                        if (data.isNull() || data.type() != Json::stringValue) \
                                        { \
                                            VLOG(0) << __FUNCTION__ << "\t"#data << "is not string" << ", Type: " << data.type(); \
                                        } \
                                        else \
                                        { \
                                            value = data.asString(); \
                                        } \
                                    } while(0)

#if 0
static bool g_bHasInit = false;
static string g_sUrlRoot = "http://139.219.69.127:8080";

void InitializeHttpPost2Java(const string &urlRoot)
{
    common::CurlWrapper::Init();
    g_sUrlRoot = urlRoot;
    g_bHasInit = true;
}
#endif
int SendHttpPost2Java(const string &url, const string &post_content, string &out)
{
    int ret = ERR_SUCCEED;
#if 0
    if (g_bHasInit != true)
    {
        VLOG(0) << "HttpPost2Java need init!";
        return ERR_NOINIT;
    }
#endif
    map<string, string> headerSetting;
    string error = "";
    string result = "";
    string type = "Content-type";
    string value = "Content-type:application/json;charset=UTF-8";
    headerSetting.insert(pair<string, string>(type, value));
    headerSetting.insert(pair<string, string>("User-Agent", "libcurl-agent/1.0"));
    string realUrl = FLAGS_java_http + url;
    int64 st_time = common::GetTimeInMs();
    VLOG(1) << __FUNCTION__ << ", start, url : " << realUrl << " post_content: " << post_content;
    ret = common::CurlWrapper::GetPostRespondData(realUrl, post_content, headerSetting, &result, &error);
    VLOG(1) << __FUNCTION__ << ", over, url : " << realUrl << " result : " << result << " call time : " << common::GetTimeInMs() - st_time;
    if (0 != ret)
    {
        VLOG(0) << "Send Http Failed error: " << error;
        return ERR_SENDHTTP;
    }
    Json::Value jsonResult;
    unique_ptr<Json::Reader> reader(new Json::Reader(Json::Features::strictMode()));
    if (!reader->parse(result, jsonResult))
    {
        VLOG(0) << "CallJava json format error!";
        return ERR_RESPONSE_FORMAT;
    }

    string res_code;
    SET_DATA_VALUE(jsonResult["code"], res_code);
    if ("0" == res_code)
    { // 成功
        if (jsonResult["data"].isObject())
        {
            Json::Value realResponseValue = jsonResult["data"];
            Json::FastWriter writer;
            out = writer.write(realResponseValue);
            ret = ERR_SUCCEED;
        }
        else if (jsonResult["data"].isString())
        {
            out = jsonResult["data"].asString();
            ret = ERR_SUCCEED;
        }
        else
        {
            VLOG(0) << "ret data is not object or string";
            ret = ERR_RETTYPE;
        }
    }
    else
    { // 失败
        ret = ERR_RESPONSE_JAVA;
        string errMsg = "";
        SET_DATA_VALUE(jsonResult["data"], errMsg);
        VLOG(0) << "java response error, msg: " << errMsg;
        out = errMsg;
    }
    return ret;
}
}
