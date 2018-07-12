//
//  RedisWrapper.cpp
//  TrioDM
//
//  Created by Zhuoran Wang on 29/6/2016.
//  Copyright © 2016 Zhuoran Wang. All rights reserved.
//

#include "RedisWrapper.h"
#include "Common.h"

#define K_MILLISECOND_PER_SECOND 1000

#define CHECK_CONNECTED() \
    do{\
        if( _redis_con == NULL || _redis_con->err ) {\
            VLOG(WARNING) << __FUNCTION__ << ", not connected redis";\
            if ( !ReConnectRedis() ) {\
                return false;\
            }\
        }\
    }while(0)\

#define CHECK_REPLY() \
    do{\
        if( reply == NULL || reply->type == REDIS_REPLY_ERROR ) {\
            if( reply ) {\
                VLOG(WARNING) << __FUNCTION__ << ", redisCommand error, " << reply->str;\
                freeReplyObject(reply);\
            } else {\
                VLOG(WARNING) << __FUNCTION__ << ", redisCommand error, reply is NULL;";\
            }\
            return false;\
        }\
    } while(0)\

namespace TrioDM {

RedisWrapper* RedisWrapper::_instance = NULL;

RedisWrapper::RedisWrapper() 
    : _redis_con(NULL),
      m_nTimeOut(0) {
}

RedisWrapper::~RedisWrapper() {
    DestoryCon();
}

bool RedisWrapper::Init(const std::string& host, const u_int16 port, const std::string& passWd) {
    m_strHost = host;
    m_nPort = port;
    m_strPassWd = passWd;
    if ( !ConnectRedis() ) {
        return false;
    }
    // TO DO
    // keep alive
    return true;
}

RedisWrapper * RedisWrapper::Instance() {
    if (_instance == NULL) {
        _instance = new RedisWrapper();
    }
    return _instance;
}

bool RedisWrapper::hSet(const std::string& key, const std::string& hKey, const std::string& value) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "hset %s %s %s", key.c_str(), hKey.c_str(), value.c_str());
    CHECK_REPLY();
    freeReplyObject(reply);
    return true;
}

bool RedisWrapper::hMSet(const std::string& key, const std::map<std::string, std::string>& values) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    std::vector<const char*> argv;
    std::vector<size_t> argv_len;

    std::string cmd = "HMSET";
    argv.push_back(cmd.c_str());
    argv.push_back(key.c_str());
    argv_len.push_back(cmd.length());
    argv_len.push_back(key.length());

    for ( std::map<std::string, std::string>::const_iterator c_iter = values.begin();c_iter != values.end(); ++c_iter ) {
        argv.push_back(c_iter->first.c_str());
        argv_len.push_back(c_iter->first.length());
        argv.push_back(c_iter->second.c_str());
        argv_len.push_back(c_iter->second.length());
    }
    VLOG(DEBUG) << __FUNCTION__ << ", argv.size: " << argv.size() << ", argv_len.size: " << argv_len.size();
    redisReply *reply = static_cast<redisReply *>(redisCommandArgv(_redis_con, argv.size(), &(argv[0]), &(argv_len[0])));
    CHECK_REPLY();
    freeReplyObject(reply);
    return true;
}

bool RedisWrapper::hGet(const std::string& key, const std::string& hKey, std::string& value) {
    bool ret = false;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    value.clear();
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "hget %s %s", key.c_str(), hKey.c_str());
    CHECK_REPLY();
    if (reply->type == REDIS_REPLY_STRING) {
        value = reply->str;
        ret = true;
    }
    freeReplyObject(reply);
    return ret;
}

bool RedisWrapper::hGetAll(const std::string& key, std::map<std::string, std::string>& value) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    bool ret = false;
    value.clear();
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "hgetall %s", key.c_str());
    CHECK_REPLY();
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0;i < reply->elements; i+=2) {
            redisReply* k = reply->element[i];
            redisReply* v = reply->element[i+1];
            if (k->type == REDIS_REPLY_STRING && k->type == REDIS_REPLY_STRING) {
                value.insert(std::make_pair(k->str, v->str));
            }
        }
        ret = true;
    }
    freeReplyObject(reply);
    return ret;
}

bool RedisWrapper::hDel(const std::string& key, const std::string& hKey) {
    bool ret = false;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "hdel %s %s", key.c_str(), hKey.c_str());
    CHECK_REPLY();
    ret = reply->type == REDIS_REPLY_INTEGER && reply->integer == 1;
    freeReplyObject(reply);
    return ret;
}

bool RedisWrapper::rPush(const std::string& key, const std::string& value) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "rpush %s %s", key.c_str(), value.c_str());
    CHECK_REPLY();
    freeReplyObject(reply);
    return true;
}

bool RedisWrapper::lGet(const std::string& key, std::vector<std::string>& value) {
    bool ret = false;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    value.clear();
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "llen %s", key.c_str());
    CHECK_REPLY();
    if (reply->type == REDIS_REPLY_INTEGER) {
        long long len = reply->integer;
        freeReplyObject(reply);
        reply = (redisReply*)redisCommand(_redis_con, "lrange %s 0 %ll", key.c_str(), len);
        CHECK_REPLY();
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0;i < reply->elements; ++i) {
                redisReply* v = reply->element[i];
                if (v->type == REDIS_REPLY_STRING) {
                    value.push_back(v->str);
                }
            }
        }
        ret = true;
    }
    freeReplyObject(reply);
    return ret;
}

