#ifndef ROBOT_PUBLIC_QUERYREWRITER_COMMONCHAR_REWRITER_H
#define ROBOT_PUBLIC_QUERYREWRITER_COMMONCHAR_REWRITER_H

#include "Robot/Public/QueryRewriter/queryRewriterBase.h"
#include "Common/Base/scopedPtr.h"

namespace robot {

class CommonCharRewriter : public QueryRewriterBase {
public:
    CommonCharRewriter();
    virtual ~CommonCharRewriter();

public:
    virtual bool Init(QueryRewriter* p);
    virtual bool InitVoid();
    virtual void Apply(std::string& data);
    virtual Type GetType() { return QueryRewriterBase::PRE_COMPLETE; }

private:
    QueryRewriter* handler_;
};

} //robot

#endif  // ROBOT_PUBLIC_QUERYREWRITER_COMMONCHAR_REWRITER_H
