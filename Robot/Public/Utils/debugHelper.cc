#include "Robot/Public/Utils/debugHelper.h"

namespace robot {

void DebugHelper::DEBUG(robot::TrioDataBundle& bundle, const std::string& module_name, const std::string& debug_msg) {
    if (!bundle.__isset.params || !bundle.params.__isset.is_debug || !bundle.params.is_debug) {
        return;
    }
    if (!bundle.__isset.resp) {
        return;
    }
    if (!bundle.resp.__isset.debug_msgs) {
        bundle.resp.__set_debug_msgs(std::map<std::string, std::string>());
    }
    std::map<std::string, std::string>::iterator iter;
    iter = bundle.resp.debug_msgs.find(module_name);
    if (iter == bundle.resp.debug_msgs.end()){
        bundle.resp.debug_msgs[module_name] = '\n' + debug_msg + '\n';
    }
    else {             
        iter->second += debug_msg;
        iter->second += '\n';
    }
}

bool DebugHelper::ISDEBUG(robot::TrioDataBundle& bundle) {
    if (!bundle.__isset.params || !bundle.params.__isset.is_debug || !bundle.params.is_debug) {
        return false;
    }
    if (!bundle.__isset.resp) {
        return false;
    }
    return true;
}

} // namespace robot
