//
//  RedisWrapper.h
//  TrioDM
//
//  Created by Zhuoran Wang on 29/6/2016.
//  Copyright © 2016 Zhuoran Wang. All rights reserved.
//
#ifndef __TrioDM__RedisWrapper__
#define __TrioDM__RedisWrapper__

#include <map>
#include <pthread.h>
#include <string>
#include <vector>
#include <boost/thread.hpp>

#include "Common/ThirdParty/hiredis/hiredis.h"


namespace TrioDM  {
/**
 * @brief redis 连接管理类
 */
 class RedisWrapper {
    typedef unsigned short int u_int16;
    typedef unsigned int u_int;
    public:
        bool Init(const std::string& host = "127.0.0.1", const u_int16 port = 6379, const std::string& passWd = "");
        static RedisWrapper * Instance();

        void setHost(const std::string& host) {m_strHost = host;}
        void setPort(u_int16 port) {m_nPort = port;}
        void setPassWd(const std::string& passWd) {m_strPassWd = passWd;}
        void setTimeOut(u_int timeOut) {m_nTimeOut = timeOut;}

        // hash
        bool hSet(const std::string& key, const std::string& hKey, const std::string& value);
        bool hMSet(const std::string& key, const std::map<std::string, std::string>& values);
        bool hGet(const std::string& key, const std::string& hKey, std::string& value);
        bool hGetAll(const std::string& key, std::map<std::string, std::string>& value);
        bool hDel(const std::string& key, const std::string& hKey);

        // list
        bool rPush(const std::string& key, const std::string& value);
        bool lGet(const std::string& key, std::vector<std::string>& value);

        // set
        bool sAdd(const std::string& key, const std::string& value);
        bool sMembers(const std::string& key, std::vector<std::string>& value);

        // string
        bool set(const std::string& key, const std::string& val, int ex_time = 0);
        bool get(const std::string& key, std::string& value);
        std::string Get(const std::string& key);

        // key
        bool exists(const std::string& key);
        bool del(const std::string& key);
        bool expire(const std::string& key, u_int timeOut);

    private:
        RedisWrapper();
        virtual ~RedisWrapper();
        bool ReConnectRedis();
        bool ConnectRedis();
        void DestoryCon();
        void keepalive();

    private:
        boost::recursive_mutex _mutex;
        static RedisWrapper* _instance;
        redisContext *_redis_con;
        // millisecond
        u_int m_nTimeOut;
        std::string m_strHost;
        u_int16 m_nPort;
        std::string m_strPassWd;
};

}

#endif /* __TrioDM__RedisWrapper__ */
