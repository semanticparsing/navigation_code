#ifndef PUBLIC_QUERYREWRITER_REWRITERPOLICY_H
#define PUBLIC_QUERYREWRITER_REWRITERPOLICY_H

#include <string>
#include <vector>
#include "Common/Base/trio.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"

namespace robot {

class RewriterPolicyBase {
protected:
	RewriterPolicyBase() {}
public:
    std::vector<std::string> rewriter_names;
};

class RewriterPolicyDefault : public RewriterPolicyBase
{
public:
	RewriterPolicyDefault() {
        rewriter_names.clear();
        rewriter_names.push_back("CommonCharRewriter");
        rewriter_names.push_back("EmotionRemoveRewriter");
        rewriter_names.push_back("DuplicateRemoveRewriter");
        rewriter_names.push_back("NicknameRemoveRewriter");
        rewriter_names.push_back("PinyinRewriter");
        rewriter_names.push_back("PuncRemoveRewriter");
        rewriter_names.push_back("SynonymRewriter");
        rewriter_names.push_back("TruncateRewriter"); 
	}
};

} // namespace robot

#endif // PUBLIC_WORDFILTER_POLICY_BASE_H
