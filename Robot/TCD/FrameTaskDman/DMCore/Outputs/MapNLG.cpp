#include "MapNLG.h"
#include "Common/Util/File/simpleLineReader.h"

namespace TrioTDM {

CMapNLG::CMapNLG() {
    ;
}

CMapNLG::~CMapNLG() {
    ;
}

bool CMapNLG::Generate(const std::string& key, std::string& result) const {
    std::map<std::string, std::string>::const_iterator f_iter = _nlg_map.find(key);

    if ( f_iter != _nlg_map.end() ) {
        result = f_iter->second;
        return true;
    }
    return false;
}

bool CMapNLG::Load(const std::string& dit_file) {
    file::SimpleLineReader reader;
    
    if ( !reader.Open(dit_file) ) {
        VLOG(FATAL) << "CMapNLG Load can not read file, " << dit_file;
        return false;
    }
    
    std::vector<std::string> lines;
    std::string key;
    std::string sentence;

    reader.ReadLines(&lines);

    for ( size_t i = 0;i < lines.size(); ++i ) {
        std::string line = Trim(lines[i]);
        if ( line.empty() || line[0] == '#' || line[0] == '-') {
            continue;
        }
        if ( SplitOnFirst(lines[i], "\t", key, sentence) ) {
            if ( !((_nlg_map.insert(std::make_pair(key, sentence))).second) ) {
                VLOG(FATAL) << "key is exist, " << key;
                return false;
            }
        } else {
            VLOG(FATAL)  << "line format error, " << lines[i];
            return false;
        }
    }
    return true;
}

}
