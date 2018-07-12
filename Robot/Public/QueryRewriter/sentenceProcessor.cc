#include "Robot/Public/QueryRewriter/sentenceProcessor.h"
#include <vector>
#include <string>
#include "Common/Base/flags.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Util/Encode/iconvWrapper.hpp"
#include "Common/Util/Encode/encodeUtil.h"
#include "Common/Base/logging.h"
#include "Common/Base/stringUtil.h"
#include "Robot/Public/Declare/declearHead.h"
#include "Common/ThirdParty/glog/logging.h"
#include "Robot/Public/Utils/trioLog.h"
#include "Robot/Public/QueryRewriter/queryRewriter.h"
#include "Robot/Public/Utils/puncOper.h"

DEFINE_string(qpp_stop_subsentences, "", "the stop subsentences file path");
DEFINE_string(qpp_sentence_separators, "", "the sentence separator file path");
DEFINE_bool(last_is_most_important, true, "last sentence is most important");

namespace robot {

SentenceProcessor::SentenceProcessor(){
}

SentenceProcessor::~SentenceProcessor() {
}

bool SentenceProcessor::Init(QueryRewriter* p) {
    handler_ = p;
    CHECK(InitStopSentenceSet(FLAGS_qpp_stop_subsentences)) 
        << "read stop subsentence failed!" << endl;
    CHECK(InitSubSentenceSepSet(FLAGS_qpp_sentence_separators)) 
        << "read sentence separator failed!" << endl;
    CHECK(SingletonPuncOper::get()) << "singleton punc oper null";
    CHECK(SingletonPuncOper::get()->Init()) << "singleton punc oper init fail";
    return true;
}

bool SentenceProcessor::InitStopSentenceSet(const std::string& stop_sentence_file) {
    if (stop_sentence_file.empty()) {
        return false;
    }
    file::SimpleLineReader reader(stop_sentence_file, true);
    std::vector<std::string> lines;
    reader.ReadLines(&lines);
    std::string trim_line;
    std::string basic_seg;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (StartsWithASCII(lines[i], "#", false)) {
            continue;
        }
        trim_line.clear();
        TrimWhitespace(lines[i], TRIM_ALL, &trim_line);
        stop_sentence_set_.insert(trim_line);
        //SingletonWordSegTag::get()->Segment(trim_line, basic_seg);
        //VLOG(TRIO_LOG_CUSTOM+1) << "stop sentence: " << lines[i] 
        //    << ", basic_seg: " << basic_seg << endl;
        //stop_sentence_set_.insert(basic_seg);
    }
    return true;
}

bool SentenceProcessor::InitSubSentenceSepSet(const std::string& sub_sentence_sep_file) {
    if (sub_sentence_sep_file.empty()) {
        return false;
    }
    file::SimpleLineReader reader(sub_sentence_sep_file, true);
    std::vector<std::string> lines;
    reader.ReadLines(&lines);
    for (size_t i = 0; i < lines.size(); ++i) {
        if (StartsWithASCII(lines[i], "#", false)) {
            continue;
        }
        sub_sentence_seps_set_.insert(lines[i]);
    }
    return true;
}

void SentenceProcessor::Run(const std::string& data, bool force_no_sep, RewriteQuery& rewrite_data) {

    std::string terms_str = data;

    vector<string> sub_sentences;
    if (force_no_sep){
        sub_sentences.push_back(terms_str);
    }
    else {
        SeparateSentence(terms_str, sub_sentences);
    }

    vector<string> new_sub_sentences;
    std::string trim_senten;
    for (size_t i = 0; i < sub_sentences.size(); i++) {
        trim_senten.clear();
        TrimWhitespace(sub_sentences[i], TRIM_ALL, &trim_senten);
        if (!trim_senten.empty()) {
            new_sub_sentences.push_back(trim_senten);
        }
    }

    std::vector<int> weights;
    CalcSentenceWeight(new_sub_sentences, weights);

    std::vector<robot::RewriteSentence> sentens;
    for (size_t i=0; i<new_sub_sentences.size(); i++){
        robot::RewriteSentence senten;
        senten.__set_senten(new_sub_sentences[i]);
        senten.__set_weight(weights[i]);
        sentens.push_back(senten);
    }
    rewrite_data.__set_sentences(sentens);
}

void SentenceProcessor::SeparateSentence(const string& sentence, vector<string>& sub_sentences) {
    std::vector<std::string> chars;
    common::CharSegment(sentence, chars);
    std::string sub_sentence;
    for (size_t k = 0; k < chars.size(); k++) {
        if (sub_sentence_seps_set_.find(chars[k]) != sub_sentence_seps_set_.end()) {
            if (!sub_sentence.empty()) {
                sub_sentences.push_back(sub_sentence);
                sub_sentence.clear();
            }
        } else {
            sub_sentence += chars[k];
        }
    }
    if (!sub_sentence.empty()) {
        sub_sentences.push_back(sub_sentence);
    }
}

void SentenceProcessor::CalcSentenceWeight(const vector<string>& sub_sentences, vector<int>& weights) {
    int last_non_stop_index = -1;
    weights.clear();
    std::string sub_senten;
    for (size_t i = 0; i < sub_sentences.size(); i++){
        sub_senten = sub_sentences[i];
        SingletonPuncOper::get()->ReplacePunc("", &sub_senten);
        if (stop_sentence_set_.find(sub_senten) == stop_sentence_set_.end()) {
            weights.push_back(100);//initial all important
            last_non_stop_index = i;
        }
        else {
            weights.push_back(0);
        }
    }
    //set non last to not important 0
    if (FLAGS_last_is_most_important) {
        if (last_non_stop_index != -1){
            for (int i=last_non_stop_index-1; i>=0; i--){
                weights[i] = 0;
            }
        }
    }
    //all stop sentence, then set all to 100
    if (last_non_stop_index == -1){
        for (size_t i=0; i<weights.size(); i++){
            weights[i] = 100;
        }
    }
}
} // namespace robot
