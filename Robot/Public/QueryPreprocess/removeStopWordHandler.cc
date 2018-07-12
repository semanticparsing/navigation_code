#include "removeStopWordHandler.h"

#include <vector>
#include <string>
#include "Common/Base/flags.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Util/Encode/iconvWrapper.hpp"
#include "Common/Util/Encode/encodeUtil.h"
#include "Common/Base/logging.h"
#include "Common/Base/stringUtil.h"
#include "Robot/Public/Declare/declearHead.h"

DEFINE_string(qpp_stop_subsentences, "../data/qpp_stop_subsentences.dat", "the stop subsentences file path");
DEFINE_string(qpp_sentence_separators, "../data/sentence_sepatators.dat", "the sentence separator file path");
DEFINE_string(qpp_synonyms_transfer, "../data/qpp_synonyms_transfer.dat", "the synonyms transfer file path");
DEFINE_string(qpp_synonyms_term, "../data/qpp_synonyms_term.dat", "the synonyms term file path");
DEFINE_string(qpp_production_filted_term, "../data/qpp_production_filted_term.dat", "each production need filter term GBK");
DEFINE_string(reduplication, "../data/reduplication.utf8.dat", "");
DEFINE_bool(match_people, false, "match famous people");
DEFINE_string(people_info_file, "../data/people_info.txt", "people info file");
DEFINE_string(scw_conf, "scw.conf", "");
DEFINE_string(worddict, "../data", "");
DEFINE_bool(last_is_most_important, true, "last sentence is most important");
DEFINE_int32(thread_pool_size, 1, "");
DEFINE_string(punctuation_file, "../Data/QueryPreprocess/punctuation_utf8.txt", "");

#define BYTESIZE 1024
using std::string;
using std::vector;
using common::Iconv::utf8_to_gbk;
using common::Iconv::easy_conv;
using encode::GBK2UTF8;
using encode::UTF82GBK;

