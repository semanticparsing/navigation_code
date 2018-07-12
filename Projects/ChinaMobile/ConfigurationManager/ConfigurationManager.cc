#include "ConfigurationManager.h"
#include "Common/Base/stringUtil.h"
#include <fstream> 

using namespace std;

namespace ConfigurationManager {

map<string, string> ConfigurationManager::kvPairs;

bool ConfigurationManager::Init(const string &configFilePath)
{
    ifstream infile;
    infile.open(configFilePath.c_str());
    if (!infile) {
        VLOG(0) << " can not open file:" << configFilePath;
        return false;
    }
    string line;
    while (getline(infile, line, '\n')) {
        vector<string> kvPair;
        SplitString(line, '=', &kvPair);
        if(kvPair.size() == 2)
        {
            string key = "";
            string value = "";
            char trimChars[3]={ '\r','\n',' '};
            TrimString(kvPair[0], trimChars, &key);
            TrimString(kvPair[1], trimChars, &value);
            if (key != "" && value != "")
            {
                kvPairs[key] = value;
                VLOG(3) << "ConfigurationManager: " + key + " = " + value;
            }
        }
    }
    infile.close();
    return true;
}
bool ConfigurationManager::GetValue(const string &key, string &value)
{
    if((int)(kvPairs.count(key)) > 0)
    {
        value = kvPairs[key];
        return true;
    }
    VLOG(1) << "ConfigurationManager: " + key + " is not existed.";
    return false;
}
}
