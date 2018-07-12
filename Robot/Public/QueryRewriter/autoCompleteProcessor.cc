#include "Common/Util/File/simpleLineReader.h"
#include "Common/Base/stringUtil.h"
#include "Robot/Public/QueryRewriter/autoCompleteProcessor.h"
#include "Robot/Public/QueryAutoComplete/Src/query_autocomplete.h"
#include "Robot/Public/QueryRewriter/queryRewriter.h"
#include "Robot/Public/Utils/singletonWordSegTag.h"


using common::hash_map;
using std::make_pair;
using std::pair;
using std::set;
using std::string;
using std::vector;

DEFINE_string(hot_sentence_file,
    "../Data/QueryAutoComplete/other/hot_sentence.data", "");
DEFINE_string(interr_badcase,
    "../Data/QueryAutoComplete/sentence_type_data/norm_interr_badcase.data",
    "");
DEFINE_string(interr_re_file,
    "../Data/QueryAutoComplete/sentence_type_data/interr.data",
    "regular expression in interrogative sentence");
DEFINE_string(nega_badcase_file,
    "../Data/QueryAutoComplete/sentence_type_data/negative_badcase.data",
    "for example: 管->不管");
DEFINE_string(nega_re_file,
    "../Data/QueryAutoComplete/sentence_type_data/negative.data",
    "regular expression in negative sentence");
DEFINE_string(norm_interr_re_file,
    "../Data/QueryAutoComplete/sentence_type_data/norm_interr.data",
    "regular expression in norm-interrogative sentence");
DEFINE_string(norm_interr_special_word,
    "../Data/QueryAutoComplete/other/norm_interr_special_word.data", "");
DEFINE_string(norm_interr_tail_word,
    "../Data/QueryAutoComplete/sentence_type_data/rtrim.data",
    "tail word in general interrogative sentence");
DEFINE_string(punc_file, "../Data/QueryAutoComplete/other/punctuation.dat", "");
DEFINE_string(query_to_complete_file,
    "../Data/QueryAutoComplete/query/query_need_complete.data",
    "query need to complete");
DEFINE_string(selected_interr_re_file,
    "../Data/QueryAutoComplete/sentence_type_data/selected_interr.data",
    "regular expression in selected-interrogative sentence");
DEFINE_string(seperate_file, "../Data/QueryAutoComplete/other/separator.dat",
    "seperate text to sentences");
DEFINE_string(unpronoun_rev_word,
    "../Data/QueryAutoComplete/other/unpronoun_rev.data", "");

const string BU = "不";
const string MEI = "没";
const string YOU = "有";
const string WO = "我";
const string NI = "你";
const string MEN = "们";

