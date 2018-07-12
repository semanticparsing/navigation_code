#include "Robot/Public/Utils/trioDataBundleHelper.h"

namespace robot {

bool TrioDataBundleHelper::CheckSetDefault(robot::TrioDataBundle& bundle) {
    if (!bundle.__isset.query || bundle.query.empty()) {
        VLOG(0) << "trio bundle empty query" << endl;
        return false;
    }
    if (!bundle.__isset.params) {
        VLOG(0) << "trio bundle params null" << endl;
        return false;
    }
    if (!bundle.__isset.user || !bundle.user.__isset.uid || !bundle.user.__isset.platform) {
        VLOG(0) << "trio bundle empty uid" << endl;
        return false;
    }

    //set
    if (!bundle.__isset.user_ctx) {
        robot::UserContext uctx;
        bundle.__set_user_ctx(uctx);
    }
    if (!bundle.__isset.resp) {
        robot::ResponseParams resp;
        bundle.__set_resp(resp);
    }
    if (!bundle.__isset.ctx){
        robot::ServiceContext sctx;
        bundle.__set_ctx(sctx);
    }
    return true;
}

bool TrioDataBundleHelper::Convert(const robot::DataBundle& src, robot::TrioDataBundle& dest, const DefaultConvertParam* p) {
    static DefaultConvertParam default_params;
    if (NULL == p) p = &default_params;

    dest.__set_query(src.query);
    robot::ResponseParams params;
    params.__set_return_status(src.return_status);
    params.__set_proactive_reply(src.proactive_reply);
    vector<Reply> replys;
    if (p->use_replys){
        replys = src.replys;
    } else {
        replys.clear();
        replys.push_back(src.reply);
    }
    params.__set_replys(replys);
    dest.__set_resp(params);
    return true;
}

bool TrioDataBundleHelper::Convert(const robot::TrioDataBundle& src, robot::DataBundle& dest) {
    dest.__set_query(src.query);
    if (!dest.__isset.user_id){
        dest.__set_user_id(src.user.uid);
    }
    if (!dest.__isset.user_info){
        dest.__set_user_info(src.user);
    }
    return true;
}


void log_huge_string(const std::string& flag, int level, const std::string& str) {
    VLOG(level) << "---hugestart---" << flag << "---";
    int size = str.size();
    int per = 1024;
    int num = (size + per - 1) / per;
    int pos = 0;
    for (int i = 0; i < num; i++) {
        int this_size = per;
        if (pos + per > size) {
            this_size = size - pos;
        }
        VLOG(level) << str.substr(pos, this_size);
        pos += this_size;
    }
    VLOG(level) << "---hugeend---" << flag << "---";
}

} // namespace robot
