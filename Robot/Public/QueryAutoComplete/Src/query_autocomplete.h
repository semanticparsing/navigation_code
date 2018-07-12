#ifndef ROBOT_QUERY_AUTOCOMPLETE_SRC_QUERY_AUTOCOMPLETE_H
#define ROBOT_QUERY_AUTOCOMPLETE_SRC_QUERY_AUTOCOMPLETE_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "Common/Base/hashTables.h"
#include "Common/Base/scopedPtr.h"
#include "Common/ThirdParty/re2/include/re2.h"

namespace trio {
    class WordSegTag;
}

namespace query_autocomplete {

enum SentenceType {
    AFFIRM = 1, //肯定
    INTERR = 2, //特殊疑问
    NORM_INTERR = 3, //一般疑问
    NEGA = 4, //否定
    SELECTED_INTERR = 5, //选择疑问
    NONE = 0,
};

enum CompleteType {
   UNKNOWN_COMPLETE = 0, // unknown type. No auto-completing.
   AFFIRM_COMPLETE = 1, // 当前query是肯定词，补全
   NEGA_COMPLETE = 2, // 当前query是否定词，补全
   JOINT_BEFORE_COMPLETE = 3, // 当前query + , + 上一轮reply
   JOINT_AFTER_COMPLETE = 4, // 上一轮reply + , + 当前query 
   REPLY_REPLACE_COMPLETE = 5, // 直接用上一轮reply替换当前query
   QUERY_REPLACE_COMPLETE = 6, // 直接用上一轮query替换当前query（该策略已去掉）
   ASK_COMPLETE = 7, // 直接用当前query拼上上一轮reply
   ASK_IN_REPLY_COMPLETE = 8, // 上一轮reply是疑问句，对其进行反问
   ADD_LAST_QUERY_COMPLETE = 9, // 对上一轮query进行补充
   SECOND_LAST_QUERY = 10, // 对上一轮query的问题进行追问
   JOINT_BEFORE_LASTQUERY_COMPLETE = 11, //当前query + ，+ 上一轮query
}; 

class QueryAutoComplete {
 public:
  QueryAutoComplete() {
  }
  virtual ~QueryAutoComplete() {
    if (segmenter_ != NULL) {
      delete segmenter_;
      segmenter_ = NULL;
    }
  }
  bool Init();
  CompleteType Run(const std::string& query,
          const std::string& last_query,
          const std::string& last_reply,
          std::string *complete_query);
  CompleteType Process(const std::string& query,
          const std::string& last_query,
          const std::string& last_reply,
          std::string *complete_query,
          int32& completion_src);
 
 private:
  bool ReadFileToSet(const std::string& file_path,
                     std::set<std::string>* term_set);
  SentenceType GetSentenceType(const std::string &sent);
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
  common::hash_map<std::string, std::pair<std::string, CompleteType> > complete_query_;
  scoped_ptr<re2::RE2> interr_sentence_re_;
  scoped_ptr<re2::RE2> norm_interr_sentence_re_;
  scoped_ptr<re2::RE2> selected_interr_sentence_re_;
  scoped_ptr<re2::RE2> nega_sentence_re_;
  trio::WordSegTag* segmenter_;
  std::set<std::string> nega_set_;
  std::set<std::string> normal_nega_set_;
  std::set<std::string> interr_badcase_set_;
  common::hash_map<std::string, double> hot_sentence_map_;
  common::hash_set<std::string> punc_set_;
  std::set<std::string> norm_interr_special_word_; 
  std::set<std::string> unpronoun_rev_set_;
};

} // query_autocomplete

#endif  // ROBOT_QUERY_AUTOCOMPLETE_SRC_QUERY_AUTOCOMPLETE_H
