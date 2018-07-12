#include "Robot/Public/QueryRewriter/pinyinRewriter.h"
#include "Common/Base/flags.h"
#include "Robot/Public/Declare/declearHead.h"

namespace robot {

PinyinRewriter::PinyinRewriter(){
}

PinyinRewriter::~PinyinRewriter() {
}

bool PinyinRewriter::Init(QueryRewriter* p) {
	CHECK(NULL != SingletonPinyinConvert::get()) << "pinyin convert new failed" << endl;
	CHECK(SingletonPinyinConvert::get()->Init()) << "pinyin convert init failed" << endl;
    return true;
}

void PinyinRewriter::ApplySentence(robot::RewriteSentence& data) {
	SingletonPinyinConvert::get()->Convert(data.senten, data.pinyin);
}

REGISTER_REWRITER_PROCESSHANDLER(PinyinRewriter)
} // namespace robot
