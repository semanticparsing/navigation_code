#include "DTTManagerAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Registry.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/AllDialogAgents.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DiscourseAgents/AllDiscourseAgents.h"
//#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/TaskSpecification/AllTaskSpecifications.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"

namespace TrioTDM {

CDTTManagerAgent* CDTTManagerAgent::s_instance = NULL;

CDTTManagerAgent::CDTTManagerAgent(const std::string& name, const std::string& configuration, const std::string& type) :
    CAgent(name, configuration, type) {
}

CDTTManagerAgent::~CDTTManagerAgent() {
    for ( std::map<std::string, CDialogAgent*>::iterator iter = _session_2_root.begin(); iter != _session_2_root.end(); ++iter ) {
        if ( iter->second != NULL ) {
            iter->second->OnDestruction();
            delete iter->second;
        }
    }
    _session_2_root.clear();
}

CAgent* CDTTManagerAgent::AgentFactory(const std::string& name, const std::string& configuration) {
    return new CDTTManagerAgent(name, configuration);
}

CDTTManagerAgent* CDTTManagerAgent::Instance() {
    if ( s_instance == NULL && CRegistryAgentType::Instance() != NULL ) {
        s_instance = (CDTTManagerAgent*)CRegistryAgentType::Instance()->CreateAgent("CDTTManagerAgent", "DTTManagerAgent");
    }
    return s_instance;
}

void CDTTManagerAgent::Reset() {
    VLOG(FATAL) << __FUNCTION__ << ", Can not Reset.";
    exit(1);
}

void CDTTManagerAgent::Reset(const std::string& root_name) {
    VLOG(DATA) << __FUNCTION__;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    std::map<std::string, CDialogAgent*>::iterator f_iter = _session_2_root.find( root_name );
    if ( f_iter != _session_2_root.end() ) {
        if ( f_iter->second != NULL ) {
            f_iter->second->Reset();
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", f_iter->second is NULL, RootName: " << root_name;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", can not find task, RootName: " << root_name;
    }
}

void CDTTManagerAgent::Initialize() {
    // register all the agents for the dialog task
    CreateDialogTaskAgentome();
    for ( size_t i = 0;i < _da_infos.size(); ++i ) {
        (_da_infos[i].fRegisterAgent)();
    }
}

void CDTTManagerAgent::Use(const std::string& type, const std::string& name, FRegisterAgent funct, const std::string& configuration) {
    TDiscourseAgentInfo dai;
    dai.sDAName = name;
    dai.sDAType = type;
    dai.fRegisterAgent = funct;
    dai.sDAConfiguration = configuration;
    _da_infos.push_back(dai);
}

CDialogAgent* CDTTManagerAgent::CreateDialogTree(const std::string& root_name, CDMCoreAgent* core_agent) {
    VLOG(DATA) << __FUNCTION__ << ", Strting Dialog Tree Creation Phase ...";
    // create the actal task tree
    boost::recursive_mutex::scoped_lock lock(_mutex);
    CDialogAgent* root = CreateDialogTaskTree( root_name );
    root->SetOwnerCoreAgent( core_agent );
    root->Register();

    for ( size_t i = 0;i < _da_infos.size(); ++i ) {
        MountAgent(root, _da_infos[i].sDAType, 
                _da_infos[i].sDAName, _da_infos[i].sDAConfiguration, mmAsLastChild);
    }
    _session_2_root.insert(std::make_pair(root_name, root));
    return root;
}

void CDTTManagerAgent::DestroyDialogTree(const std::string& root_name) {
    VLOG(DATA) << __FUNCTION__ << ", RootName: " << root_name;
    boost::recursive_mutex::scoped_lock lock(_mutex);
    std::map<std::string, CDialogAgent*>::iterator f_iter = _session_2_root.find( root_name );
    if ( f_iter != _session_2_root.end() ) {
        if ( f_iter->second != NULL ) {
            f_iter->second->OnDestruction();
            delete f_iter->second;
        }
        _session_2_root.erase(f_iter);
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", DeleteTask, not find task, RootName: " << root_name;
    }
}

void CDTTManagerAgent::ReCreateDialogTree(const std::string& root_name, CDMCoreAgent* core_agent) {
    VLOG(DATA) << __FUNCTION__ << ", Starting Dialog Tree ReCreation Phase ...";
    DestroyDialogTree( root_name );
    CreateDialogTree( root_name, core_agent );
}

CDialogAgent* CDTTManagerAgent::GetDialogTaskTreeRoot(const std::string& root_name, CDMCoreAgent* core_agent) {
    boost::recursive_mutex::scoped_lock lock(_mutex);
    std::map<std::string, CDialogAgent*>::iterator f_iter = _session_2_root.find( root_name );
    
    if ( f_iter != _session_2_root.end() ) {
        return f_iter->second;
    } else {
        CDialogAgent* root = CreateDialogTree( root_name, core_agent );
        return root;
    }
}

void CDTTManagerAgent::MountAgent(CDialogAgent* where, CDialogAgent* who, TMountingMethod how, const std::string& dy_agent_id) {
    if ( !where || !who ) {
        VLOG(ERROR) << "where == NULL or who == NULL.";
        return ;
    }

    VLOG(DATA) << __FUNCTION__ << ", Who: " << who->GetName() << ", Where: " << where->GetName() << ", Method: " << how;

    switch ( how ) {
        case mmAsLastChild:
            where->AddSubAgent(who, NULL, asamAsLastChild);
            break;
        case mmAsFirstChild:
            where->AddSubAgent(who, NULL, asamAsFirstChild);
            break;
        case mmAsLeftSibling:
            if ( !where->GetParent() ) {
                VLOG(WARNING) << "Cannot mount, " << who->GetName();
                return ;
            }
            where->GetParent()->AddSubAgent(who, where, asamAsLeftSibling);
            break;
        case mmAsRightSibling:
            if ( !where->GetParent() ) {
                VLOG(WARNING) << "Cannot mount, " << who->GetName();
                return ;
            }
            where->GetParent()->AddSubAgent(who, where, asamAsRightSibling);
            break;
    }
    who->SetDynamicAgentID(dy_agent_id);
}

CDialogAgent* CDTTManagerAgent::MountAgent(CDialogAgent* where, const std::string& agent_type, 
        const std::string& agent_name, const std::string& agent_configuration, TMountingMethod how,
        const std::string& dy_agent_id) {

    if ( CRegistryAgentType::Instance() != NULL ) {
        CDialogAgent* nw_agent = (CDialogAgent*)CRegistryAgentType::Instance()->CreateAgent(agent_type, agent_name, agent_configuration);
        MountAgent(where, nw_agent, how, dy_agent_id);
        return nw_agent;
    } else {
        return NULL;
    }
}

void CDTTManagerAgent::UnMountAgent(CDialogAgent* who) {
    if ( who ) {
        if ( who->GetParent() ) {
            who->GetParent()->DeleteSubAgent(who);
        } else {
            VLOG(WARNING) << "Cannot UnMountAgent, who->GetParent() is NULL.";
        }
    } else {
        VLOG(WARNING) << "Cannot UnMountAgent, who is NULL.";
    }
}

void CDTTManagerAgent::MountAgentsFromArrayConcept(CDialogAgent* parent, const std::string& array_concept_name,
        const std::string& agents_type, const std::string& agents_name, 
        const std::string& agents_concept_name, const std::string& agents_dy_id) {
    for ( size_t i = 0;i < parent->C(array_concept_name).GetSize(); ++i ) {
        CDialogAgent* nw_agent = CDTTManagerAgent::Instance()->MountAgent(parent, agents_type, 
                ReplaceSubString(agents_name, "#", FormatString("%d", i)), "", mmAsLastChild);
        if ( agents_concept_name != "" ) {
            std::ostringstream ostr;
            ostr << array_concept_name << "." << i;
            nw_agent->C(agents_concept_name) = parent->C( ostr.str() );
        }
        nw_agent->SetDynamicAgentID(
                ReplaceSubString(agents_dy_id, "#", FormatString("%d", i)));
    }
}

std::string CDTTManagerAgent::GetParentName(const std::string& agent_path) {
    std::string parent, agent;
    SplitOnLast(agent_path, "/", parent, agent);
    return parent;
}

bool CDTTManagerAgent::IsParentOf(const std::string& parent_agent_path, const std::string& agent_path) {
    return (GetParentName(agent_path) == parent_agent_path);
}

bool CDTTManagerAgent::IsChildOf(const std::string& child_agent_path, const std::string& agent_path) {
    return (GetParentName(child_agent_path) == agent_path);
}

bool CDTTManagerAgent::IsAncestorOf(const std::string& ancestor_agent_path, const std::string& agent_path) {
    return (agent_path.substr(0, ancestor_agent_path.length()) == agent_path && 
            ancestor_agent_path != agent_path);
}

bool CDTTManagerAgent::IsAncestorOrEqualOf(const std::string& ancestor_agent_path, const std::string& agent_path) {
    return (agent_path.substr(0, ancestor_agent_path.length()) == ancestor_agent_path);
}

bool CDTTManagerAgent::IsDescendantOf(const std::string& descendant_agent_paht, const std::string& agent_path) {
    return IsAncestorOf(agent_path, descendant_agent_paht);
}

bool CDTTManagerAgent::IsSiblingOf(const std::string& agent_path1, const std::string& agent_path2) {
    return GetParentName(agent_path1) == GetParentName(agent_path2);
}

CDialogAgent* MountAgent(CDialogAgent* where, const std::string& agent_type, const std::string& agent_name,
        const std::string& agent_configuration, TMountingMethod how) {
    return CDTTManagerAgent::Instance()->MountAgent(where, agent_type, agent_name, agent_configuration, how);
}

void MountAgentsFromArrayConcept(CDialogAgent* parent, const std::string& array_concept_name, 
        const std::string& agent_type, const std::string& agent_name, const std::string& agents_concept_name) {
    CDTTManagerAgent::Instance()->MountAgentsFromArrayConcept(parent, array_concept_name, agent_type, agent_name, agents_concept_name);
}

}
