#include "Robot/Public/QueryRewriter/nicknameRemoveRewriter.h"
#include "Common/Base/flags.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Base/logging.h"
#include "Common/Base/stringUtil.h"

DEFINE_string(nickname_term_file, "", "");

namespace robot {

NicknameRemoveRewriter::NicknameRemoveRewriter(){
}

NicknameRemoveRewriter::~NicknameRemoveRewriter() {
}

bool NicknameRemoveRewriter::Init(QueryRewriter* p) {
    handler_ = p;
    file::SimpleLineReader reader(true);
    CHECK(reader.Open(FLAGS_nickname_term_file))
        << "nickname remove rewriter open file failed: " 
        << FLAGS_nickname_term_file << endl;
    vector<string> lines;
    reader.ReadLines(&lines);
    for (size_t i = 0; i < lines.size(); ++i) {
        production_filted_term_.insert(lines[i]);
    }
    return true;
}

void NicknameRemoveRewriter::Apply(std::string& data){ 
    common::hash_set<string>::const_iterator c_it;
    for (c_it = production_filted_term_.begin(); c_it != production_filted_term_.end(); ++c_it) {
        size_t pos = 0;
        while ((pos = data.find(*c_it)) != string::npos) {
            data.replace(pos, c_it->size(), "");
        }
    } 
}

REGISTER_REWRITER_PROCESSHANDLER(NicknameRemoveRewriter)
} // namespace robot
