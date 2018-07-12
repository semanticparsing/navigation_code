#include "Robot/Public/QueryRewriter/truncateRewriter.h"
#include <vector>
#include <string>
#include "Common/Base/flags.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Base/logging.h"
#include "Common/Base/stringUtil.h"
#include "Robot/Public/Declare/declearHead.h"

DEFINE_int32(truncate_word_size, 30, "max query size");

namespace robot {

TruncateRewriter::TruncateRewriter(){
}

TruncateRewriter::~TruncateRewriter() {
}

bool TruncateRewriter::InitVoid() {
	return true;
}

bool TruncateRewriter::Init(QueryRewriter* p) {
    handler_ = p;
    return true;
}

void TruncateRewriter::ApplySentence(robot::RewriteSentence& data){
    int max_words = FLAGS_truncate_word_size;
    int trim_words = data.basic_segs.size() - max_words;
    if (trim_words > 0) {
        std::vector<RewriteWord> tmp;
        for (size_t i=0; i<max_words; i++){
            tmp.push_back(data.basic_segs[trim_words+i]);
        }
        data.basic_segs = tmp;
    }
}

void TruncateRewriter::Apply(string& data) {
	vector<string> aps;
	SplitString(data, ' ', &aps);
	int max_words = FLAGS_truncate_word_size;
	int trim_words = aps.size() - max_words;
	if (trim_words > 0) {
		string tmp = "";
		string prefix = "";
		for (size_t i = 0; i < max_words; i++) {
			tmp += prefix + aps[trim_words + i];
			prefix = " ";
		}
		data = tmp;
	}
}

REGISTER_REWRITER_PROCESSHANDLER(TruncateRewriter)
} // namespace robot