bool RedisWrapper::sAdd(const std::string& key, const std::string& value) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "sadd %s %s", key.c_str(), value.c_str());
    CHECK_REPLY();
    freeReplyObject(reply);
    return true;
}

bool RedisWrapper::sMembers(const std::string& key, std::vector<std::string>& value) {
    bool ret = false;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    value.clear();
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "smembers %s", key.c_str());
    CHECK_REPLY();
    if (reply->type == REDIS_REPLY_ARRAY) {
        for(size_t i = 0;i < reply->elements; ++i) {
            redisReply* v = reply->element[i];
            if (v->type == REDIS_REPLY_STRING) {
                value.push_back(v->str);
            }
        }
        ret = true;
    }
    freeReplyObject(reply);
    return ret;
}

bool RedisWrapper::set(const std::string& key, const std::string& val, int ex_tim) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "set %s %s", key.c_str(), val.c_str());
    CHECK_REPLY();
    freeReplyObject(reply);
    if (0 != ex_tim) {
        return expire(key, ex_tim);
    }
    return true;
}

bool RedisWrapper::get(const std::string& key, std::string& value ) {
    
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "get %s", key.c_str());
    CHECK_REPLY();
    if ( reply->type == REDIS_REPLY_STRING ) {
        value = reply->str;
        freeReplyObject(reply);
        return true;
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", Not String, Type: " << reply->type;
        freeReplyObject(reply);
        return false;
    }
}

std::string RedisWrapper::Get(const std::string& key) {
    std::string value;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    
    if( _redis_con == NULL || _redis_con->err ) {
        VLOG(WARNING) << __FUNCTION__ << ", not connected redis";
        if ( !ReConnectRedis() ) {
            return "";
        }
    }
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "get %s", key.c_str());
    
    do{
        if( reply == NULL || reply->type == REDIS_REPLY_ERROR ) {
            if( reply ) {
                VLOG(WARNING) << __FUNCTION__ << ", redisCommand error, " << reply->str;
                freeReplyObject(reply);
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", redisCommand error, reply is NULL;";
            }
            return "";
        }
    } while(0);

    if ( reply != NULL && reply->type == REDIS_REPLY_STRING) {
        value = reply->str;
    }

    if ( reply != NULL ) {
        freeReplyObject(reply);
    }

    return value;
}

bool RedisWrapper::exists(const std::string& key) {
    bool ret = false;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "exists %s", key.c_str());
    CHECK_REPLY();
    ret = reply->type == REDIS_REPLY_INTEGER && reply->integer == 1;
    freeReplyObject(reply);
    return ret;
}

bool RedisWrapper::del(const std::string& key) {
    bool ret = false;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "del %s", key.c_str());
    CHECK_REPLY();
    ret = reply->type == REDIS_REPLY_INTEGER && reply->integer >= 0;
    freeReplyObject(reply);
    return ret;
}

bool RedisWrapper::expire(const std::string& key, u_int timeOut) {
    bool ret = false;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CHECK_CONNECTED();
    redisReply* reply = (redisReply*)redisCommand(_redis_con, "expire %s %u", key.c_str(), timeOut);
    CHECK_REPLY();
    ret = reply->type == REDIS_REPLY_INTEGER && reply->integer == 1;
    freeReplyObject(reply);
    return ret;
}

bool RedisWrapper::ReConnectRedis() {
    DestoryCon();
    return ConnectRedis();
}

bool RedisWrapper::ConnectRedis() {
    VLOG(DATA) << __FUNCTION__;
    if ( m_nTimeOut == 0 ) {
        _redis_con = redisConnect(m_strHost.c_str(), m_nPort);
    } else {
        u_int second = m_nTimeOut / K_MILLISECOND_PER_SECOND;
        u_int mill = m_nTimeOut % K_MILLISECOND_PER_SECOND; 
        struct timeval timeout = {second, mill};
        _redis_con = redisConnectWithTimeout(m_strHost.c_str(), m_nPort, timeout);
    }
    if ( _redis_con == NULL ) {
        return false;
    }
    if ( _redis_con->err ) {
        return false;
    }
    VLOG(DATA) << __FUNCTION__ << ", RedisCon: Err: " << _redis_con->err << ", Msg: " << _redis_con->errstr
        << ", flags: " << _redis_con->flags;
    if ( !m_strPassWd.empty() ) {
        redisReply* reply = (redisReply*)redisCommand(_redis_con, "AUTH %s", m_strPassWd.c_str());
        CHECK_REPLY();
        freeReplyObject(reply);
    }
    return true;
}

void RedisWrapper::DestoryCon() {
    if (_redis_con != NULL) {
        redisFree(_redis_con);
        _redis_con = NULL;
    }
}

void RedisWrapper::keepalive() {
}

}
