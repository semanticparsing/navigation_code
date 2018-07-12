#ifndef ROBOT_PUBLIC_USERCONTEXT_H
#define ROBOT_PUBLIC_USERCONTEXT_H

#include <stdio.h>
#include <vector>
#include <string>

#include "Common/Base/flags.h"
#include "Common/Base/stringUtil.h"
#include "Robot/Public/serviceHead.h"
#include "Common/Base/scopedPtr.h"
#include "Common/Util/RedisWrapper/redisPool.h"

namespace robot {

class UserContextOper {
public:
    UserContextOper();
    ~UserContextOper();
    bool Init();
    bool LoadUserContext(const UserInfo& user_info, UserContext& ctx);
    void AddUserContextUnit(const UserContextUnit& unit, UserContext& ctx);
    bool SaveUserContext(const UserContext& ctx);

private:
    bool RedisPoolSet(const std::string& key, const std::string& value);
    bool RedisPoolGet(const std::string& key, std::string* value);
    bool LoadSession(DataBundle& bundle);
    bool SaveSession(DataBundle& bundle);

private:
    scoped_ptr<trio::RedisPool> redis_pool_;
};

class UserContextHelper
{
public:
    static bool GetLastDomainQuery(const UserContext& ctx, std::string& last_query);
    static bool GetLastQueryReply(const UserContext& ctx, std::string& last_query, std::string& last_reply);
    static bool GetLastUnit(const UserContext& ctx, UserContextUnit& unit);
    static void GetQueryReply(const UserContextUnit& unit, std::string& query, std::string& reply);
};

} // namespace robot

#endif // ROBOT_PUBLIC_USERCONTEXT_H
