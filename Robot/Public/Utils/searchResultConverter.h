// Copyright (c) 2017 Trio.ai. All rights reserved.
#ifndef ROBOT_PUBLIC_UTILS_SEARCHRESULTCONVERTER_H_
#define ROBOT_PUBLIC_UTILS_SEARCHRESULTCONVERTER_H_

#include <string>
#include <vector>
#include "Common/Util/Search/trioSearch.h"
using std::string;
namespace robot {

bool ParseReplys(
    const search::SearchResult& raw_result,
    std::vector<std::string>* replys) {
  const std::string& json_str = raw_result.result;
  // VLOG(0) << "es reply:" << json_str;
  const string& reply_str = "\"reply\"";
  size_t pos = 0;
  replys->clear();
  while (pos != string::npos) {
    pos = json_str.find(reply_str, pos);
    if (pos == string::npos) break;
    pos = json_str.find("\"", pos + reply_str.size());
    if (pos == string::npos) return false;
    size_t reply_start_pos = ++pos;
    if (pos > (json_str.size() - 1)) return false;
    while (pos != string::npos) {
      pos = json_str.find("\"", pos);
      if (pos == string::npos) return false;
      if (json_str[pos-1] != '\\') {
        break;
      }
      if (++pos > (json_str.size() - 1)) return false;
    }
    size_t reply_end_pos = pos - 1;
    if (reply_end_pos < reply_start_pos) continue;
    string reply_str =
      json_str.substr(reply_start_pos, reply_end_pos - reply_start_pos + 1);
    // VLOG(0) << "reply str:" << reply_str;
    SplitStringUsingSubstr(reply_str, "##triochat##", replys);
    if (++pos > (json_str.size() - 1)) return false;
  }
  return (!replys->empty());
}
}  // namespace robot
#endif  // ROBOT_PUBLIC_UTILS_SEARCHRESULTCONVERTER_H_
