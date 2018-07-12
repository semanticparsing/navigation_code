#ifndef ROBOT_PUBLIC_QUERYREWRITER_DUPLICATE_REMOVE_REWRITER_H
#define ROBOT_PUBLIC_QUERYREWRITER_DUPLICATE_REMOVE_REWRITER_H

#include <map>
#include <string>
#include <vector>
#include "Common/Base/hashTables.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"

namespace robot {

class DuplicateRemoveRewriter : public QueryRewriterBase {

public:
    DuplicateRemoveRewriter();
    virtual ~DuplicateRemoveRewriter();

    virtual bool Init(QueryRewriter* p);
    virtual Type GetType() { return QueryRewriterBase::PRE_COMPLETE; }
    virtual void Apply(std::string& data);

private:
    common::hash_map<std::string, int> reduplication_map_;

    QueryRewriter* handler_;

    DISALLOW_COPY_AND_ASSIGN(DuplicateRemoveRewriter);
};

} // namespace robot

#endif // ROBOT_PUBLIC_QUERYREWRITER_SENTENCE_REWRITER_H
