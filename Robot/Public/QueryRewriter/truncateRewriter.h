#ifndef ROBOT_PUBLIC_QUERYREWRITER_TRUNCATE_REWRITER_H
#define ROBOT_PUBLIC_QUERYREWRITER_TRUNCATE_REWRITER_H

#include <map>
#include <string>
#include "Common/Base/hashTables.h"
#include "Common/Util/Trie/jumpTrie.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"

namespace robot {

class TruncateRewriter : public QueryRewriterBase {

public:
    TruncateRewriter();
    virtual ~TruncateRewriter();

    virtual bool Init(QueryRewriter* p);
    virtual bool InitVoid();
    virtual void ApplySentence(robot::RewriteSentence& data);
    virtual void Apply(string& data);
    virtual Type GetType() { return QueryRewriterBase::POST_SEGMENT; }

private:
    QueryRewriter* handler_;

    DISALLOW_COPY_AND_ASSIGN(TruncateRewriter);
};

} // namespace robot

#endif // ROBOT_PUBLIC_QUERYREWRITER_SENTENCE_REWRITER_H
