#ifndef ROBOT_PUBLIC_QUERYREWRITER_AUTOCOMPLETE_PROCESSOR_H
#define ROBOT_PUBLIC_QUERYREWRITER_AUTOCOMPLETE_PROCESSOR_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "Common/Base/hashTables.h"
#include "Common/Base/scopedPtr.h"
#include "Common/ThirdParty/re2/include/re2.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"
#include "Robot/Public/QueryAutoComplete/Src/query_autocomplete.h"

using namespace query_autocomplete;

namespace robot {

class QueryRewriter;
class AutoCompleteProcessor {
 public:
  AutoCompleteProcessor() {
  }
  virtual ~AutoCompleteProcessor() {
  }
  virtual bool Init(QueryRewriter* p);

  query_autocomplete::CompleteType Run(const std::string& query,
          const std::string& last_query,
          const std::string& last_reply,
          std::string *complete_query);

private:
    query_autocomplete::CompleteType Process(const std::string& query,
          const std::string& last_query,
          const std::string& last_reply,
          std::string *complete_query,
          int32& completion_src);
 
 private:
  bool ReadFileToSet(const std::string& file_path,
                     std::set<std::string>* term_set);
  query_autocomplete::SentenceType GetSentenceType(const std::string &sent);
  void SeperateSentences(const std::string &text,
                         std::vector<std::pair<std::string, std::string> >* sents);
  void PostProcess(const std::string &sentence, std::string* sentence_post);
  void PronounReverse(const std::string &sentence, std::string* sentence_reverse);
  bool AffirmComplete(const std::string& query,
                      const std::string& last_reply,
                      std::string* complete_query);
  bool NegaComplete(const std::string& query,
                    const std::string& last_reply,
                    std::string* complete_query);

  bool JoinQueryReplyComplete(const std::string& query,
                              const std::string& last_reply,
                              std::string* complete_query,
                              bool query_is_front);

  bool JoinQueryQueryComplete(const std::string& query,
                              const std::string& last_query,
                              std::string* complete_query, bool rev = false);

  bool GetReFromFile(const std::string& re_file,
                     std::string* re_expression);
  bool GetRe2FromFile(const std::string& re_file,
                      std::string* re_expression);
  int32 IsNormInterrSentence(const std::string& sent, int32& len);
  void RtrimString(const std::string& str, std::string* trim_str);
  bool IsPuncEnd(const std::string& sent);
  void PopPunc(std::string& sent, const std::set<std::string> &punc);
  void ReplacePunc(std::string& sent, const std::string &spunc, const std::string &dpunc); 
  bool ContainQuery(const std::string& query, const std::string& reply);
  void GetKeySentence(const std::string &text, std::string* sentence);
 private:
  std::set<std::string> seperator_set_;
  std::set<std::string> rtrim_set_;
  common::hash_set<std::string> affirm_set_;
  common::hash_set<std::string> filter_word_;
  common::hash_map<std::string, std::pair<std::string, query_autocomplete::CompleteType> > complete_query_;
  scoped_ptr<re2::RE2> interr_sentence_re_;
  scoped_ptr<re2::RE2> norm_interr_sentence_re_;
  scoped_ptr<re2::RE2> selected_interr_sentence_re_;
  scoped_ptr<re2::RE2> nega_sentence_re_;
  std::set<std::string> nega_set_;
  std::set<std::string> normal_nega_set_;
  std::set<std::string> interr_badcase_set_;
  common::hash_map<std::string, double> hot_sentence_map_;
  common::hash_set<std::string> punc_set_;
  std::set<std::string> norm_interr_special_word_; 
  std::set<std::string> unpronoun_rev_set_;
  QueryRewriter* handler_;
};

} //robot

#endif  // ROBOT_PUBLIC_QUERYREWRITER_AUTOCOMPLETE_PROCESSOR_H
