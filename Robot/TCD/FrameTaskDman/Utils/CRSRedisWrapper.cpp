//
//  RedisWrapper.cpp
//  TrioDM
//
//  Created by Zhuoran Wang on 29/6/2016.
//  Copyright © 2016 Zhuoran Wang. All rights reserved.
//

#include "RedisWrapper.h"
#include "Common.h"
#include "Projects/ChinaMobile/CRS/Src/Interface/OperateRedisInterface.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"

using namespace ChinaMobileProvince;

namespace TrioDM {

RedisWrapper* RedisWrapper::_instance = NULL;

RedisWrapper::RedisWrapper() 
    : _redis_con(NULL),
      m_nTimeOut(0) {
}

RedisWrapper::~RedisWrapper() {
}

bool RedisWrapper::Init(const std::string& host, const u_int16 port, const std::string& passWd) {
    VLOG(DATA) << __FUNCTION__ << ", CRS Do Nothing.";
    return true;
}

RedisWrapper * RedisWrapper::Instance() {
    if (_instance == NULL) {
        _instance = new RedisWrapper();
    }
    return _instance;
}

bool RedisWrapper::hSet(const std::string& key, const std::string& hKey, const std::string& value) {
    VLOG(DEBUG) << __FUNCTION__ << ", key: " << key << ", hkey: " << hKey;
    return RedisOperator_HSet(key, hKey, value);
}

bool RedisWrapper::hMSet(const std::string& key, const std::map<std::string, std::string>& values) {
    VLOG(DEBUG) << __FUNCTION__ << ", Key: " << key << ", values.size: " << values.size();
    Json::FastWriter fast_writer;
    Json::Value j_data;
    j_data["jsonKey"] = key;
    j_data["jsonValue"] = Json::Value();
    Json::Value& j_value = j_data["jsonValue"];

    for ( std::map<std::string, std::string>::const_iterator c_iter = values.begin();c_iter != values.end(); ++c_iter ) {
        j_value[c_iter->first] = c_iter->second;
    }
    return RedisOperator_ExecuteNoQuery("hmset", fast_writer.write(j_data));
}

bool RedisWrapper::hGet(const std::string& key, const std::string& hKey, std::string& value) {
    VLOG(DEBUG) << __FUNCTION__ << ", key: " << key << ", hkey: " << hKey << ", value: " << value;
    return RedisOperator_HGet(key, hKey, value);
}

bool RedisWrapper::hGetAll(const std::string& key, std::map<std::string, std::string>& value) {
    VLOG(DEBUG) << __FUNCTION__ << ", key: " << key;
    value.clear();
    std::string output;
    Json::FastWriter fast_writer;
    Json::Value param;
    param["jsonKey"] = key;
    if ( !RedisOperator_ExecuteQuery("hgetAll" ,fast_writer.write(param), output) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Call RedisOperator Error..";
        return false;
    }
    VLOG(DEBUG) << __FUNCTION__ << ", Output: " << output;
    Json::Reader reader;
    Json::Value j_data;
    if ( !reader.parse(output, j_data) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Json Parse Error, Output: " << output;
        return false;
    }
    for (Json::ValueIterator iter = j_data.begin();iter != j_data.end(); ++iter) {
        std::string k = iter.key().asString();
        if ( !(*iter).isString() ) {
            VLOG(WARNING) << __FUNCTION__ << ", Error, Output: " << output;
            return false;
        }
        std::string v = (*iter).asString();
        value.insert(std::pair<std::string, std::string>(k, v));
    }

    return true;
}

bool RedisWrapper::hDel(const std::string& key, const std::string& hKey) {
    VLOG(DEBUG) << __FUNCTION__ << ", key: " << key << ", hkey: " << hKey;
    bool ret = false;
    if ( !RedisOperator_HDel(key, hKey, ret) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Call RedisOperator Error..";
        return false;
    }
    return ret;
}

bool RedisWrapper::rPush(const std::string& key, const std::string& value) {
    VLOG(WARNING) << __FUNCTION__ << ", Not Achieve.";
    return false;
}

bool RedisWrapper::lGet(const std::string& key, std::vector<std::string>& value) {
    VLOG(WARNING) << __FUNCTION__ << ", Not Achieve.";
    return false;
}

bool RedisWrapper::sAdd(const std::string& key, const std::string& value) {
    VLOG(WARNING) << __FUNCTION__ << ", Not Achieve.";
    return false;
}

bool RedisWrapper::sMembers(const std::string& key, std::vector<std::string>& value) {
    VLOG(WARNING) << __FUNCTION__ << ", Not Achieve.";
    return false;
}

bool RedisWrapper::set(const std::string& key, const std::string& val, int ex_tim) {
    VLOG(DEBUG) << __FUNCTION__ << ", Key: " << key << ", val: " << val << ", ex_tim; " << ex_tim;
    if ( ex_tim == 0 ) {
        return RedisOperator_Set(key, val, "-1" );
    } else {
        return RedisOperator_Set(key, val, IntToString(ex_tim) );
    }
}

bool RedisWrapper::get(const std::string& key, std::string& value ) {
    VLOG(DEBUG) << __FUNCTION__ << ", key: " << key;
    return RedisOperator_Get(key, value);
}

std::string RedisWrapper::Get(const std::string& key) {
    VLOG(DEBUG) << __FUNCTION__ << ", key: " << key;
    std::string value;
    if ( !RedisOperator_Get(key, value) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Call RedisOperator Error..";
        return "";
    }
    return value;
}

bool RedisWrapper::exists(const std::string& key) {
    VLOG(DEBUG) << __FUNCTION__ << ", key: " << key;
    bool ret = false;
    if ( !RedisOperator_Exist(key, ret) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Call RedisOperator Error..";
        return false;
    }
    return ret;
}

bool RedisWrapper::del(const std::string& key) {
    VLOG(DEBUG) << __FUNCTION__ << ", key: " << key;
    bool ret = false;
    if ( !RedisOperator_Remove(key, ret) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Call RedisOperator Error..";
        return false;
    }
    return ret;
}

bool RedisWrapper::expire(const std::string& key, u_int timeOut) {
    VLOG(WARNING) << __FUNCTION__ << ", Not Achieve..";
    return false;
}

bool RedisWrapper::ReConnectRedis() {
    VLOG(DATA) << __FUNCTION__ << ", Not Need Achieve..";
    return true;
}

bool RedisWrapper::ConnectRedis() {
    VLOG(DATA) << __FUNCTION__ << ", Not Need Achieve..";
    return true;
}

void RedisWrapper::DestoryCon() {
    VLOG(DATA) << __FUNCTION__ << ", Not Need Achieve..";
}

void RedisWrapper::keepalive() {
}

}
