#include "Robot/Public/QueryRewriter/emotionRemoveRewriter.h"
#include "Common/Base/flags.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Base/logging.h"
#include "Common/Base/stringUtil.h"

DEFINE_string(emotion_file, "", "");

namespace robot {

EmotionRemoveRewriter::EmotionRemoveRewriter(){
}

EmotionRemoveRewriter::~EmotionRemoveRewriter() {
}

bool EmotionRemoveRewriter::Init(QueryRewriter* p) {
    handler_ = p;
    file::SimpleLineReader reader(true);
    std::vector<std::string> lines;
    if (!reader.Open(FLAGS_emotion_file)) {
        VLOG(TRIO_LOG_ERR) << "open FLAGS_emotion_file failed " 
            << FLAGS_emotion_file;
        return false;
    }
    lines.clear();
    reader.ReadLines(&lines);
    for (size_t i = 0; i < lines.size(); ++i) {
        StringToLowerASCII(&lines[i]);
        emoji_trie_.AddString(lines[i], "0");
    }
    emoji_trie_.Build();        

    return true;
}

void EmotionRemoveRewriter::Apply(std::string& data){
    std::vector<std::pair<const std::string*, const std::string*> >matched_emoji;
    emoji_trie_.LongestMatch(data, &matched_emoji);
    string cleaned_data = "";
    int start = 0;
    for (size_t i = 0; i < matched_emoji.size(); ++i) {
        string emoji = *(matched_emoji[i].first);
        size_t pos = data.find(emoji, start);
        if (pos != string::npos) {
            string sub_str = data.substr(start, pos - start);
            start = pos + emoji.size();
            cleaned_data += sub_str;
        }
    }
    if (start < data.size()) {
        cleaned_data += data.substr(start);
    }
    data = cleaned_data;
}

REGISTER_REWRITER_PROCESSHANDLER(EmotionRemoveRewriter)
} // namespace robot
