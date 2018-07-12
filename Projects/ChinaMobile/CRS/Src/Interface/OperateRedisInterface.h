#ifndef _OPERATE_REDIS_INTERFACE_H_
#define _OPERATE_REDIS_INTERFACE_H_

#include <string>

using namespace std;

namespace ChinaMobileProvince {

    bool RedisOperator_Get(const string &url, const string &key, string &value);

    bool RedisOperator_Set(const string &key, const string &value, const string &timeout);
    bool RedisOperator_Get(const string &key, string &value);
    bool RedisOperator_Exist(const string &key, bool &result);
    bool RedisOperator_Remove(const string &key, bool &result);

    bool RedisOperator_HSet(const string &key, const string &field, const string &value);
    bool RedisOperator_HGet(const string &key, const string &field, string &value);
    bool RedisOperator_HDel(const string &key, const string &field, bool &result);

    bool RedisOperator_ExecuteNoQuery(const string &command, const string &jsonStr); // 当前可用command:hmset、expire
    bool RedisOperator_ExecuteQuery(const string &command, const string &inputJsonStr, string &outputJsonStr); // 当前可用command:hgetAll
    bool RedisOperator_ExistAndRemove(const string &key, const string& url, bool &result);

} // namespace ChinaMobileProvince
#endif //_OPERATE_REDIS_INTERFACE_H_
