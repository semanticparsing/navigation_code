
#ifndef ROBOT_PUBLIC_UTILS_DEBUG_HELPER_H
#define ROBOT_PUBLIC_UTILS_DEBUG_HELPER_H

#include <map>
#include <string>

#include "Common/Base/basictypes.h"
#include "Robot/Public/serviceHead.h"

namespace robot {

class DebugHelper {
private:
    DebugHelper() {}
public:
    static void DEBUG(robot::TrioDataBundle& bundle, const std::string& module_name, const std::string& debug_msg);
    static bool ISDEBUG(robot::TrioDataBundle& bundle);
private:
    DISALLOW_COPY_AND_ASSIGN(DebugHelper);
};

} // namespace robot

#endif // ROBOT_PUBLIC_UTILS_DEBUG_HELPER_H
