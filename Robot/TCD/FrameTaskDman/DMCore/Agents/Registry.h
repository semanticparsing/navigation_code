// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-07 17:30
// Description: 

#ifndef _REGISTRY_H__
#define _REGISTRY_H__

#include "RegistryAgent.h"
#include "RegistryAgentType.h"
/*
#include <boost/thread.hpp>
#include "Robot/TCD/FrameTaskDman/Utils/Common.h"

namespace TrioTDM {

class CAgent;

typedef CAgent* (*FCreateAgent)(const std::string&, const std::string&);

typedef void (*FRegisterAgent) (void);

class CRegistry {
public:
    CRegistry();

    void Clear();

    void RegisterAgent(const std::string& agent_name, CAgent* pagent);
    void UnRegisterAgent(const std::string& agent_name);
    bool IsRegisteredAgent(const std::string& agent_name);

    CAgent* operator[](const std::string& agent_name);

    void RegisterAgentType(const std::string& agent_type_name, FCreateAgent fct_create_agent);
    void UnRegisterAgentType(const std::string& agent_type_name);
    bool IsRegisteredAgentType(const std::string& agent_type);

    CAgent* CreateAgent(const std::string& agent_type_name, const std::string& agent_name, const std::string& agent_configuration = "");

    static CRegistry* Instance();

private:
    boost::recursive_mutex _mutex;
    std::map<std::string, CAgent*> _agents_hash;
    std::map<std::string, FCreateAgent> _agents_type_hash;
    static CRegistry* s_instance;
};

}
*/

#endif
