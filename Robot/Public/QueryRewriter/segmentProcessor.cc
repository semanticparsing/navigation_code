#include "Robot/Public/QueryRewriter/truncateRewriter.h"
#include <vector>
#include <string>
#include "Common/Base/flags.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Base/logging.h"
#include "Common/Base/stringUtil.h"
#include "Robot/Public/Declare/declearHead.h"
#include "Robot/Public/QueryRewriter/queryRewriter.h"

namespace robot {

SegmentProcessor::SegmentProcessor(){
}

SegmentProcessor::~SegmentProcessor() {
}

bool SegmentProcessor::Init(QueryRewriter* p) {
    handler_ = p;
    return true;
}

void SegmentProcessor::Run(robot::RewriteSentence& data){

    //tmp ignore weight

    std::vector<std::string> terms;
    SingletonWordSegTag::get()->Segment(data.senten, terms);

    std::vector<RewriteWord> words;
    RewriteWord word;
    for (size_t i=0; i<terms.size(); i++){
        word.__set_weight(1);
        word.__set_word(terms[i]);
        words.push_back(word);
    }
    data.__set_basic_segs(words);
    data.__set_phrase_segs(words);
}

void SegmentProcessor::ResegRun(robot::RewriteSentence& data) {

    //tmp ignore weight

    std::string seg_str = "";
    for (size_t i = 0; i < data.basic_segs.size(); i++) {
        seg_str += data.basic_segs[i].word;
    }
    data.senten = seg_str;

    std::vector<std::string> terms;
    SingletonWordSegTag::get()->Segment(data.senten, terms);

    std::vector<RewriteWord> words;

    RewriteWord word;
    for (size_t i = 0; i < terms.size(); i++) {
        word.__set_weight(1);
        word.__set_word(terms[i]);
        words.push_back(word);
    }
    data.__set_basic_segs(words);
    data.__set_phrase_segs(words);
}

} // namespace robot
