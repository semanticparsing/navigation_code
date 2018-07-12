#include "userContext.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Base/flags.h"
#include "Common/Util/Serialize/base64.h"
#include "Common/Util/RedisWrapper/redisHelper.h"
#include "Robot/Public/Utils/rewriteQueryHelper.h"
#include "Robot/Public/Utils/trioLog.h"


DEFINE_string(redis_host,               "127.0.0.1" ,   "redis host");
DEFINE_int32 (redis_port,               6379,           "redis port");
DEFINE_string(redis_passwd,             "1234567Myc",   "redis password");
DEFINE_int32 (redis_pool_size,          20,             "redis pool size");
DEFINE_int32 (redis_expire_time,        600,            "redis EXPIRE key second");
DEFINE_int32 (max_user_context_size,    10,             "max user context size");
DEFINE_string(user_context_prefix,      "ctx_",         "user context redist key prefix");

namespace robot {

UserContextOper::UserContextOper() {

}

UserContextOper::~UserContextOper() {

}

bool UserContextOper::Init() {
    trio::RedisOption opt;
    opt.host = FLAGS_redis_host;
    opt.port = FLAGS_redis_port;
    opt.password = FLAGS_redis_passwd;
    opt.timeout = {1, 500000};
    redis_pool_.reset(new trio::RedisPool(opt, FLAGS_redis_pool_size));
    if(!redis_pool_->Init()) {
        VLOG(TRIO_LOG_ERR) << "user context oper redis pool init failed";
        return false;
    }
    return true;
}

bool UserContextOper::LoadUserContext(const UserInfo& user_info,
                                      UserContext& ctx){
    if (!user_info.__isset.uid || !user_info.__isset.bot_name ||
        user_info.uid.empty()) {
        VLOG(TRIO_LOG_ERR) << "load user context failed empty uid or bot_name" << endl;
        return false;
    }
    std::string key = user_info.bot_name + "_" + \
                      FLAGS_user_context_prefix + user_info.uid;
    std::string base64_value("");
    if (!trio::RedisHelper::GetString(redis_pool_.get(), key, base64_value)) {
        VLOG(TRIO_LOG_ERR) << "load user context get redis failed";
        return false;
    }
    if (!base64_value.empty())
    {
        std::string value("");
        if (!crypto::Base64Decode(base64_value, &value)) {
            VLOG(TRIO_LOG_ERR) << "load user context base64 decode failed";
            return false;
        }
        if (!FromStringToThrift<UserContext>(value, &ctx)) {
            VLOG(TRIO_LOG_ERR) << "load user context thrift decode failed";
            return false;
        }
    }
    return true;
}

void UserContextOper::AddUserContextUnit(const UserContextUnit& unit, UserContext& ctx){
    ctx.user_context.insert(ctx.user_context.begin(), unit);
    if(ctx.user_context.size() > FLAGS_max_user_context_size){
        ctx.user_context.erase(ctx.user_context.end()-1, ctx.user_context.end());
        std::vector<UserContextUnit>(ctx.user_context).swap(ctx.user_context);
    }
}

bool UserContextOper::SaveUserContext(const UserContext& ctx){
    if (!ctx.__isset.user_info || !ctx.user_info.__isset.bot_name ||
        !ctx.user_info.__isset.uid || ctx.user_info.uid.empty()) {
        VLOG(TRIO_LOG_ERR) << "save user context failed because no uid or bot_name";
        return false;
    }
    if (!ctx.__isset.user_context || ctx.user_context.empty()) {
        VLOG(TRIO_LOG_ERR) << "save user context failed because history is empty";
        return false;
    }
    std::string key = ctx.user_info.bot_name + "_" + \
                      FLAGS_user_context_prefix + ctx.user_info.uid;
    std::string value = FromThriftToString<UserContext>(&ctx);
    std::string base64_value("");
    if (value.empty() || !crypto::Base64Encode(value, &base64_value)) {
        VLOG(TRIO_LOG_ERR) << "save user context failed because thrift string or base64encode empty";
        return false;
    }
    if (!trio::RedisHelper::SetString(redis_pool_.get(), key, base64_value, FLAGS_redis_expire_time)) {
        VLOG(TRIO_LOG_ERR) << "save user context set redis failed";
        return false;
    }
    return true;
}

bool UserContextHelper::GetLastUnit(const UserContext& ctx, UserContextUnit& unit) {
    if (ctx.user_context.empty()) {
        return false;
    }
    unit = ctx.user_context[0];
    return true;
}

bool UserContextHelper::GetLastQueryReply(const UserContext& ctx, std::string& last_query, std::string& last_reply) {
    last_query = last_reply = "";
    UserContextUnit unit;
    if (!GetLastUnit(ctx, unit)) {
        return false;
    }
    GetQueryReply(unit, last_query, last_reply);
    return true;
}


void UserContextHelper::GetQueryReply(const UserContextUnit& unit, std::string& query, std::string& reply) {
    RewriteQueryHelper helper(unit.rewrite.prep);
    helper.GetQuery(query);
    reply.clear();
    if (!unit.replys.empty()) {
        reply = unit.replys[0].reply;
    }
}

bool UserContextHelper::GetLastDomainQuery(const UserContext& ctx, std::string& last_query) {
    last_query = "";
    UserContextUnit unit;
    if (!GetLastUnit(ctx, unit)) {
        return false;
    }
    last_query = unit.query;
    return true;
}

} // namespace robot
