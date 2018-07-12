#include "Robot/Public/QueryRewriter/commonCharRewriter.h"
#include "Robot/Public/Utils/singletonTran2Simp.h"

DEFINE_string(rewriter_chinese_dict_file, "", "");


namespace robot {

CommonCharRewriter::CommonCharRewriter(){
}

CommonCharRewriter::~CommonCharRewriter(){
}

bool CommonCharRewriter::InitVoid() {
    CHECK(NULL != SingletonTran2Simp::get());
    CHECK(SingletonTran2Simp::get()->Init(FLAGS_rewriter_chinese_dict_file));
    return true;
}

bool CommonCharRewriter::Init(QueryRewriter* p) {
    handler_ = p;

    CHECK(NULL != SingletonTran2Simp::get());
    CHECK(SingletonTran2Simp::get()->Init(FLAGS_rewriter_chinese_dict_file));

    return true;
}

void CommonCharRewriter::Apply(std::string& data) {

    std::string query;
    common::FullWidthToHalfWidth(data, &query);
    StringToLowerASCII(&query);

    std::string han_src = query;
    SingletonTran2Simp::get()->Convert(han_src, query);

    data = query;
}

REGISTER_REWRITER_PROCESSHANDLER(CommonCharRewriter)

} // namespace robot
