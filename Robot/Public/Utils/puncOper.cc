#include "Robot/Public/Utils/puncOper.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Base/stringUtil.h"

DEFINE_string(global_punctuation_file, "../Data/QueryRewriter/QueryPreprocess/punctuation_utf8.txt", "punctuation file");

namespace robot {

bool PuncOper::Init() {
    if (!InitPuncSet(FLAGS_global_punctuation_file)) {
        VLOG(0) << "punc oper init failed, file: " << FLAGS_global_punctuation_file << endl;
        return false;
    }
    inited_ = true;
    return true;
}

bool PuncOper::IsPunc(const std::string& key) {
	return punc_set_.find(key) != punc_set_.end();
}

void PuncOper::ReplacePunc(const std::string& replacer, std::string* data) {
    if (!data) {
        return;
    }
    std::string new_query;
    std::vector<string> chars;
    common::CharSegment(*data, chars);
    bool replaced = true;
    for (size_t i = 0; i < chars.size(); ++i) {
        if (punc_set_.find(chars[i]) == punc_set_.end()) {
            new_query += chars[i];
            replaced = false;
        }
        else {
            if (!replaced) {
                new_query += replacer;
            }
            replaced = true;
        }
    }
    *data = new_query;
}

bool PuncOper::InitPuncSet(const std::string& punc_file) {
    file::SimpleLineReader reader(true);
    if (!reader.Open(punc_file)) {
        VLOG(0) << "punc oper open file failed: " << punc_file << endl;
        return false;
    }
    std::vector<std::string> lines;
    reader.ReadLines(&lines);
    for (size_t i = 0; i < lines.size(); ++i) {
        VLOG(7) << "punc line: " << lines[i];
        punc_set_.insert(lines[i]);
    }
    return true;
}

} // namespace robot
