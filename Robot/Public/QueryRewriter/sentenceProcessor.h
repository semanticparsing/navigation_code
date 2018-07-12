#ifndef ROBOT_PUBLIC_QUERYREWRITER_SENTENCE_REWRITER_H
#define ROBOT_PUBLIC_QUERYREWRITER_SENTENCE_REWRITER_H

#include <map>
#include <string>
#include "Common/Base/hashTables.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"

namespace robot {

class QueryRewriter;
class SentenceProcessor {

public:
    SentenceProcessor();
    virtual ~SentenceProcessor();

    virtual bool Init(QueryRewriter* p);
    virtual void Run(const std::string& data, bool force_no_sep, RewriteQuery& rewrite_data);

private:
    bool InitStopSentenceSet(const std::string& stop_sentence_file);
    bool InitSubSentenceSepSet(const std::string& sub_sentence_sep_file);

    void SeparateSentence(const string& sentence, vector<string>& sub_sentences);
    void CalcSentenceWeight(const vector<string>& sub_sentences, vector<int>& weights);

private:
    common::hash_set<std::string> stop_sentence_set_;

    common::hash_set<std::string> sub_sentence_seps_set_;

    QueryRewriter* handler_;

    DISALLOW_COPY_AND_ASSIGN(SentenceProcessor);
};

} // namespace robot

#endif // ROBOT_PUBLIC_QUERYREWRITER_SENTENCE_REWRITER_H
