#include "Robot/Public/Utils/pinyinConvert.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Base/stringUtil.h"
#include "Common/Base/flags.h"
#include "Robot/Public/Utils/trioLog.h"

DEFINE_string(global_pinyin_file, "../Data/QueryRewriter/QueryPreprocess/pinyin.data", "convert han to pinyin map");

namespace robot {

bool PinyinConvert::Init() {
    if (!InitPinyinMap(FLAGS_global_pinyin_file)) {
        VLOG(TRIO_LOG_ERR) << "pinyin convert init map failed " << FLAGS_global_pinyin_file << endl;
        return false;
    }
    CHECK(NULL != SingletonPuncOper::get()) << "punc oper new failed" << endl;
    if (!SingletonPuncOper::get()->Init()) {
        VLOG(TRIO_LOG_ERR) << "punc oper init failed " << endl;
        return false;
    }
    return true;
}

void PinyinConvert::ConvertPyWord(const std::string& src, std::string& dst) {
  std::vector<std::string> chars;
  common::CharSegment(src, chars);

  dst.clear();
  dst.reserve(src.length() * 6);

  common::hash_map<std::string, std::string>::const_iterator c_it;
  for (size_t i = 0; i < chars.size(); ++i) {
    std::string& cur_char = chars[i];
    c_it = pinyin_map_.find(cur_char);
    if (c_it != pinyin_map_.end()) {
      dst += c_it->second;
    }
    else {
      dst += cur_char;
    }
  }
}

void PinyinConvert::ConvertPyChar(const std::string& src, std::string& dst) {
  std::string charsstr = src;
  ReplaceSubstringsAfterOffset(&charsstr, 0, " ", "");
  std::vector<std::string> chars;
  common::CharSegment(charsstr, chars);

  std::vector<std::string> py_chars;
  py_chars.reserve(charsstr.size());

  common::hash_map<std::string, std::string>::const_iterator c_it;
  for (size_t i = 0; i < chars.size(); ++i) {
    std::string& cur_char = chars[i];
    c_it = pinyin_map_.find(cur_char);
    if (c_it != pinyin_map_.end()) {
      py_chars.push_back(c_it->second);
    }
    else {
      py_chars.push_back(cur_char);
    }
  }

  dst = JoinString(py_chars, ' ');
}

void PinyinConvert::Convert(const std::string& src_query, std::string& dst_pinyin) {
    std::string query = src_query;
    std::vector<std::string> chars;
    common::CharSegment(query, chars);

    std::vector<std::string> pinyin_words;
    std::string alpha_str;
    alpha_str.reserve(src_query.length());

    dst_pinyin.clear();
    dst_pinyin.reserve(src_query.length() * 10);

    std::string seg_pinyin_str;
    seg_pinyin_str.reserve(src_query.length() * 10);

    for (size_t i = 0; i < chars.size(); ++i) {
        std::string& cur_char = chars[i];
        if (!SingletonPuncOper::get()->IsPunc(cur_char)) {
            if (IsAsciiAlpha(cur_char)) {
                alpha_str += cur_char;
                continue;
            }
            if (!alpha_str.empty()) {
                seg_pinyin_opt(alpha_str, seg_pinyin_str);
                common::JoinAppendString(seg_pinyin_str, ' ', dst_pinyin);
                alpha_str.clear();
            }
            common::hash_map<std::string, std::string>::const_iterator c_it = pinyin_map_.find(cur_char);
            if (c_it != pinyin_map_.end()) {
                common::JoinAppendString(c_it->second, ' ', dst_pinyin);
            }
            else {
                common::JoinAppendString(cur_char, ' ', dst_pinyin);
                VLOG(TRIO_LOG_ERR) << "pinyin rewriter char miss: " << chars[i];
            }
        }
    }
    if (alpha_str != "") {
        seg_pinyin_opt(alpha_str, seg_pinyin_str);
        common::JoinAppendString(seg_pinyin_str, ' ', dst_pinyin);
    }
}

void PinyinConvert::SplitConvert(const std::string& src, SplitConvertResult& result) {

    result.Reset(src.length());

    std::vector<string> chars;
    common::CharSegment(src, chars);

    for (size_t i = 0; i < chars.size(); ++i) {
        std::string& cur_char = chars[i];
        if (SingletonPuncOper::get()->IsPunc(cur_char)) {
            result.blend_punc_hanzi += cur_char;
        }
        else if (IsAsciiAlpha(cur_char)) {
            result.pure_pinyin += cur_char;
            result.blend_pinyin_hanzi += cur_char;
        }
        else if (isdigit(cur_char[0])) {
            result.pure_digit += cur_char;
        }
        else {
            result.pure_hanzi += cur_char;
            result.blend_pinyin_hanzi += cur_char;
            result.blend_punc_hanzi += cur_char;
        }
    }

    seg_pinyin_opt(result.pure_pinyin, result.pure_pinyin);
}

void PinyinConvert::seg_pinyin_opt(const std::string& pinyin_str, std::string& seg_str) {
    int i = 0;
    seg_str.clear();
    while (i < pinyin_str.size()) {
        vector<pair<string, int> > sub_pinyin_vec;
        sub_pinyin_vec.push_back(make_pair(pinyin_str.substr(i, 1), 1));
        int j = i + 1;
        while (j < pinyin_str.size()) {
            string sub_pinyin = pinyin_str.substr(i, j - i + 1);
            if (pinyin_set_.find(sub_pinyin) != pinyin_set_.end()) {
                sub_pinyin_vec.push_back(make_pair(sub_pinyin, j - i + 1));
            }
            ++j;
        }
        pair<string, int> last = sub_pinyin_vec.back();
        common::JoinAppendString(last.first, ' ', seg_str);
        i += last.second;
    }
}


bool PinyinConvert::InitPinyinMap(const std::string& pinyin_file) {
    file::SimpleLineReader reader(true);
    if (!reader.Open(pinyin_file)) {
        VLOG(TRIO_LOG_ERR) << "pinyin convert open file failed: " << pinyin_file << endl;
        return false;
    }
    vector<string> lines;
    reader.ReadLines(&lines);
    vector<string> cols;
    for (size_t i = 0; i < lines.size(); ++i) {
        cols.clear();
        SplitString(lines[i], ' ', &cols);
        if (cols.size() != 2) {
            VLOG(TRIO_LOG_ERR) << "pinyin convert read line error, size: " << cols.size() << ", " << lines[i] << endl;
            continue;
        }
        pinyin_map_[cols[0]] = cols[1];
        pinyin_set_.insert(StringToLowerASCII(cols[1]));
    }
    return true;
}

} // namespace robot
