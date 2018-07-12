#ifndef ROBOT_PUBLIC_QUERYREWRITER_EMOTION_REMOVE_REWRITER_H
#define ROBOT_PUBLIC_QUERYREWRITER_EMOTION_REMOVE_REWRITER_H

#include <map>
#include <string>
#include <vector>
#include "Common/Util/Trie/jumpTrie.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"

namespace robot {

class EmotionRemoveRewriter : public QueryRewriterBase {

public:
    EmotionRemoveRewriter();
    virtual ~EmotionRemoveRewriter();

    virtual bool Init(QueryRewriter* p);
    virtual Type GetType() { return QueryRewriterBase::PRE_COMPLETE; }
    virtual void Apply(std::string& data);

private:
    trie::KvJumpTrie<string> emoji_trie_;
    QueryRewriter* handler_;

    DISALLOW_COPY_AND_ASSIGN(EmotionRemoveRewriter);
};

} // namespace robot

#endif // ROBOT_PUBLIC_QUERYREWRITER_SENTENCE_REWRITER_H
