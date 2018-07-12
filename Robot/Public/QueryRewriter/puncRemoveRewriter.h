#ifndef ROBOT_PUBLIC_QUERYREWRITER_PUNC_REMOVE_REWRITER_H
#define ROBOT_PUBLIC_QUERYREWRITER_PUNC_REMOVE_REWRITER_H

#include "Robot/Public/QueryRewriter/queryRewriterBase.h"

namespace robot {

class PuncRemoveRewriter : public QueryRewriterBase {
public:
    PuncRemoveRewriter();
    virtual ~PuncRemoveRewriter();

public:
    virtual bool Init(QueryRewriter* p);
    virtual bool InitVoid();
    virtual void ApplySentence(robot::RewriteSentence& data);
    virtual void Apply(string& data);
    virtual Type GetType() { return QueryRewriterBase::PRE_SEGMENT; }

private:
    DISALLOW_COPY_AND_ASSIGN(PuncRemoveRewriter);
};

} //robot

#endif  // ROBOT_PUBLIC_QUERYREWRITER_PUNC_REMOVE_REWRITER_H
