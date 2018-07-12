#include "OutputManagerAgent.h"

namespace TrioTDM {

COutputManagerAgent::COutputManagerAgent(const std::string& sAName,
										 const std::string& sAConfiguration, 
										 const std::string& sAType) : 
	CAgent(sAName, sAConfiguration, sAType) {
}

COutputManagerAgent::~COutputManagerAgent() {
    ;
}

CAgent* COutputManagerAgent::AgentFactory(const std::string& sAName, const std::string& sAConfiguration) {
	return new COutputManagerAgent(sAName, sAConfiguration);
}

void COutputManagerAgent::Reset() {
    _session2outpus.clear();
}

bool COutputManagerAgent::DeclareSaveData(std::map<std::string, std::string>& agents) {
    VLOG(DATA) << __FUNCTION__ << ", _session2outpus.size: " << _session2outpus.size();
    int output_num_limit = 0;
    if ( Config::Instance() != NULL ) {
        std::string tmp = Config::Instance()->get_value("task_dman/output_num_limit");
        output_num_limit = StringToInt(tmp);
    }
    Json::Value j_data(Json::arrayValue);
    int count = 0;
    for ( std::vector<COutput>::reverse_iterator riter = _session2outpus.rbegin(); 
            riter != _session2outpus.rend(); ++riter ) {
        count += 1;
        j_data.append( riter->ToJson() );
        if ( output_num_limit != 0 && count >= output_num_limit ) {
            break;
        }
    }
    Json::FastWriter fast_writer;
    agents[_name] = fast_writer.write( j_data );
    return true;
}

bool COutputManagerAgent::Load(const std::map<std::string, std::string>& agents) {

    std::map<std::string, std::string>::const_iterator c_iter = agents.find( _name );

    if ( c_iter == agents.end() ) {
        VLOG(WARNING) << __FUNCTION__ << ", Not Find Agent, AgentName: " << _name;
        return false;
    }

    Json::Reader reader;
    Json::Value j_data;

    if ( !reader.parse(c_iter->second, j_data) ) {
        VLOG(WARNING) << __FUNCTION__ << ", Parse Json Fail, AgentName: " << _name <<" ,strg: " << c_iter->second;
        return false;
    }

    if ( !j_data.isArray() ) {
        VLOG(WARNING) << __FUNCTION__ << ", JData is Not Array, AgentName: " << _name << ", strg: " << c_iter->second;
        return false;
    }
    _session2outpus.clear();
    _session2outpus.resize( j_data.size() );
    size_t idx = 0;
    
    for ( std::vector<COutput>::reverse_iterator riter = _session2outpus.rbegin(); 
            riter != _session2outpus.rend() && idx < j_data.size();
            ++riter, ++idx ) {
        riter->FromJson( j_data[idx] );
    }

    return true;
}


bool COutputManagerAgent::GetLastOutputs(const std::string& session, COutput& output) {
    if ( !_session2outpus.empty() ) {
        output = _session2outpus.back();
        return true;
    }
    return false;
}

bool COutputManagerAgent::GetLastHelpfulOutputs(const std::string& session, COutput& output) {
    for ( std::vector<COutput>::reverse_iterator riter = _session2outpus.rbegin();
            riter != _session2outpus.rend(); ++riter ) {
        if ( riter->GetIsHelpful() ) {
            output = *riter;
            return true;
        }
    }
    return false;
}

void COutputManagerAgent::AddOutputs(const std::string& session, const COutput& output) {
    _session2outpus.push_back( output );
}

}
