#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <vector>

#include "Common/Base/flags.h"
#include "Common/Base/stringUtil.h"
#include "Common/Base/logging.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Util/Trie/jumpTrie.h"
#include "Common/Base/hashTables.h"

DEFINE_string(global_pinyin_file, "", "");
DEFINE_string(porn_pinyin_dict, "", "");
DEFINE_string(input_path, "", "");

using namespace std;
bool Compare(std::pair<string, int> a, std::pair<string, int> b) {
  if (a.second > b.second) {
    return true;
  }
  return false;
}

bool BuildPinyinTrie(const string& pinyin_dict_path, trie::KvJumpTrie<std::string>& general_political_word_pinyin_trie) {
  vector<string> lines;
  lines.clear();
  file::SimpleLineReader reader;
  if (!reader.Open(pinyin_dict_path)) {
    VLOG(1) << "open general pornWords pinyin file failed.";
    return false;
  }
  reader.ReadLines(&lines);
  for (size_t i = 0; i < lines.size(); ++i) {
    vector<string> pinyin_word_vec;
    SplitString(lines[i], '\t', &pinyin_word_vec);
    if (pinyin_word_vec.size() != 2) {
      continue;
    }
    string query_pinyin = pinyin_word_vec[0];
    StringToLowerASCII(&query_pinyin);
    VLOG(6) << "porn word:" << query_pinyin;
    general_political_word_pinyin_trie.AddString(query_pinyin, "0");
  }
  general_political_word_pinyin_trie.Build();
  return true;
}

bool ReadPinYinToMap(const std::string& file_name, common::hash_map<string, string>* pinyin_map, common::hash_set<std::string>& pinyin_set) {
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

    pinyin_set.insert(StringToLowerASCII(cols[1]));
  }
  return true;
}

std::string seg_pinyin(string pinyin_str, const common::hash_set<std::string>& pinyin_set) {
  int i = 0;
  vector<string> res;
  while (i < pinyin_str.size()) {
    vector<pair<string, int> > sub_pinyin_vec;

    sub_pinyin_vec.push_back(make_pair(pinyin_str.substr(i, 1), 1));
    int j = i + 1;
    while( j < pinyin_str.size()) {
      string sub_pinyin = pinyin_str.substr(i, j - i + 1);
      if (pinyin_set.find(sub_pinyin) != pinyin_set.end()) {
        sub_pinyin_vec.push_back(make_pair(sub_pinyin, j - i + 1));
      }
      ++j;
    }
    pair<string, int> last = sub_pinyin_vec.back();
    res.push_back(last.first);
    i += last.second;
  }

  VLOG(6) << "seg pinyin res:" << res;
  return JoinVector(res, ' ');
}

string ConvertQuery2Pinyin(string query, common::hash_map<string, string> pinyin_map, const common::hash_set<std::string>& pinyin_set) {
  StringToLowerASCII(&query);
  std::vector<string> word_vec;
  common::CharSegment(query, word_vec);
  vector<string> query_pinyin_vec;
  string alpha_str = "";
  for (size_t i = 0; i < word_vec.size(); ++i) {
    if (IsAsciiAlpha(word_vec[i])) {
      alpha_str += word_vec[i];
      continue;
    } else if (alpha_str != "") {
      VLOG(6) << "alpha_str:" << alpha_str;
      query_pinyin_vec.push_back(seg_pinyin(alpha_str, pinyin_set));
      alpha_str = "";
    }
    common::hash_map<string, string>::const_iterator c_it = pinyin_map.find(word_vec[i]);
    if (c_it != pinyin_map.end()) {
      query_pinyin_vec.push_back(c_it->second);
    } else {
      query_pinyin_vec.push_back(word_vec[i]);
      VLOG(6) << "not found pinyin:" << word_vec[i];
    }
  }

  if (alpha_str != "") {
    VLOG(6) << "alpha_str1:" << alpha_str;
    query_pinyin_vec.push_back(seg_pinyin(alpha_str, pinyin_set));
    alpha_str = "";
  }
  string query_pinyin = JoinVector(query_pinyin_vec, ' ');
  VLOG(6) << "query_pinyin:" << query_pinyin;
  return query_pinyin;
}


bool isGeneralPoliticalPinyin(const string& sentence, const std::string& query, trie::KvJumpTrie<std::string>& general_political_word_pinyin_trie, common::hash_map<string, int>& match_word_map) {
  std::vector<std::pair<const std::string*, const std::string*> > political_words;
  political_words.clear();
  general_political_word_pinyin_trie.LongestMatch(sentence, &political_words);
  if (political_words.size() > 0) {
    for (size_t k = 0; k < political_words.size(); ++k) {
      string word = *(political_words[k].first);
      int start = sentence.find(word);
      int end = start + word.size();
      if ((start == 0 || sentence[start - 1] == ' ') && (end == sentence.size() || sentence[end] == ' ')) {
        VLOG(5) << "raw:" << query << " match word:" << word;
        VLOG(6) << "sentence:" << sentence << "filted by general political sensitive word";
        if (match_word_map.find(word) == match_word_map.end()) {
          match_word_map[word] = 0;
        }
        ++match_word_map[word];
        return true;
      }
    }
  }
  return false;
}

int main(int  argc, char** argv) {
  common::ParseCommandLineFlags(&argc, &argv, false);
  trie::KvJumpTrie<std::string> general_political_word_pinyin_trie;
  BuildPinyinTrie(FLAGS_porn_pinyin_dict, general_political_word_pinyin_trie);
  common::hash_map<string, string> pinyin_map;
  common::hash_set<std::string> pinyin_set;
  ReadPinYinToMap(FLAGS_global_pinyin_file, &pinyin_map, pinyin_set);
  VLOG(2) << "input path:" << FLAGS_input_path;
  std::ifstream input(FLAGS_input_path.c_str());
  std::string line;

  common::hash_map<string, int> match_word_dict;
  while(getline(input, line, '\n')) {
    string pinyin_line = ConvertQuery2Pinyin(line, pinyin_map, pinyin_set);
    isGeneralPoliticalPinyin(pinyin_line, line, general_political_word_pinyin_trie, match_word_dict);
  }
  vector<pair<string, int> > match_word_vec;
  for (common::hash_map<string, int>::iterator it = match_word_dict.begin(); it != match_word_dict.end(); ++it) {
    match_word_vec.push_back(make_pair(it->first, it->second));
  }

  sort(match_word_vec.begin(), match_word_vec.end(), Compare);
  for (size_t i = 0; i < match_word_vec.size(); ++i) {
    std::cout << match_word_vec[i].first << "\t" << match_word_vec[i].second << std::endl; 
  }
}
