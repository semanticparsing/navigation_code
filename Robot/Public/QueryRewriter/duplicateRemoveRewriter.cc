#include "Robot/Public/QueryRewriter/duplicateRemoveRewriter.h"
#include "Common/Base/flags.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Base/logging.h"
#include "Common/Base/stringUtil.h"

DEFINE_string(reduplication, "../QueryRewriter/QueryPreprocess/reduplication.utf8.dat", "");

namespace robot {

DuplicateRemoveRewriter::DuplicateRemoveRewriter(){
}

DuplicateRemoveRewriter::~DuplicateRemoveRewriter() {
}

bool DuplicateRemoveRewriter::Init(QueryRewriter* p) {
    handler_ = p;
    file::SimpleLineReader reader(true);
    CHECK(reader.Open(FLAGS_reduplication)) 
        << "sentence rewriter open file failed: " 
        << FLAGS_reduplication << endl;
    vector<string> lines;
    reader.ReadLines(&lines);
    vector<string> eles;
    for (size_t i = 0; i < lines.size(); ++i) {
        eles.clear();
        SplitString(lines[i], '\t', &eles);
        CHECK(eles.size() == 2) 
            << "reduplication_file line size error: " 
            << lines[i] << endl;
        reduplication_map_[eles[0]] = StringToInt(eles[1]);
    }
    return true;
}

void DuplicateRemoveRewriter::Apply(std::string& data){

    std::vector<std::string> chars;
    common::CharSegment(data, chars);

    std::vector<std::string> new_chars;
    int i = 0;
    while (i < chars.size()) {
        bool find = false;
        common::hash_map<string, int>::const_iterator c_it;
        c_it = reduplication_map_.find(chars[i]);
        if (c_it != reduplication_map_.end()) {
            find = true;
        }
        new_chars.push_back(chars[i]);
        ++i;
        if (find) {
            int count = 1;
            while (i < chars.size() && (c_it->first == chars[i] || chars[i] == " ")) {
                if (count < c_it->second) {
                    if (c_it->first == chars[i]) {
                        new_chars.push_back(chars[i]);
                        ++count;
                    }
                }
                ++i;
                if (i >= chars.size()) {
                    return;
                }
            }
            if (i > 0 && chars[i - 1] == " ") {
                new_chars.push_back(" ");
            }
            find = false;
        }
    }
    data = JoinString(new_chars, "");
}

REGISTER_REWRITER_PROCESSHANDLER(DuplicateRemoveRewriter)
} // namespace robot
