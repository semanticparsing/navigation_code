
#ifndef ROBOT_QUERY_PROCESS_HANDLER_REMOVE_STOP_WORD_HANDLER_H
#define ROBOT_QUERY_PROCESS_HANDLER_REMOVE_STOP_WORD_HANDLER_H

#include <map>
#include <string>
#include "Common/Base/hashTables.h"
#include "Common/Util/Thread/threadPool.h"
#include "Common/ThirdParty/segmenter/SegWrapper/segWrapper.h"
#include "Common/Util/Trie/jumpTrie.h"
// #include "common/jump_trie/jump_trie.h"
// #include "trio/wordseg.h"
#include "docProcessHandler.h"

namespace preprocess {
class RemoveStopWordHandler : public DocProcessHandler {
 public:
  RemoveStopWordHandler():init_ok_(false){}
  virtual ~RemoveStopWordHandler();

  bool Init();
  bool ProcessDoc(QueryInfo& query_info);

  bool InitBuf(size_t pool_size) {
    for (size_t idx = 0; idx < pool_size; ++idx) {
      scw_out_t* p_scw_out = NULL;
      p_scw_out = scw_create_out(10000, SCW_OUT_ALL | SCW_OUT_PROP);
      if (p_scw_out == NULL) {
        VLOG(1) << "Failed to creat wordseg handler";
        return false;
      }
      ptr_scw_buf_pool_->Put(p_scw_out);
    }
    return true;
  }


  bool ClearBuf(size_t pool_size) {
    LOG(INFO) << "clear";
    for (size_t idx = 0; idx < pool_size; ++idx) {
      scw_out_t* p_scw_out = ptr_scw_buf_pool_->Get();
      LOG(INFO) << "delete,"<< idx;
      if (p_scw_out != NULL) {
        scw_destroy_out(p_scw_out);
      } else {
        VLOG(1) << "get failed.";
      }
    }
    LOG(INFO) << "clear done";
    return true;
  }
 private:
  string FindVitalSubsentence(const vector<string>& sub_query_vec);
  string OnlyRemoveStopSentence(const vector<string>& sub_query_vec);
  void SeparateSentence(const string& sentence, vector<string>* sub_sentences);
  bool ReadFileToSet(const std::string& file_path, common::hash_set<std::string>* str_set);
  bool ReadFileToMap(const std::string& file_name, common::hash_map<std::string, int>* res_map);
  bool BuildSynonymsTermTrie(const string& file_path, trie::KvJumpTrie<string>& jump_trie);
  bool BuildEmojiTrie(const string& file_path, trie::KvJumpTrie<string>& jump_trie);
  bool ReplaceSynonymsStrByTrie(std::string& query);
  bool TransferSynonymsStrByTrie(std::string& query);
  string ReplaceSep(const std::string& str, char src_sep, const std::string& des_sep);
  bool TruncateLongQuery(const std::string& raw_query, string* query);
  void Deduplicate(const vector<string>& raw_query_vec, vector<string>* query);
  bool MatchPeopleInfo(const string& query, QueryInfo* query_info);
  bool ReadFileToSeWordgSet(const std::string& file_path, common::hash_set<std::string>* str_set);
  bool DelProductionFilterTerms(std::string& query);
  bool TruncateLongQuery(const string &raw_query, string &query);
  string SepcialJoinString(vector<string> words, const string& des_sep);
  bool ReadPinYinToMap(const std::string& file_name, common::hash_map<string, string>* pinyin_map);
  bool ConvertQuery2Pinyin(QueryInfo* query_info);
  std::string RemovePunctuation(const std::string& query);
  std::string seg_pinyin(string pinyin_str);
  string RemoveEmoji(const string& query);

 private:
  common::hash_set<std::string> stop_sentence_set_;
  common::hash_set<std::string> punc_set_;
  common::hash_set<std::string> sep_set_;
  common::hash_set<std::string> production_filted_term_;
  common::hash_set<std::string> pinyin_set_;
  common::hash_map<string, int> reduplication_map_;
  trie::KvJumpTrie<string> synonyms_term_trie_;
  trie::KvJumpTrie<string> synonyms_transfer_trie_;
  trie::KvJumpTrie<string> people_info_trie_;
  trie::KvJumpTrie<string> emoji_trie_;
  seg_wrapper::wordSeg wordSegInstance_;
  common::ProducerConsumerQueue<scw_out_t>* ptr_scw_buf_pool_;
  common::hash_map<string, string> pinyin_map_;
  bool init_ok_;



  DISALLOW_COPY_AND_ASSIGN(RemoveStopWordHandler);
};

} // namespace preprocess

#endif // ROBOT_QUERY_PROCESS_HANDLER_REMOVE_STOP_WORD_HANDLER_H
