#ifndef ROBOT_PUBLIC_UTILS_PINYIN_CONVERT_H
#define ROBOT_PUBLIC_UTILS_PINYIN_CONVERT_H

#include <string>
#include <vector>
#include "Common/Base/hashTables.h"
#include "Common/Base/basictypes.h"
#include "Robot/Public/Utils/puncOper.h"
#include "Common/Base/scopedPtr.h"

namespace robot {


class PinyinConvert {
public:
    PinyinConvert() {}
    ~PinyinConvert() {}
public:
    struct SplitConvertResult {
        std::string pure_digit;
        std::string pure_pinyin;
        std::string pure_hanzi;
        std::string blend_pinyin_hanzi;
        std::string blend_punc_hanzi;

        void Reset(int reserve) {
            pure_digit.clear();
            pure_digit.reserve(reserve);
            pure_pinyin.clear();
            pure_pinyin.reserve(reserve);
            pure_hanzi.clear();
            pure_hanzi.reserve(reserve);
            blend_pinyin_hanzi.clear();
            blend_pinyin_hanzi.reserve(reserve);
            blend_punc_hanzi.clear();
            blend_punc_hanzi.reserve(reserve);
        }
    };

public:
    bool Init();

    void ConvertPyWord(const std::string& src, std::string& dst);

    void ConvertPyChar(const std::string& src, std::string& dst);

    void Convert(const std::string& src, std::string& dst);

    void SplitConvert(const std::string& src, SplitConvertResult& result);

private:
    bool InitPinyinMap(const std::string& pinyin_file);
    void seg_pinyin_opt(const std::string& pinyin_str, std::string& seg_str);
 
private:
    common::hash_map<std::string, std::string> pinyin_map_;
    common::hash_set<std::string> pinyin_set_;

    DISALLOW_COPY_AND_ASSIGN(PinyinConvert);
};

typedef Singleton<PinyinConvert> SingletonPinyinConvert;

} // namespace robot

#endif // ROBOT_QUERY_PROCESS_HANDLER_REMOVE_STOP_WORD_HANDLER_H