namespace robot {

bool AutoCompleteProcessor::Init(QueryRewriter* p) {
    handler_ = p;
  // init seperator set
  if (!ReadFileToSet(FLAGS_seperate_file, &seperator_set_)) {
    VLOG(1) << "get seperator set failed.";
    return false;
  }

  // interrogative badcase set
  if (!ReadFileToSet(FLAGS_interr_badcase, &interr_badcase_set_)) {
    VLOG(1) << "get interrogative badcase set failed.";
    return false;
  }

  // rtrim set in general interrogative sentence
  if (!ReadFileToSet(FLAGS_norm_interr_tail_word, &rtrim_set_)) {
    VLOG(1) << "get rtrim set failed.";
    return false;
  }

  // nega set
  if (!ReadFileToSet(FLAGS_nega_re_file, &nega_set_)) {
    VLOG(1) << "get nega set failed.";
    return false;
  }

  // normal nega set
  normal_nega_set_.insert(BU);
  normal_nega_set_.insert(MEI);

  // specail set
  if (!ReadFileToSet(FLAGS_norm_interr_special_word, &norm_interr_special_word_)) {
    VLOG(1) << "get unpronouon_rev set failed.";
    return false;
  }
  // unpronoun_rev
  if (!ReadFileToSet(FLAGS_unpronoun_rev_word, &unpronoun_rev_set_)) {
    VLOG(1) << "get norm interr special word set failed.";
    return false;
  }

  // init interr_sentence_re
  string interr_re_str("");
  GetReFromFile(FLAGS_interr_re_file, &interr_re_str);
  interr_sentence_re_.reset(new re2::RE2(".*" + interr_re_str + ".*"));
  if (interr_sentence_re_.get() == NULL) {
    VLOG(1) << "init interr_sentence_re_ failed.";
    return false;
  }
  // init norm_interr_sentence_re
  string norm_interr_re_str("");
  GetReFromFile(FLAGS_norm_interr_re_file, &norm_interr_re_str);
  norm_interr_sentence_re_.reset(new re2::RE2(norm_interr_re_str));
  if (norm_interr_sentence_re_.get() == NULL) {
    VLOG(1) << "init norm_interr_sentence_re_ failed.";
    return false;
  }
  // init selected_interr_sentence_re
  string selected_interr_re_str("");
  GetReFromFile(FLAGS_selected_interr_re_file, &selected_interr_re_str);
  selected_interr_sentence_re_.reset(new re2::RE2(selected_interr_re_str));
  if (selected_interr_sentence_re_.get() == NULL) {
    VLOG(1) << "init selected_interr_sentence_re_ failed.";
    return false;
  }
  
  // init nega_re 
  string nega_re_str("");
  GetReFromFile(FLAGS_nega_re_file, &nega_re_str);
  string nega_re_case_str("");
  GetRe2FromFile(FLAGS_nega_badcase_file, &nega_re_case_str);
  nega_sentence_re_.reset(new re2::RE2(".*" + nega_re_str + nega_re_case_str + ".*"));
  if (nega_sentence_re_.get() == NULL) {
    VLOG(1) << "init nega_sentence_re_ failed.";
    return false; 
  }

  // init hot query dict need to complete
  file::SimpleLineReader reader(FLAGS_query_to_complete_file, true);
  vector<string> lines;
  reader.ReadLines(&lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    vector<string> items;
    SplitString(lines[i], '\t', &items);
    if (items.size() != 3) {
      continue;
    }
    complete_query_[items[0]] = make_pair(items[1], query_autocomplete::CompleteType(StringToInt(items[2])));
  }

  // init hot sentence hash_map
  lines.clear();
  if (!reader.Open(FLAGS_hot_sentence_file)) {
    VLOG(1) << "open hot sentence file failed.";
    return false;
  }
  reader.ReadLines(&lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    vector<string> items;
    SplitString(lines[i], '\t', &items);
    if (items.size() != 2) {
      continue;
    }
    hot_sentence_map_[items[0]] = StringToDouble(items[1]);
  }

  // init punctuation hash_set
  lines.clear();
  if (!reader.Open(FLAGS_punc_file)) {
    VLOG(1) << "open punctuation file failed.";
    return false;
  }
  reader.ReadLines(&lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    if (lines[i].empty()) {
      continue;
    }
    punc_set_.insert(lines[i]);
  }
  
  return true;
}

bool AutoCompleteProcessor::GetReFromFile(const string& re_file,
                                      string* re_expression) {
  if (re_file.empty()) {
    VLOG(2) << "regular file is empty.";
    return false;
  } 
  if (re_expression->empty()) {
    re_expression->clear();
  }
  file::SimpleLineReader reader(re_file, true);
  vector<string> lines;
  reader.ReadLines(&lines);
  *re_expression = "(";
  string or_str("");
  for (size_t i = 0; i < lines.size(); ++i) {
    *re_expression += or_str + lines[i];
    or_str = "|";
  }
  *re_expression += ")";
  return true;
}

bool AutoCompleteProcessor::GetRe2FromFile(const string& re_file,
                                      string* re_expression) {
  if (re_file.empty()) {
    VLOG(2) << "regular file is empty.";
    return false;
  }
  if (re_expression->empty()) {
    re_expression->clear();
  }
  file::SimpleLineReader reader(re_file, true);
  vector<string> lines;
  reader.ReadLines(&lines);
  *re_expression = "[^";
  for (size_t i = 0; i < lines.size(); ++i) {
    *re_expression += lines[i];
  }
  *re_expression += "]";
  return true;
}
   
bool AutoCompleteProcessor::ReadFileToSet(const string& file_path,
                                      set<string>* term_set) {
  if (file_path.empty()) {
    VLOG(1) << "file path is empty.";
    return false;
  }
  file::SimpleLineReader reader(file_path, true);
  std::vector<std::string> lines;
  reader.ReadLines(&lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    if (lines[i].empty()) {
      continue;
    }
    term_set->insert(lines[i]);
  }
  return true;
}

void AutoCompleteProcessor::GetKeySentence(const string &text,
                                       string* sentence) {
  vector<pair<string, string> > sents;
  SeperateSentences(text, &sents);
  if (sents.size() == 1) {
    *sentence = text;
    return;
  }

  string temp("");
  for (size_t i = 0; i < sents.size(); ++i) {
    if (sents[i].first.empty() || sents[i].first == " " ||
        punc_set_.find(sents[i].first) != punc_set_.end()) {
      continue;
    }
    if (hot_sentence_map_.find(sents[i].first) == hot_sentence_map_.end()) {
      temp += sents[i].first + sents[i].second;
    }
  }
  if (temp.empty()) {
    for (int i = sents.size() - 1; i >= 0; --i) {
      if (sents[i].first.empty() || sents[i].first == " " ||
          punc_set_.find(sents[i].first) != punc_set_.end()) {
        continue;
      }
      *sentence = sents[i].first + sents[i].second;
      break;
    }
  } else {
    *sentence = temp;
  }
}

void AutoCompleteProcessor::SeperateSentences(const string &text,
    vector<pair<string, string> >* sents) {
  vector<string> word_vec;
  common::CharSegment(text, word_vec);
  string sentence("");
  for (size_t k = 0; k < word_vec.size(); ++k) {
    if (seperator_set_.find(word_vec[k]) != seperator_set_.end()) {
      if (!sentence.empty()) {
        sents->push_back(make_pair(sentence, word_vec[k]));
        sentence.clear();
      }
    } else {
      sentence += word_vec[k];
    }
  }
  if (!sentence.empty()) {
    sents->push_back(make_pair(sentence, ""));
  }
}

void AutoCompleteProcessor::PronounReverse(const string& sentence,
                                       string* sentence_reverse) {
  sentence_reverse->clear();
  vector<string> segs_vec;
  SingletonWordSegTag::get()->Segment(sentence, segs_vec);
  for (size_t i = 0; i < segs_vec.size(); ++i) {
    if (segs_vec[i].empty()) {
      continue;
    }
    bool hit_unpronoun_rev_set = false;
    for (std::set<string>::iterator iter = unpronoun_rev_set_.begin();
      // unpronoun_rev_set filter.
        iter != unpronoun_rev_set_.end();
        ++iter) {
      if (segs_vec[i].find(*iter) != string::npos) {
        hit_unpronoun_rev_set = true;
        break;
      }
    }

    if (hit_unpronoun_rev_set) {
      *sentence_reverse += segs_vec[i];
      continue;
    } else {
      // Reverse "WO", "NI".
      size_t curr_pos = 0;
      while (curr_pos != segs_vec[i].size()) {
        if ((curr_pos + WO.size()) <= segs_vec[i].size() &&
            WO == segs_vec[i].substr(curr_pos, WO.size())) {
          segs_vec[i].replace(curr_pos, WO.size(), NI);
          curr_pos += NI.size();
        } else if ((curr_pos + NI.size()) <= segs_vec[i].size() &&
            NI == segs_vec[i].substr(curr_pos, NI.size())) {
          segs_vec[i].replace(curr_pos, NI.size(), WO);
          curr_pos += WO.size();
        } else {
          ++curr_pos;
        }
      }
      *sentence_reverse += segs_vec[i];
    }
  }
  VLOG(5) << "sentence_reversed: " << *sentence_reverse;
}

int32 AutoCompleteProcessor::IsNormInterrSentence(const string& sent, int32& len) {
  // Return the index of B in syntactical structure "ABA",
  // in which B is a negative word in nega_set_.
  // And if this sentence is not like "ABA", return 0.
  // "len" is the length of A.
  vector<string> word_vec;
  common::CharSegment(sent, word_vec);
  for (int i = 0; i < word_vec.size(); ++i) {
    // check every negative word.
    set<string>::iterator itr = nega_set_.find(word_vec[i]);
    if (itr != nega_set_.end()) {
      for (int j = 2; j > 0; --j) {
        // check bigram and unigram in order.
        int begin = i - j;
        int end = i + j;
        if (begin < 0 || end >= word_vec.size()) {
          continue;
        }
        string str_left, str_right;
        for (int k = i - j, m = 1; k < i; ++k, ++m) {
          // prepare comparing strings before and behind the negative word.
          str_left += word_vec[k];
          str_right += word_vec[i+m];
        }
        if (str_left == str_right &&
            interr_badcase_set_.find(str_left) == interr_badcase_set_.end()) {
          // negative word blacklist filtering.
          int temp = i - j;
          if (temp == 0 || (temp > 0 && word_vec[temp-1] != *itr)) {
            // finally, guarantee the word before str_left
            // is not this negative word.
            len = j;
            return i;
          }
        }
      }
    }
  }
  return 0;
}

query_autocomplete::SentenceType AutoCompleteProcessor::GetSentenceType(const string &sent) {
  int length = 0;
  if (re2::RE2::FullMatch(sent, *selected_interr_sentence_re_)) {
    return SELECTED_INTERR;
  } else if (re2::RE2::FullMatch(sent, *interr_sentence_re_)) {
    return INTERR;
  } else if (re2::RE2::FullMatch(sent, *norm_interr_sentence_re_)) {
    return NORM_INTERR;
  } else if (IsNormInterrSentence(sent, length)) {
    return NORM_INTERR;
  } else if (re2::RE2::FullMatch(sent, *nega_sentence_re_)) {
    return NEGA;
  } else {
    return AFFIRM;
  }
}

bool AutoCompleteProcessor::AffirmComplete(const string& query,
                                       const string& last_reply,
                                       string* complete_query) {
  int32 length = 0;
  int32 nega_word_index = IsNormInterrSentence(last_reply, length);

  vector<string> word_vec;
  common::CharSegment(last_reply, word_vec);

  if (nega_word_index > 0) {
    // syntactical structure ABA.
    for (size_t i = 0; i < word_vec.size(); ++i) {
      if (i < nega_word_index - length || i > nega_word_index) {
        // skip indices [nega_word_index - length, nega_word_index].
        *complete_query += word_vec[i];
      }
    }
  } else {
    // negative interrogative sentence.
    int i = 0;
    for (; i < word_vec.size(); ++i) {
      set<string>::iterator itr;
      if ((itr = nega_set_.find(word_vec[i])) != nega_set_.end()) {
        // search the first negative word and skip it when insert into complete_query.
        i++;
        break;
      } else {
        // normal interrogative sentence (no negative word).
        *complete_query += word_vec[i];
      }
    }
    for (; i < word_vec.size(); ++i) {
      *complete_query += word_vec[i];
    }
  }

  string result("");
  RtrimString(*complete_query, &result);
  //PronounReverse(result, &reverse_reply);
  VLOG(5) << "RtrimString-ed result: " << result;
  // Eliminate special word in norm_interr.
  for(std::set<std::string>::iterator it = norm_interr_special_word_.begin();
          it != norm_interr_special_word_.end(); ++it) {
    int pos;
    if((pos = result.find(*it)) != std::string::npos) {
        result.erase(pos, it->size());
    }
  }
  *complete_query = result;
  VLOG(5) << "AffirmComplete end, complete_query: " << *complete_query;
  return true;
}

bool AutoCompleteProcessor::NegaComplete(const string& query,
                                     const string& last_reply,
                                     string* complete_query) {
  int32 length = 0;
  int32 nega_word_index = IsNormInterrSentence(last_reply, length);

  vector<string> word_vec;
  common::CharSegment(last_reply, word_vec);

  if (nega_word_index > 0) {
    // syntactical structure ABA.
    for (size_t i = 0; i < word_vec.size(); ++i) {
      if (i < nega_word_index - length || i >= nega_word_index) {
        // skip indices [nega_word_index - length, nega_word_index).
        *complete_query += word_vec[i];
      }
    }
  } else {
    bool normal_nega_in_head_processed = false;
    string normal_nega_in_head("");
    for (std::set<string>::iterator iter = normal_nega_set_.begin();
        iter != normal_nega_set_.end();
        ++iter) {
      // for cases that normal_nega word in query head.
      if (0 == query.find(*iter)) {
        normal_nega_in_head = *iter;
        VLOG(1) << "iter normal_nega_in_head: " << normal_nega_in_head;
        string query_aft_part = query.substr((*iter).size());
        size_t query_aft_part_pos = last_reply.find(query_aft_part);
        if (query_aft_part_pos != string::npos &&
            last_reply.find(query) == string::npos) {
          // aft part matched with reply and last_reply does not contain query.
          string tmp_result = last_reply;
          tmp_result.replace(query_aft_part_pos, query_aft_part.size(), query);
          *complete_query = tmp_result;
          normal_nega_in_head_processed = true;
          break;
        } else if (last_reply.find(query) != string::npos) {
          // last_reply contains query.
          *complete_query = last_reply;
          normal_nega_in_head_processed = true;
          break;
        } else {
          // left for considering with part of speech.
        }
      }
    }
    if (!normal_nega_in_head_processed) {
      // other cases. Considering with part of speech.
      vector<string> segs;
      SingletonWordSegTag::get()->Segment(last_reply, segs);

      hash_map<string, pair<string, query_autocomplete::CompleteType> >::iterator itr =
          complete_query_.find(query);
      bool is_nega_query = (itr != complete_query_.end()) &&
          (itr->second.second == NEGA_COMPLETE);
      size_t i = 0;
      bool replaced = false;
      for (; i < segs.size(); ++i) {
        vector<string> seg_items;
        SplitString(segs[i], '\001', &seg_items);
        if (seg_items.size() != 2) {
          continue;
        }
        if (is_nega_query) {
          // query still negative. Eliminate norm_neg word in reply.
          for (std::set<std::string>::iterator find_normal_neg_iter = normal_nega_set_.begin();
              find_normal_neg_iter != normal_nega_set_.end();
              ++find_normal_neg_iter) {
            if (seg_items[0].find(*find_normal_neg_iter) != string::npos) {
              //ReplaceString(seg_items[0], *find_normal_neg_iter, "");
              ReplaceFirstSubstringAfterOffset(&seg_items[0], 0,
                  *find_normal_neg_iter, "");
              *complete_query += seg_items[0];
              replaced = true;
              break;
            }
          }
          if (replaced) {
            break;
          } else {
            // find the first verb or adjective,
            // last segs is not normal_neg (last "for" works), head norm_neg word.
            if (seg_items[1] != "v" && seg_items[1] != "vm" &&
                seg_items[1] != "a") {
              *complete_query += seg_items[0];
              continue;
            }
            VLOG(1) << "last_reply: " << last_reply
                    << "\tseg_items[0]: " << seg_items[0]
                    << "\tseg_items[1]: " << seg_items[1];

            if (normal_nega_in_head.empty()) {
              if (seg_items[0] == YOU) {
                *complete_query += MEI + seg_items[0];
              } else {
                *complete_query += BU + seg_items[0];
              }
            } else {
              *complete_query += (normal_nega_in_head + seg_items[0]);
            }
            replaced = true;
          }
        }
        if (replaced) break;
      }
      if (replaced) {
        // append aft part.
        i++;
        for (; i < segs.size(); ++i) {
          vector<string> seg_items;
          SplitString(segs[i], '\001', &seg_items);
          if (seg_items.size() != 2) {
            continue;
          }
          *complete_query += seg_items[0];
        }
      } else {
        // default operation for others.
        JoinQueryReplyComplete(query, last_reply, complete_query, true);
      }
    }
  }

  string result("");
  RtrimString(*complete_query, &result);
  //PronounReverse(result, &reverse_reply);
  VLOG(5) << "RtrimString-ed result: " << result;
  // Eliminate special word in norm_interr.
  for(std::set<std::string>::iterator it = norm_interr_special_word_.begin();
          it != norm_interr_special_word_.end(); ++it) {
    int pos;
    if((pos = result.find(*it)) != std::string::npos) {
        result.erase(pos, it->size());
    }
  }
  *complete_query = result;
  VLOG(5) << "NegaComplete end, complete_query: " << *complete_query;
  return true;
}

bool AutoCompleteProcessor::ContainQuery(const string& query, const string& reply) {
  string temp = query;
  vector<string> word_vec;
  common::CharSegment(temp, word_vec);
  //update@zhengpeiming@15.02.17
  if(word_vec.size() >= 5){
    return false;
  }
  //
  if (word_vec.size() > 1 && (nega_set_.find(word_vec[0]) != nega_set_.end())) {
    temp = temp.substr(word_vec[0].size(), temp.size()-word_vec[0].size());
  }
  if (reply.find(temp) != string::npos) {
    return true;
  }
  return false;
}

void AutoCompleteProcessor::RtrimString(const string& str, string* trim_str) {
  vector<string> word_vec;
  common::CharSegment(str, word_vec);
  bool trim_mark = false;
  for (int i = word_vec.size()-1; i >= 0; --i) {
    if (rtrim_set_.find(word_vec[i]) != rtrim_set_.end()) {
      word_vec.pop_back();
      trim_mark = true;
    } else {
      break;
    }
  }
  if (trim_mark) {
    for (size_t i = 0; i < word_vec.size(); ++i) {
      *trim_str += word_vec[i];
    }
  } else {
    *trim_str = str;
  }
}

bool AutoCompleteProcessor::IsPuncEnd(const string& sent) {
  vector<string> word_vec;
  common::CharSegment(sent, word_vec);
  if (word_vec.empty()) {
    return false;
  }
  set<string>::iterator itr = seperator_set_.find(word_vec[word_vec.size()-1]);
  if (itr != seperator_set_.end()) {
    return true;
  }
  return false;
}
void AutoCompleteProcessor::PopPunc(string& sent, const std::set<string> &punc) {//zhengpeiming
  vector<string> word_vec;
  common::CharSegment(sent, word_vec);
  while(word_vec.size() > 1 && punc.find(word_vec.back()) != punc.end()) {
    sent = sent.substr(0, sent.size() - word_vec.back().size());
    word_vec.pop_back();    
  }
}
void AutoCompleteProcessor::ReplacePunc(string& sent, //zhengpeiming
                                    const string &spunc, 
                                    const string &dpunc) {
  string::size_type pos = 0;
  string::size_type slen = spunc.size();
  string::size_type dlen = dpunc.size();
  while((pos = sent.find(spunc, pos)) != string::npos) {
    sent.replace(pos, slen, dpunc);
    pos += dlen;
  }
}
bool AutoCompleteProcessor::JoinQueryReplyComplete(const string& query,
                                               const string& last_reply,
                                               string* complete_query,
                                               bool query_is_front) {
  if (query.empty() || last_reply.empty()) {
    *complete_query = query;
    return false;
  }
  /*
  string reverse_reply("");
  if(query.find(WOYE) == string::npos) {
    //PronounReverse(last_reply, &reverse_reply);
  } else {
    reverse_reply = last_reply;
  }
  */
  string join_str("");
  if (query_is_front) {
    if (!IsPuncEnd(query)) {
      join_str = "，";
    }
    *complete_query = query + join_str + last_reply;
  } else {
    if (!IsPuncEnd(last_reply)) {
      join_str = "，";
    } 
    *complete_query = last_reply + join_str + query;
  }  
  return true;
}

bool AutoCompleteProcessor::JoinQueryQueryComplete(const string& query,
                                               const string& last_query,
                                               string* complete_query, bool rev) {
  if (query.empty() || last_query.empty()) {
    *complete_query = query;
    return false;
  }
  string join_str("");
  if (!IsPuncEnd(last_query)) {
    join_str = "，";
  }
  if(!rev) {
    *complete_query = last_query + join_str + query;
  }
  else {
    *complete_query = query + join_str + last_query;
  }
  return true;
}

query_autocomplete::CompleteType AutoCompleteProcessor::Run(const string &query,
                           const string &last_query,
                           const string &last_reply,
                           string* complete_query) {
  int32 completion_src = 0;
  query_autocomplete::CompleteType complete_ret =
      Process(query, last_query, last_reply, complete_query, completion_src);
  // Pronoun reverse the completed query iff reply completion.
  if (2 == completion_src) {
    string reversed_completed_query("");
    PronounReverse(*complete_query, &reversed_completed_query);
    *complete_query = reversed_completed_query;
  }
  return complete_ret;
}

query_autocomplete::CompleteType AutoCompleteProcessor::Process(const string &query,
                           const string &last_query,
                           const string &last_reply,
                           string* complete_query,
                           int32& completion_src) {
  if (query.empty()) {
    return UNKNOWN_COMPLETE;
  }
  complete_query->clear();
  string last_reply_key("");
  GetKeySentence(last_reply, &last_reply_key);
  if (query.find("？") == string::npos && ContainQuery(query, last_reply_key)) {
    // This query is the reply to last reply, including affirm and nega.
    query_autocomplete::SentenceType sentence_type = GetSentenceType(last_reply_key);
    if (sentence_type == NORM_INTERR) {
      if ((query.find(BU) != string::npos || query.find(MEI) != string::npos)) {
        NegaComplete(query, last_reply_key, complete_query);
        completion_src = 2;
        if(!complete_query->empty()) {
          PopPunc(*complete_query, {"?", "？"});
        }
        return NEGA_COMPLETE;
      } else {
        AffirmComplete(query, last_reply_key, complete_query);
        completion_src = 2;
        if(!complete_query->empty()) {
          PopPunc(*complete_query, {"?", "？"});
        }
        return AFFIRM_COMPLETE;
      }
    }
  }

  hash_map<string, pair<string, query_autocomplete::CompleteType> >::iterator itr =
      complete_query_.find(query); 
  if (itr == complete_query_.end()) {
    if (complete_query->empty()) {
      *complete_query = query;
    }
    return UNKNOWN_COMPLETE;
  }

  // CompleteType matched.
  query_autocomplete::CompleteType complete_type = itr->second.second;
  if (complete_type == AFFIRM_COMPLETE) {
    query_autocomplete::SentenceType sentence_type = GetSentenceType(last_reply_key);
    if (sentence_type == NORM_INTERR) {
      AffirmComplete(itr->second.first, last_reply_key, complete_query);
    } else if (sentence_type == AFFIRM || sentence_type == NEGA) { 
      *complete_query = last_reply_key;
    }
    if((*complete_query).empty()) {
      *complete_query = last_reply_key;
    }
    JoinQueryReplyComplete(itr->second.first, *complete_query, complete_query, true);
    completion_src = 2;
  } else if (complete_type == NEGA_COMPLETE) { 
    query_autocomplete::SentenceType sentence_type = GetSentenceType(last_reply_key);
    if (sentence_type == AFFIRM || sentence_type == NEGA ||
        sentence_type == NORM_INTERR) {
      NegaComplete(itr->second.first, last_reply_key, complete_query);
      completion_src = 2;
    } else {
      return UNKNOWN_COMPLETE;
    }
  } else if (complete_type == JOINT_BEFORE_COMPLETE) {
    query_autocomplete::SentenceType sentence_type = GetSentenceType(last_reply_key);
    if (sentence_type == AFFIRM || sentence_type == NEGA) {
      JoinQueryReplyComplete(itr->second.first, last_reply_key, complete_query, true);
      completion_src = 2;
    } else {
      return UNKNOWN_COMPLETE;
    }
  } else if (complete_type == JOINT_AFTER_COMPLETE) {
    query_autocomplete::SentenceType sentence_type = GetSentenceType(last_reply_key);
    if (sentence_type == AFFIRM || sentence_type == NEGA) {
      JoinQueryReplyComplete(itr->second.first, last_reply_key, complete_query, false);
      completion_src = 2;
    } else {
      return UNKNOWN_COMPLETE;
    }
  } else if (complete_type == ASK_COMPLETE) {
    JoinQueryReplyComplete(itr->second.first, last_reply_key, complete_query, true);
    completion_src = 2;
  } else if (complete_type == ASK_IN_REPLY_COMPLETE) {
    query_autocomplete::SentenceType sentence_type = GetSentenceType(last_reply_key);
    if (sentence_type == INTERR || sentence_type == NORM_INTERR ||
        sentence_type == SELECTED_INTERR) {
      JoinQueryReplyComplete(itr->second.first, last_reply_key, complete_query, true);
      completion_src = 2;
    } else {
      return UNKNOWN_COMPLETE;
    }
  } else if (complete_type == REPLY_REPLACE_COMPLETE) {
    *complete_query = last_reply_key;
    completion_src = 2;
  } else if (complete_type == QUERY_REPLACE_COMPLETE) {
    *complete_query = last_query;
    completion_src = 1;
  } else if (complete_type == ADD_LAST_QUERY_COMPLETE) {
    JoinQueryQueryComplete(itr->second.first, last_query, complete_query);
    completion_src = 1;
  } else if (complete_type == SECOND_LAST_QUERY) {
    query_autocomplete::SentenceType sentence_type = GetSentenceType(last_query);
    if (sentence_type == INTERR || sentence_type == NORM_INTERR) {
      *complete_query = last_query;
      completion_src = 1;
    } else {
      return UNKNOWN_COMPLETE;
    }
  } else if(complete_type == JOINT_BEFORE_LASTQUERY_COMPLETE) {
   query_autocomplete::SentenceType sentence_type = GetSentenceType(last_reply_key);
    if (sentence_type == INTERR || sentence_type == NORM_INTERR) {
      JoinQueryReplyComplete(itr->second.first, last_reply, complete_query, true);
      completion_src = 2;
    }
    else {
      JoinQueryQueryComplete(itr->second.first, last_query, complete_query, true);
      completion_src = 1;
    }
  }
  if (!complete_query->empty()) {
    PopPunc(*complete_query, {"?", "？"});
  }
  return complete_type; 
}

} // namespace query_autocomplete