namespace preprocess {


RemoveStopWordHandler::~RemoveStopWordHandler() {
  if (init_ok_) {
    ClearBuf(FLAGS_thread_pool_size);
  }
}

bool RemoveStopWordHandler::Init() {
  // int64_t t1_1 = common::GetTimeInUsec();
	wordSegInstance_.Init(FLAGS_scw_conf, FLAGS_worddict);

  if (!ReadFileToSeWordgSet(FLAGS_qpp_stop_subsentences, &stop_sentence_set_)) {
    VLOG(2) << "Read stop subsentence failed!";
    return false;
  }

  if (!ReadFileToSet(FLAGS_qpp_sentence_separators, &sep_set_)) {
    VLOG(2) << "Read sentence separator failed!";
    return false;
  }

  if (!ReadFileToSet(FLAGS_qpp_production_filted_term, &production_filted_term_)) {
    VLOG(2) << "Read production filter items failed!";
    return false;
  }

  if (!BuildSynonymsTermTrie(FLAGS_qpp_synonyms_term, synonyms_term_trie_)) {
    VLOG(2) << "Build synonyms term trie failed!";
    return false;
  }

  if (!BuildSynonymsTermTrie(FLAGS_qpp_synonyms_transfer, synonyms_transfer_trie_)) {
    VLOG(2) << "Build synonyms transfer trie failed!";
    return false;
  }

  if (!ReadFileToMap(FLAGS_reduplication, &reduplication_map_)) {
    VLOG(2) << "Read reduplication failed!";
    return false;
  }

  if (!BuildSynonymsTermTrie(FLAGS_people_info_file, people_info_trie_)) {
    VLOG(2) << "build people info trie failed!";
    return false;
  }

  if (!ReadPinYinToMap(FLAGS_global_pinyin_file, &pinyin_map_)) {
    VLOG(2) << "read pinyin file failed!";
    return false;
  }

  if (!ReadFileToSet(FLAGS_punctuation_file, &punc_set_)) {
    VLOG(2) << "Read sentence separator failed!";
    return false;
  }

  CHECK(BuildEmojiTrie(FLAGS_global_emoji_file, emoji_trie_)) << "Build emoji trie failed";

  //int64_t t1_2 = common::GetTimeInUsec();
  //VLOG(2) << "Init cost time:" << t1_2 - t1_1;
  ptr_scw_buf_pool_ = new common::ProducerConsumerQueue<scw_out_t>(FLAGS_thread_pool_size);
  if (!InitBuf(FLAGS_thread_pool_size)) {
    VLOG(2) << "Init buf failed";
    return false;
  }
  init_ok_ = true;
  return true;
}

bool RemoveStopWordHandler::ProcessDoc(QueryInfo &query_info) {
  ConvertQuery2Pinyin(&query_info);
  query_info.is_hit_people = false;
  query_info.people_info = "";
  string raw_query;
  UTF82GBK(query_info.query, raw_query);
  // string raw_query = easy_conv(utf8_to_gbk, query_info.query);

  // word segment
  string basic_seg;
  string phrase_seg;
  CHECK(ptr_scw_buf_pool_ != NULL) << "ptr_scw_buf_pool_ is NULL";
  scw_out_t* p_scw_out = ptr_scw_buf_pool_->Get();
  if (p_scw_out == NULL) {
    ptr_scw_buf_pool_->Put(p_scw_out);
    return false;
  }
  //wordSegInstance_.wordseg(p_scw_out, raw_query, basic_seg, phrase_seg);
  wordSegInstance_.wordseg(raw_query, basic_seg, phrase_seg);
  query_info.basic_seg = basic_seg;
  query_info.phrase_seg = phrase_seg;

  VLOG(5) << "raw_query:" << raw_query;
  ReplaceSubstringsAfterOffset(&raw_query, 0, " ", "\001");
  VLOG(5) << "raw_query:" << raw_query;
  // delelte terms by production
  // int64_t t2_1 = common::GetTimeInUsec();
  DelProductionFilterTerms(raw_query);
  // int64_t t2_2 = common::GetTimeInUsec();
  // VLOG(2) << "DelProductionFilterTerms cost time:" << t2_2 - t2_1;

  string tmp_query = "";
  TruncateLongQuery(raw_query, tmp_query);
  raw_query =  tmp_query;
  // int64_t t2_3 = common::GetTimeInUsec();
  // VLOG(2) << "wordseg cost time:" << t2_3 - t2_2;

  // gbk to utf8
  VLOG(5) << "basic_seg:" << basic_seg;
  char buf[10240] = {0};
  int utf8Len = ul_gbk_to_utf8(buf, 10240, basic_seg.c_str());
  if (utf8Len <= 0) {
    VLOG(2) << "gbk to utf8 false";
    ptr_scw_buf_pool_->Put(p_scw_out);

    return false;
  }
  std::string query_utf8 = buf;
  VLOG(5) << "query utf8:" << query_utf8;
  //int64_t t2_4 = common::GetTimeInUsec();
  //VLOG(2) << "ul_gbk_to_utf8:" << t2_4 - t2_3;

  // full width convert to half width
  std::string half_wid_query;
  common::FullWidthToHalfWidth(query_utf8, &half_wid_query);
  //VLOG(5) << "half wid query:" << half_wid_query;
  //int64_t t2_5 = common::GetTimeInUsec();
  //VLOG(2) << "FullWidthToHalfWidth cost time:" << t2_5 - t2_4;

  // convert to lower
  StringToLowerASCII(&half_wid_query);
  VLOG(5) << "lower query:" << half_wid_query;
  std::vector<std::string> word_vec;
  //int64_t t2_6 = common::GetTimeInUsec();
  //VLOG(2) << "StringToLowerASCII cost time:" << t2_6 - t2_5;

  // add leading and trailing space
  string people_sub_query = " " + half_wid_query;
  people_sub_query += " ";

  // match people info
  if (FLAGS_match_people) {
    if (MatchPeopleInfo(people_sub_query, &query_info)) {
      VLOG(5) << "Match people";
      ptr_scw_buf_pool_->Put(p_scw_out);

      return true;
    }
  }
  // int64_t t2_7 = common::GetTimeInUsec();
  // VLOG(2) << "MatchPeopleInfo cost time:" << t2_7 - t2_6;
  // separate query to subquerys
  vector<string> sub_querys;
  SeparateSentence(half_wid_query, &sub_querys);
 // int64_t t2_8 = common::GetTimeInUsec();
  //VLOG(2) << "SeparateSentence cost time:" << t2_8 - t2_7;

  // find vital subsentence from query
  string vital_sub_query;
  if (FLAGS_last_is_most_important) {
    vital_sub_query = FindVitalSubsentence(sub_querys);
  } else {
    vital_sub_query = OnlyRemoveStopSentence(sub_querys);
  }
  if (vital_sub_query.empty()) {
    VLOG(5) << "vital_sub_query is empty";
    string prefix = "";
    for (size_t i = 0; i < sub_querys.size(); ++i) {
      vital_sub_query = prefix + sub_querys[i];
      prefix = " ";
    }
  }
  //int64_t t2_9 = common::GetTimeInUsec();
  //VLOG(2) << "find vital sentence cost time:" << t2_9 - t2_8;

  // add leading and trailing space
  vital_sub_query = " " + vital_sub_query;
  vital_sub_query += " ";

  VLOG(5) << "vital_sub_query:" << vital_sub_query;
  // relpace synonyms term: xxx -> yyy
  ReplaceSynonymsStrByTrie(vital_sub_query);
  // int64_t t2_10 = common::GetTimeInUsec();
  // VLOG(2) << "ReplaceSynonymsStrByTrie cost time:" << t2_10 - t2_9;
  VLOG(5) << "replace synonyms vital_sub_query:" << vital_sub_query;
  // transferm synonyms term: xxx -> yyy + zzz
  TransferSynonymsStrByTrie(vital_sub_query);
  VLOG(5) << "transfer synonyms vital sub query:" << vital_sub_query;
  // int64_t t2_11 = common::GetTimeInUsec();
  // VLOG(2) << "TransferSynonymsStrByTrie cost time:" << t2_11 - t2_10;


  // basic_seg to new query
  string query = ReplaceSep(vital_sub_query, ' ', "");
  VLOG(5) << "ReplaceSep:" << query;
  // int64_t t2_12 = common::GetTimeInUsec();
  // VLOG(2) << "ReplaceSep cost time:" << t2_12 - t2_11;

  // utf to gbk and word segment
  query = RemovePunctuation(query);

  if (!query.empty()) {
    char buf1[BYTESIZE] = {0};
    int gbkLen = ul_utf8_to_gbk(buf1, BYTESIZE, query.c_str());
    if (gbkLen <= 0) {
      ptr_scw_buf_pool_->Put(p_scw_out);

      VLOG(2) << "utf8 to gbk false";
      return false;
    }
    query = buf1;
  } else {
    query = raw_query;
  }
  query_info.preprocessed_query = query;
  // int64_t t2_13 = common::GetTimeInUsec();
  // VLOG(2) << "ul_utf8_to_gbk cost time:" << t2_13 - t2_12;
  phrase_seg = "";
  wordSegInstance_.wordseg(p_scw_out, query, basic_seg, phrase_seg);
  ptr_scw_buf_pool_->Put(p_scw_out);
  //wordSegInstance_.wordseg(query, basic_seg, phrase_seg);
  query_info.preprocessed_basic_seg = basic_seg;
  query_info.preprocessed_phrase_seg = phrase_seg;
  VLOG(5) << "raw query:" << query_info.query << "\tbasic_seg:" << query_info.basic_seg << "\tphrase_seg:" << query_info.phrase_seg
          <<  "\tquery_info.preprocessed_query" << query_info.preprocessed_query << "\tbasic_seg:" << query_info.preprocessed_basic_seg << "\tquery_info.preprocessed_phrase_seg:" << query_info.preprocessed_phrase_seg;

  // int64_t t2_14 = common::GetTimeInUsec();
  // VLOG(2) << "wordseg cost time:" << t2_14 - t2_13;
  return true;
}

/* delete some unnecessary word from query by each production
 * input: query
 * the word need delete saved in a set: production_filted_term_
 */
bool RemoveStopWordHandler::DelProductionFilterTerms(std::string& query) {
  common::hash_set<string>::const_iterator c_it;
  for (c_it = production_filted_term_.begin(); c_it != production_filted_term_.end(); ++c_it) {
    size_t pos = 0;
    while ((pos = query.find(*c_it)) != string::npos) {
      query.replace(pos, c_it->size(), "");
    }
  }
  return true;
}

/* match famous people info
 * input: query and query_info
 * use KVJumpTrie to save and match famous people info: key is people name, value is people description
 * if query match a people then add people_info(a field) to query_info
 */
bool RemoveStopWordHandler::MatchPeopleInfo(const string& query, QueryInfo* query_info) {
  std::vector<std::pair<const std::string*, const std::string*> > people_infos;
  people_info_trie_.LongestMatch(query, &people_infos);
  if (people_infos.size() > 0) {
    query_info->is_hit_people = true;
    string people_info("");
    string sperate = "";
    for (size_t i = 0; i < people_infos.size(); ++i) {
      people_info += sperate + *(people_infos[i].second);
      sperate = "\n";
    }
    char buf_gbk[10240] = {0};
    int gbkLen = ul_utf8_to_gbk(buf_gbk, 10240, people_info.c_str());
    if (gbkLen > 0) {
      query_info->people_info = buf_gbk;
      return true;
    }
  }
  return false;
}

/* separate a sentence to a word vector, and speparate word vector to subsentence by punctuation and also do deduplicate
 * input: sentence and sub_sentences(output as well)
 * punctuation save in a set sep_set_
 */
void RemoveStopWordHandler::SeparateSentence(const string& sentence, vector<string>* sub_sentences) {
  vector<string> word_vec_mid;
  vector<string> word_vec;
  // separate to word vec
  // int64_t t3_1 = common::GetTimeInUsec();
  common::CharSegment(sentence, word_vec_mid);
  // int64_t t3_2 = common::GetTimeInUsec();
  // VLOG(2) << "CharSegment cost time:" << t3_2 - t3_1;
  //
  Deduplicate(word_vec_mid, &word_vec);
  // int64_t t3_3 = common::GetTimeInUsec();
  // VLOG(2) << "Deduplicate cost time:" << t3_3 - t3_2;
  std::string sub_query;
  for (size_t k = 0; k < word_vec.size(); ++k) {
    if (sep_set_.find(word_vec[k]) != sep_set_.end()) {
      VLOG(5) << word_vec[k];
      if (!sub_query.empty()) {
        sub_sentences->push_back(sub_query);
        sub_query.clear();
      }
    } else {
        sub_query += word_vec[k];
        VLOG(6) << "word_vec[k]:" << word_vec[k] << " sub_query:" << sub_query;
    }
  }
  if (!sub_query.empty()) {
    sub_sentences->push_back(sub_query);
  }
}

/* find the most important subsentence
 * input: subsentence vector
 * output: most important subsentence
 * rule: 1.back mort important 2.not int stop sentence set(stop_sentence_set_)
 */
string RemoveStopWordHandler::FindVitalSubsentence(const vector<string>& sub_query_vec) {
  VLOG(5) << "sub_query_vec:" << sub_query_vec.size();
  for (int i = sub_query_vec.size() - 1; i >= 0; --i) {
    string trim_sub_query;
    TrimWhitespace(sub_query_vec[i], TRIM_ALL, &trim_sub_query);
    if (stop_sentence_set_.find(trim_sub_query) != stop_sentence_set_.end()) {
      VLOG(6) << "sub query:" << sub_query_vec[i] << "\ttrim_sub_query:" << trim_sub_query;
      continue;
    }
    return sub_query_vec[i];
  }
  return "";
}

/* remove all stop sentence
 * input: subsentence vector
 * output: all valid subsentence join
 */
string RemoveStopWordHandler::OnlyRemoveStopSentence(const vector<string>& sub_query_vec) {
  std::vector<std::string> new_sub_query_vec;
  VLOG(5) << "sub_query_vec:" << sub_query_vec.size();
  for (int i = 0; i< sub_query_vec.size(); ++i) {
    string trim_sub_query;
    TrimWhitespace(sub_query_vec[i], TRIM_ALL, &trim_sub_query);
    if (stop_sentence_set_.find(trim_sub_query) != stop_sentence_set_.end()) {
      VLOG(5) << "sub query:" << sub_query_vec[i] << "\ttrim_sub_query:" << trim_sub_query;
      continue;
    }
    new_sub_query_vec.push_back(sub_query_vec[i]);
  }
  string subsentence_join = "";
  string prefix = "";
  for (size_t i = 0; i < new_sub_query_vec.size(); ++i) {
    subsentence_join += prefix + new_sub_query_vec[i];
    prefix = " , ";
  }
  return subsentence_join;
}


/* replace term by it synonyms term
 * input: query
 * use KVJumpTrie, key is term, value is synonyms term
 */
bool RemoveStopWordHandler::ReplaceSynonymsStrByTrie(std::string& query) {
  std::vector<std::pair<const std::string*, const std::string*> > synonyms_terms;
  synonyms_term_trie_.LongestMatch(query, &synonyms_terms);
  for (size_t i = 0; i < synonyms_terms.size(); ++i) {
    size_t pos = query.find(*(synonyms_terms[i].first), 0);
    while(pos != std::string::npos) {
      VLOG(5) << "first query:" << query << " synonyms_terms[i].first" << *(synonyms_terms[i].first) << "\tsynonyms_terms[i].second:" << synonyms_terms[i].second;
      query.replace(pos, synonyms_terms[i].first->size() - 1, *(synonyms_terms[i].second));
      pos = query.find(*(synonyms_terms[i].first), pos + synonyms_terms[i].second->size());
      VLOG(5) << "second query:" << query;
    }
  }
  VLOG(5) << "query:" << query;
  return true;
}

/* transfer term by it synonyms term
 * input: query
 * use KVJumpTrie, key is term, value is synonyms term(xxx\001last_str\002first_str)
 */
bool RemoveStopWordHandler::TransferSynonymsStrByTrie(std::string& query) {
  std::vector<std::pair<const std::string*, const std::string*> > synonyms_transfer;
  VLOG(5) << "query:" << query;
  synonyms_transfer_trie_.LongestMatch(query, &synonyms_transfer);
  VLOG(5) << "size:" << synonyms_transfer.size();
  string last_str("");
  string first_str("");
  for (size_t i = 0; i < synonyms_transfer.size(); ++i) {
    string value = *(synonyms_transfer[i].second);
    vector<string> items;
    SplitString(value, '\001', &items);
    if (items.size() < 2) {
      continue;
    }
    if (items.size() == 2) {
      vector<string> sub_items;
      SplitString(items[1], '\002', &sub_items);
      if (items.size() == 2) {
        first_str = sub_items[1];
        last_str = sub_items[0];
      }
    }
    size_t pos = query.find(*(synonyms_transfer[i].first), 0);
    while(pos != std::string::npos) {
      VLOG(5) << "first query:" << query << " synonyms_terms[i].first" << *(synonyms_transfer[i].first);
      query.replace(pos, synonyms_transfer[i].first->size(), items[0]);
      pos = query.find(*(synonyms_transfer[i].first), pos + items[0].size());
      VLOG(5) << "second query:" << query;
    }
  }
  if (!last_str.empty()) {
    query += last_str;
  }
  if (!first_str.empty()) {
    query = first_str + query;
  }

  VLOG(5) << "query:" << query;
  return true;
}

bool RemoveStopWordHandler::ReadFileToSet(const std::string& file_path, common::hash_set<std::string>* str_set) {
  VLOG(5) << "file_path:" << file_path;
  if (file_path.empty()) {
    return false;
  }
  file::SimpleLineReader reader(file_path, true);
  std::vector<std::string> lines;
  reader.ReadLines(&lines);

  for (size_t i = 0; i < lines.size(); ++i) {
    std::string ele;
    /*if (StartsWithASCII(lines[i], "#", false)) {
      continue;
    }
    TrimWhitespace(lines[i], TRIM_ALL, &ele); */
    ele = lines[i];
    VLOG(6) << "stop word: " << ele;
    str_set->insert(ele);
  }
  return true;
}

bool RemoveStopWordHandler::ReadFileToSeWordgSet(const std::string& file_path, common::hash_set<std::string>* str_set) {
  if (file_path.empty()) {
    return false;
  }
  file::SimpleLineReader reader(file_path, true);
  std::vector<std::string> lines;
  reader.ReadLines(&lines);

  for (size_t i = 0; i < lines.size(); ++i) {
    std::string ele;
    if (StartsWithASCII(lines[i], "#", false)) {
      continue;
    }
    string trim_line;
    TrimWhitespace(lines[i], TRIM_ALL, &trim_line);

    // utf8 to jbk
    string gbk_ele;
    char buf_gbk[10240] = {0};
    int gbkLen = ul_utf8_to_gbk(buf_gbk, 10240, trim_line.c_str());
    if (gbkLen < 0) {
      continue;
    }
    gbk_ele = buf_gbk;

    string basic_seg = "";
    string phrase_seg = "";
    wordSegInstance_.wordseg(gbk_ele, basic_seg, phrase_seg);
    // gbk to utf8
    char buf[10240] = {0};
    int utf8Len = ul_gbk_to_utf8(buf, 10240, basic_seg.c_str());
    if (utf8Len <= 0) {
      VLOG(2) << "gbk to utf8 false";
      continue;
    }
    std::string utf8_basic_seg = buf;
    VLOG(6) << "ele: " << lines[i] << "\tbasic_seg:" << utf8_basic_seg;
    str_set->insert(utf8_basic_seg);
  }

  return true;
}

bool RemoveStopWordHandler::ReadFileToMap(const std::string& file_name, common::hash_map<std::string, int>* res_map) {
  file::SimpleLineReader reader(true);
  if (!reader.Open(file_name)) {
    VLOG(5) << "Open file " + file_name + "failed!";
    return false;
  }
  vector<string> lines;
  reader.ReadLines(&lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    vector<string> eles;
    SplitString(lines[i], '\t', &eles);
    VLOG(6) << "line:" << lines[i] << " " << eles[0] << " " << eles[1];
    int count = 0;
    StringToInt(eles[1], &count);
    VLOG(6) << "count:" << count;
    (*res_map)[eles[0]] = count;
  }
  return true;
}

bool RemoveStopWordHandler::ReadPinYinToMap(const std::string& file_name, common::hash_map<string, string>* pinyin_map) {
  file::SimpleLineReader reader(true);
  if (!reader.Open(file_name)) {
    VLOG(5) << "Open file " + file_name + "failed!";
    return false;
  }
  vector<string> lines;
  reader.ReadLines(&lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    vector<string> cols;
    SplitString(lines[i], ' ', &cols);
    if (cols.size() != 2) {
      continue;
    }

    VLOG(6) << "line:" << cols[0] << "\t" << cols[1];
    (*pinyin_map)[cols[0]] = cols[1];

    pinyin_set_.insert(StringToLowerASCII(cols[1]));
  }
  return true;
}

bool RemoveStopWordHandler::BuildSynonymsTermTrie(const string& file_path, trie::KvJumpTrie<string>& jump_trie) {
  if (file_path.empty()) {
    VLOG(5) << "Read synonyms term file failed!";
    return false;
  }
  VLOG(5) << "file name:" << file_path;
  file::SimpleLineReader reader(file_path, true);
  vector<string> lines;
  reader.ReadLines(&lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    if (StartsWithASCII(lines[i], "#", false)) {
      continue;
    }
    vector<string> eles;
    SplitString(lines[i], '\t', &eles);
    if (eles.size() < 2) {
      VLOG(5) << "Error line:" << lines[i];
      continue;
    }
    // utf8 to jbk
    string gbk_ele;
    char buf_gbk[10240] = {0};
    int gbkLen = ul_utf8_to_gbk(buf_gbk, 10240, eles[0].c_str());
    if (gbkLen < 0) {
      continue;
    }
    gbk_ele = buf_gbk;
    // word segment
    string basic_seg = "";
    string phrase_seg = "";
    wordSegInstance_.wordseg(gbk_ele, basic_seg, phrase_seg);

    char buf[10240] = {0};
    int utf8Len = ul_gbk_to_utf8(buf, 10240, basic_seg.c_str());
    if (utf8Len <= 0) {
      VLOG(2) << "gbk to utf8 false";
      continue;
    }
    std::string utf8_basic_seg = buf;
    VLOG(5) << "utf8_basic_seg:" << utf8_basic_seg;
    utf8_basic_seg = " " + utf8_basic_seg;
    utf8_basic_seg += " ";
    jump_trie.AddString(utf8_basic_seg, eles[1]);
  }
  jump_trie.Build();
  return true;
}

bool RemoveStopWordHandler::BuildEmojiTrie(const string& file_path, trie::KvJumpTrie<string>& jump_trie) {
  if (file_path.empty()) {
    VLOG(5) << "emoji filter is empty";
    return false;
  }
  VLOG(5) << "file name:" << file_path;

  file::SimpleLineReader reader(file_path, true);
  vector<string> lines;
  reader.ReadLines(&lines);

  for (size_t i = 0; i < lines.size(); ++i) {
    StringToLowerASCII(&lines[i]);
    jump_trie.AddString(lines[i], "0");
  }
  jump_trie.Build();
  return true;
}

string RemoveStopWordHandler::ReplaceSep(const std::string& str, char src_sep, const std::string& des_sep) {
  vector<string> words;
  string tmp_str = "";
  TrimWhitespace(str, TRIM_ALL, &tmp_str);
  SplitString(tmp_str, src_sep, &words);
  return SepcialJoinString(words, des_sep);
}

string RemoveStopWordHandler::SepcialJoinString(vector<string> words, const string& des_sep) {
  string res_str = "";
  string pre_fix = "";
  for (size_t i = 0; i < words.size(); ++i) {
    if (words[i].empty()) {
      continue;
    }
    VLOG(5) << "word:" << words[i];
    if (words[i] == "," || words[i] == "\001") {
      res_str += pre_fix + ' ';
    } else {
      res_str += pre_fix + words[i];
    }
	pre_fix = des_sep;
  }
  VLOG(5) << "res_str:" << res_str;
  return res_str;
}

bool RemoveStopWordHandler::TruncateLongQuery(const string& raw_query, string* query) {
  std::basic_string<uint16> query_string16;
  bool flag = encode::ToString16(raw_query, &query_string16);
  if (!flag) {
    VLOG(2) << "utf8 to string16 failed!";
    return false;
  }
  if (query_string16.size() > 30) {
    std::basic_string<uint16> tmp_query_string16(query_string16.begin() + query_string16.size() - 30, query_string16.end());
    (*query) = encode::ToUTF8String(tmp_query_string16);
  }
  return true;
}

void RemoveStopWordHandler::Deduplicate(const vector<string>& raw_query_vec, vector<string>* query) {
  int i = 0;
  // int64_t t3_1 = common::GetTimeInUsec();
  while(i < raw_query_vec.size()) {
    bool find = false;
    common::hash_map<string, int>::const_iterator c_it = reduplication_map_.find(raw_query_vec[i]);
    if (c_it != reduplication_map_.end()) {
      // VLOG(5) << "find";
      find = true;
    }
    query->push_back(raw_query_vec[i]);
    ++i;
    if (find) {
      int count = 1;
      while(i < raw_query_vec.size() && (c_it->first == raw_query_vec[i] || raw_query_vec[i] == " ")) {
        if (count < c_it->second) {
          if (c_it->first == raw_query_vec[i]) {
            VLOG(5) << "duplicate word:" << raw_query_vec[i];
            query->push_back(raw_query_vec[i]);
            ++count;
          }
        }
        ++i;
        if (i >= raw_query_vec.size()) {
          return;
        }
        VLOG(5) << "raw_query_vec[" << i << "]:" << raw_query_vec[i] << "\t" << c_it->first;
      }
      if (i > 0 && raw_query_vec[i - 1] == " ") {
        query->push_back(" ");
      }
      find = false;
    }
  }
  // int64_t t3_5 = common::GetTimeInUsec();
  // VLOG(2) << "each run redup1 cost time:" << t3_5 - t3_1 << " count:" << i;
}

bool RemoveStopWordHandler::TruncateLongQuery(const string &raw_query, string &query) {
	query = "";
  char buf[10240] = {0};
  int utf8Len = ul_gbk_to_utf8(buf, 10240, raw_query.c_str());
  if (utf8Len <= 0) {
    VLOG(2) << "gbk to utf8 false";
    return false;
  }
  std::string query_utf8 = buf;
	std::basic_string<uint16> query_string16;
	bool flag = encode::ToString16(query_utf8, &query_string16);
	if (!flag) {
		query = "";
		VLOG(2) << "Utf-8 to string16 failed" << raw_query;
		return false;
	}

	int query_string16_size = query_string16.size();
	if (query_string16_size > 30) {
		std::basic_string<uint16> tmp_query_string16(query_string16.begin() + query_string16_size - 30, query_string16.end());
		query = encode::ToUTF8String(tmp_query_string16);
    char buf1[BYTESIZE] = {0};
    int gbkLen = ul_utf8_to_gbk(buf1, BYTESIZE, query.c_str());
    if (gbkLen <= 0) {
      VLOG(2) << "utf8 to gbk false";
      return false;
    }
    query = buf1;
	} else {
		query = raw_query;
	}
	return true;
}

string RemoveStopWordHandler::RemoveEmoji(const string& query) {
  std::vector<std::pair<const std::string*, const std::string*> >matched_emoji;
  emoji_trie_.LongestMatch(query, &matched_emoji);
  string cleaned_query = "";
  int start = 0;
  VLOG(2) << matched_emoji.size();
  for (size_t i = 0; i < matched_emoji.size(); ++i) {
    string emoji = *(matched_emoji[i].first);
    VLOG(5) << "emoji:" << emoji;
    size_t pos = query.find(emoji, start);
    if (pos != string::npos) {
      string sub_str = query.substr(start, pos - start);
      VLOG(5) << "sub_str:" << sub_str;
      start = pos + emoji.size();
      cleaned_query += sub_str;
    }
  }
  if (start < query.size()) {
    cleaned_query += query.substr(start);
  }
  VLOG(2) << "cleaned_query:" << cleaned_query << "\t" << "query:" << query;
  return cleaned_query;
}

bool RemoveStopWordHandler::ConvertQuery2Pinyin(QueryInfo* query_info) {
  string query = query_info->query;
  StringToLowerASCII(&query);
  query = RemoveEmoji(query);
  std::vector<string> word_vec;
  common::CharSegment(query, word_vec);
  VLOG(5) << "word_vec:" << word_vec;
  vector<string> new_word_vec; 
  for (size_t i = 0; i < word_vec.size(); ++i) {
    if (punc_set_.find(word_vec[i]) == punc_set_.end()) {
      new_word_vec.push_back(word_vec[i]);
    }
  }
  word_vec = new_word_vec;
  VLOG(5) << "new_word_vec:" << word_vec;
  vector<string> query_pinyin_vec;
  string alpha_str = "";
  for (size_t i = 0; i < word_vec.size(); ++i) {
    if (IsAsciiAlpha(word_vec[i])) {
      alpha_str += word_vec[i];
      VLOG(6) << "isalpha:" << alpha_str;
      continue;
    } else if (alpha_str != "") {
      VLOG(6) << "alpha_str:" << alpha_str;
      query_pinyin_vec.push_back(seg_pinyin(alpha_str));
      alpha_str = "";
    }
      common::hash_map<string, string>::const_iterator c_it = pinyin_map_.find(word_vec[i]);
      if (c_it != pinyin_map_.end()) {
        query_pinyin_vec.push_back(c_it->second);
      } else {
        query_pinyin_vec.push_back(word_vec[i]);
        VLOG(5) << "not found pinyin:" << word_vec[i];
      }
  }

  if (alpha_str != "") {
    VLOG(5) << "alpha_str1:" << alpha_str;
    query_pinyin_vec.push_back(seg_pinyin(alpha_str));
    alpha_str = "";
  }
  string query_pinyin = JoinVector(query_pinyin_vec, ' ');
  VLOG(5) << "query_pinyin:" << query_pinyin;
  query_info->query_pinyin = query_pinyin;
  return true;
}

std::string RemoveStopWordHandler::seg_pinyin(string pinyin_str) {
  int i = 0;
  vector<string> res;
  while (i < pinyin_str.size()) {
    vector<pair<string, int> > sub_pinyin_vec;

    sub_pinyin_vec.push_back(make_pair(pinyin_str.substr(i, 1), 1));
    int j = i + 1;
    while( j < pinyin_str.size()) {
      string sub_pinyin = pinyin_str.substr(i, j - i + 1);
      if (pinyin_set_.find(sub_pinyin) != pinyin_set_.end()) {
        sub_pinyin_vec.push_back(make_pair(sub_pinyin, j - i + 1));
      }
      ++j;
    }
    pair<string, int> last = sub_pinyin_vec.back();
    res.push_back(last.first);
    i += last.second;
  }

  VLOG(5) << "seg pinyin res:" << res;
  return JoinVector(res, ' ');
}

std::string RemoveStopWordHandler::RemovePunctuation(const std::string& query) {
  VLOG(2) << "before remove punctuation:"<< query;
  string cleaned_query = "";
  std::vector<string> word_vec;
  common::CharSegment(query, word_vec);
  for (size_t i = 0; i < word_vec.size(); ++i) {
    if (punc_set_.find(word_vec[i]) == punc_set_.end()) {
      cleaned_query += word_vec[i];
    }
  }

  VLOG(2) << "after remove puctuation:" << query;
  return cleaned_query;
}

REGISTER_DOCPROCESSHANDLER(RemoveStopWordHandler)
} // namespace preprocess
