//
//  Config.h
//  TrioDM
//
//  Created by Zhuoran Wang on 25/6/2016.
//  Copyright © 2016 Zhuoran Wang. All rights reserved.
//

#ifndef __TrioDM_Config__
#define __TrioDM_Config__

#include <stdio.h>
#include <string>

#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"

namespace TrioTDM {
/**
 * @brief 配置信息管理类
 */
class Config {
public:
    /**
     * @brief 初始化
     */
    static bool init(const std::string& filename);
    static const Config* Instance() ;
    /**
     * @brief 返回配置信息
     * @param[in] key 配置信息的关键字
     * @param[in] d_value 当不存在关键字时返回的模式信息
     */
    std::string get_value(const std::string& key, const std::string& d_value = "") const;
    /**
     * @brief 返回整形配置信息
     */
    int GetIntValue(const std::string& key, int d_value = 0) const;
    /**
     * @brief 获取配置信息
     * @param[in] key 配置信息的关键字
     * @param[out] value 配置信息
     */
    bool get_key_value(const std::string& key, std::map<std::string, std::string>& value) const;
    /**
     * @brief 获取配置信息
     * @param[in] key 配置信息的关键字
     * @param[out] value 配置信息
     */
    bool get_key_value(const std::string& key, std::map<std::string, std::map<std::string, std::string> >& value) const;
private:
    Config();
    virtual ~Config();
    /**
     * @brief 加载 json 格式的配置文件
     */
    bool _load_json(const std::string& filename);
    bool _add(std::string key, const Json::Value& obj);
    bool _add(std::string key, const Json::Value& obj, std::map<std::string, std::string>& value) const;
    static Config* _instance;
    Json::Value _json_config;
    Json::Value _raw_json_config;
};
}

#endif /* __TrioDM_Config__ */
