#ifndef ConfigurationManager_H_
#define ConfigurationManager_H_

using namespace std;

#include <map>
#include <string>

namespace ConfigurationManager {
class ConfigurationManager
{
public:
    ConfigurationManager() {
        ;
    }
    virtual ~ConfigurationManager() {
        ;
    }
    bool Init(const string &configFilePath);
    static bool GetValue(const string &key, string &value);
private:
    static map<string, string> kvPairs;
};
} // namespace ConfigurationManager
#endif  // CHINA_MOBILE_SESSION_H
