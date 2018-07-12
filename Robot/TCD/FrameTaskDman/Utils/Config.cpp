//
//  Ontology.cpp
//  TrioDM
//
//  Created by Zhuoran Wang on 25/6/2016.
//  Copyright © 2016 Zhuoran Wang. All rights reserved.
//
#include "Config.h"
#include <fstream>
#include <boost/algorithm/string/classification.hpp> 
#include <boost/algorithm/string/split.hpp>

#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"

#include "Log.h"
#include "Utils.h"


namespace TrioTDM {

Config* Config::_instance = NULL;

Config::Config() {
    
}

Config::~Config() {
    
}

const Config* Config::Instance() {
    if ( _instance == NULL ) {
        _instance = new Config;
    }
    return _instance;
}

bool Config::init(const std::string& filename) {
    if ( _instance == NULL ) {
        return false;
    }
    return _instance->_load_json(filename);
}

bool Config::_load_json(const std::string& filename) {
    
    Json::Reader reader(Json::Features::strictMode());
    Json::FastWriter fast_writer;
    _json_config.clear();
    
    std::ifstream ifs(filename.c_str());
    
    if ( !ifs.is_open() ) {
        VLOG(FATAL) << "Unable to open " << filename;
        return false;
    }
    
    if ( ! reader.parse( ifs, _raw_json_config ) )
    {
        VLOG(FATAL) << "Unable to parse " << filename;
        ifs.close();
        return false;
    }
    
    ifs.close();
    
    for( Json::ValueIterator itr = _raw_json_config.begin() ; itr != _raw_json_config.end() ; itr++ ) {
        if ( ! _add(Trim(itr.key().asString()), *itr) ) {
            VLOG(FATAL) << "Format error " << fast_writer.write(*itr);
            return false;
        }
    }
    
    return true;
}

bool Config::_add(std::string key, const Json::Value& obj) {
    // VLOG(DEBUG) << __FUNCTION__ << ", Key: " << key;
    if ( Trim(key).empty() ) {
        return false;
    }
    if ( !obj.isObject() ) {
        if ( !key.empty() ) {
            _json_config[key] = obj;
            return true;
        } else {
            return false;
        }
    }
    
    for( Json::ValueConstIterator itr = obj.begin() ; itr != obj.end() ; itr++ ) {
        std::string sub_key = Trim(itr.key().asString());
        if ( sub_key.empty() ) {
            return false;
        }
        if ( !_add(key+"/"+sub_key,*itr) ) {
            return false;
        }
    }
    return true;
}

std::string Config::get_value(const std::string& key, const std::string& d_value) const {
    if ( !_json_config[key].isString() ) {
        VLOG(DATA) << __FUNCTION__ << ", Can not find Key: " << key;
        return d_value;
    }
    return _json_config[key].asString();
}

int Config::GetIntValue(const std::string& key, int d_value) const {
    if ( !_json_config[key].isString() ) {
        VLOG(DATA) << __FUNCTION__ << ", Can not find Key: " << key;
        return d_value;
    }
    std::string num = _json_config[key].asString();
    return atoi(num.c_str());
}

bool Config::get_key_value(const std::string& key, std::map<std::string, std::string>& value) const {
    value.clear();
    std::vector<std::string> itemKeys;
    boost::split( itemKeys, key, boost::is_any_of("/") );  
    Json::Value j_tmp_data;
    bool isFirst = true;

    for (std::vector<std::string>::iterator iter = itemKeys.begin();iter != itemKeys.end(); ++iter) {
        const std::string& itemKey = *iter;
        if ( isFirst ) {
            isFirst = false;
            if ( _raw_json_config[itemKey].isObject() ) {
                j_tmp_data = _raw_json_config[itemKey];
            } else {
                return false;
            }
        } else {
            if ( j_tmp_data[itemKey].isObject() ) {
                j_tmp_data = j_tmp_data[itemKey];
            } else {
                return false;
            }
        }
    }

    for ( Json::ValueIterator iter = j_tmp_data.begin(); iter != j_tmp_data.end(); ++iter ) {
        if ( !_add(Trim(iter.key().asString()), *iter, value) ) {
            value.clear();
            return false;
        }
    }

    return true;
}

bool Config::get_key_value(const std::string& key, std::map<std::string, std::map<std::string, std::string> >& value) const {
    value.clear();
    std::vector<std::string> itemKeys;
    boost::split( itemKeys, key, boost::is_any_of("/") );  
    Json::Value j_tmp_data;
    bool isFirst = true;

    for (std::vector<std::string>::iterator iter = itemKeys.begin();iter != itemKeys.end(); ++iter) {
        const std::string& itemKey = *iter;
        if ( isFirst ) {
            isFirst = false;
            if ( _raw_json_config[itemKey].isObject() ) {
                j_tmp_data = _raw_json_config[itemKey];
            } else {
                return false;
            }
        } else {
            if ( j_tmp_data[itemKey].isObject() ) {
                j_tmp_data = j_tmp_data[itemKey];
            } else {
                return false;
            }
        }
    }

    for ( Json::ValueIterator iter = j_tmp_data.begin(); iter != j_tmp_data.end(); ++iter ) {
        const Json::Value& obj = *iter;
        if ( !obj.isObject() ) {
            value.clear();
            return false;
        }
        std::map<std::string, std::string> tmp;
        for ( Json::ValueIterator iiter = obj.begin();iiter != obj.end(); ++iiter ) {
            if ( !_add(Trim(iiter.key().asString()), *iiter, tmp) ) {
                value.clear();
                return false;
            }
        }
        value.insert(std::make_pair(Trim(iter.key().asString()), tmp));
    }

    return true;
}

bool Config::_add(std::string key, const Json::Value& obj, std::map<std::string, std::string>& value) const {
    if ( key.empty() ) {
        return false;
    }
    if ( !obj.isObject() ) {
        value[key] = obj.asString();
        return true;
    }
    
    for( Json::ValueConstIterator itr = obj.begin() ; itr != obj.end() ; itr++ ) {
        std::string sub_key = Trim(itr.key().asString());
        if ( sub_key.empty() ) {
            return false;
        }
        if ( !_add(key+"/"+sub_key,*itr, value) ) {
            return false;
        }
    }
    return true;
}

}
