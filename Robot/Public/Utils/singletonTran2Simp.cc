#include "Robot/Public/Utils/singletonTran2Simp.h"
#include "Common/Base/flags.h"
#include "Robot/Public/Utils/trioLog.h"
#include "Common/Base/stringUtil.h"
#include "Common/Util/File/simpleLineReader.h"

namespace robot {

Tran2Simp::Tran2Simp()
{
	inited_ = false;
}

Tran2Simp::~Tran2Simp()
{
	inited_ = false;
}

bool Tran2Simp::Init(const std::string& t2s_filename) {
	if (!inited_){
		inited_ = true;
        t2s_.clear();

        file::SimpleLineReader reader(true);
        CHECK(reader.Open(t2s_filename)) << "t2s open file failed: " << t2s_filename;
        vector<string> lines;
        reader.ReadLines(&lines);
        vector<string> strs;
        //int64_t trad, simp;
        for (size_t i = 0; i < lines.size(); ++i) {
            strs.clear();
            SplitStringDontTrim(lines[i], '\t', &strs);
            if (strs.size() != 2) {
                VLOG(TRIO_LOG_ERR) << "t2s line error size= " << strs.size() << " line=" << lines[i];
                continue;
            }
            if (strs[0].length() >= sizeof(int64_t) || strs[1].length() >= sizeof(int64_t)) {
                VLOG(TRIO_LOG_ERR) << "t2s char bytes exceed int64, " << strs[0].length() << " " << strs[1].length() << lines[i];
                continue;
            }
            t2s_[strs[0]] = strs[1];
            //trad = 0;
            //simp = 0;
            //memcpy((void*)&trad, strs[0].c_str(), strs[0].length());
            //memcpy((void*)&simp, strs[1].c_str(), strs[1].length());
            //t2s_[trad] = simp;
        }
        VLOG(TRIO_LOG_STAT) << "t2s size " << t2s_.size();
	}
	return true;
}

void Tran2Simp::Convert(const string& tran, string& simp){
    std::vector<std::string> words;
    common::CharSegment(tran, words);
    simp.clear();
    simp.reserve(tran.length() * 10);
    for (size_t i = 0; i < words.size(); ++i) {
        if (t2s_.find(words[i]) == t2s_.end()) {
            simp += words[i];
        }
        else {
            simp += t2s_[words[i]];
        }
    }
}

} // namespace robot
