#include "Robot/Public/QueryRewriter/puncRemoveRewriter.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Robot/Public/Utils/trioLog.h"
#include "Robot/Public/Utils/puncOper.h"

namespace robot {

PuncRemoveRewriter::PuncRemoveRewriter(){
}

PuncRemoveRewriter::~PuncRemoveRewriter(){
}

bool PuncRemoveRewriter::InitVoid() {
    CHECK(SingletonPuncOper::get()) << "singleton punc oper null";
    CHECK(SingletonPuncOper::get()->Init()) << "singleton punc oper init failed";
    return true;
}

bool PuncRemoveRewriter::Init(QueryRewriter* p) {
    CHECK(SingletonPuncOper::get()) << "singleton punc oper null";
    CHECK(SingletonPuncOper::get()->Init()) << "singleton punc oper init failed";
    return true;
}

void PuncRemoveRewriter::ApplySentence(robot::RewriteSentence& data) {
    SingletonPuncOper::get()->ReplacePunc("", &data.senten);
}

void PuncRemoveRewriter::Apply(string& data) {
    SingletonPuncOper::get()->ReplacePunc("", &data);
}

REGISTER_REWRITER_PROCESSHANDLER(PuncRemoveRewriter)

} // namespace robot
