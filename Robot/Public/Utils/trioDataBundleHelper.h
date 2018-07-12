
#ifndef ROBOT_PUBLIC_UTILS_TRIO_DATA_BUNDLE_HELPER_H
#define ROBOT_PUBLIC_UTILS_TRIO_DATA_BUNDLE_HELPER_H

#include <map>
#include <string>

#include "Common/Base/basictypes.h"
#include "Robot/Public/serviceHead.h"

namespace robot {

    class DefaultConvertParam
    {
    public:
        DefaultConvertParam() {
            use_replys = false;
        }
    public:
        bool use_replys;
    };

    class ReplysConvertParam : public DefaultConvertParam
    {
    public:
        ReplysConvertParam() {
            use_replys = true;
        }
    };

class TrioDataBundleHelper {
private:
    TrioDataBundleHelper() {}
public:
    static bool CheckSetDefault(robot::TrioDataBundle& bundle);
    static bool Convert(const robot::DataBundle& src, robot::TrioDataBundle& dest, const DefaultConvertParam* p);
    static bool Convert(const robot::TrioDataBundle& src, robot::DataBundle& dest);
private:
    DISALLOW_COPY_AND_ASSIGN(TrioDataBundleHelper);
};

void log_huge_string(const std::string& flag, int level, const std::string& str);

} // namespace robot

#endif // ROBOT_PUBLIC_UTILS_DEBUG_HELPER_H
