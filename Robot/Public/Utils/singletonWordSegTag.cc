#include "Robot/Public/Utils/singletonWordSegTag.h"
#include "Common/Base/flags.h"
#include "Robot/Public/Utils/trioLog.h"
#include "Common/Base/stringUtil.h"

DEFINE_string(wordsegtag_data_dir, "", "");
DEFINE_string(wordsegtag_punc_file, "", "");
DEFINE_string(wordsegtag_userdict_file, "", "");

namespace robot {

WordSegPuncTagging::WordSegPuncTagging()
{
	wordseg_tag_ = NULL;
	inited_ = false;
  split_sentence_ = false;
}

WordSegPuncTagging::~WordSegPuncTagging()
{
	inited_ = false;
	SAFE_DELETE(wordseg_tag_);
}

bool WordSegPuncTagging::Init() {
	if (!inited_){
		inited_ = true;
		wordseg_tag_ = new trio::WordSegTag(FLAGS_wordsegtag_data_dir);
		CHECK(NULL != wordseg_tag_) << "new word seg tag failed" << endl;

        bool useT2S = true;
        bool useFilter = false;
        string userDictName = "";
        int bufSize = 100;

		CHECK(wordseg_tag_->Init(useT2S, useFilter, userDictName, FLAGS_wordsegtag_punc_file, bufSize)) 
            << "word seg tag singleton init failed" << endl;
	}
	return true;
}

bool WordSegPuncTagging::InitWithUserDict() {
	if (!inited_){
		inited_ = true;
		wordseg_tag_ = new trio::WordSegTag(FLAGS_wordsegtag_data_dir);
		CHECK(NULL != wordseg_tag_) << "new word seg tag failed" << endl;

        bool useT2S = true;
        bool useFilter = false;
        int bufSize = 100;

		CHECK(wordseg_tag_->Init(FLAGS_wordsegtag_userdict_file, useT2S, useFilter, "", FLAGS_wordsegtag_punc_file, bufSize)) 
            << "word seg tag singleton init failed" << endl;
    // split_sentence must be true when preUserDict, or invalid
    split_sentence_ = true;
    VLOG(0) << "init user dict " << FLAGS_wordsegtag_userdict_file;
	}
	return true;
}

bool WordSegPuncTagging::Segment(const string& query, string &terms) {
    terms.clear();
    vector<string> vec_terms;
    if (!Segment(query, vec_terms)) {
        return false;
    }
    terms = JoinVector(vec_terms, ' ');
    return true;
}

bool WordSegPuncTagging::Segment(const string& query, vector<string> &terms) {
    terms.clear();
    std::vector<std::string> pos_lst;
    bool removePunc = true;
    bool splitSentence = split_sentence_;
    if (!wordseg_tag_->SegmentTagging(query, terms, pos_lst, removePunc, splitSentence)) {
        VLOG(TRIO_LOG_ERR) << "WordSegPuncTagging failed " << query << endl;
        return false;
    }
    return true;
}           

bool WordSegPuncTagging::SegmentWithTag(const string& query, string &terms) {
    terms.clear();
    bool removePunc = true;
    bool splitSentence = true;
    if (!wordseg_tag_->SegmentTagging(query, terms, removePunc, splitSentence)) {
        VLOG(TRIO_LOG_ERR) << "WordSegPuncTagging failed " << query << endl;
        return false;
    }
    return true;
}

} // namespace robot
