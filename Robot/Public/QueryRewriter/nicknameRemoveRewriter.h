#ifndef ROBOT_PUBLIC_QUERYREWRITER_NICKNAME_REMOVE_REWRITER_H
#define ROBOT_PUBLIC_QUERYREWRITER_NICKNAME_REMOVE_REWRITER_H

#include <map>
#include <string>
#include <vector>
#include "Common/Base/hashTables.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"

namespace robot {

class NicknameRemoveRewriter : public QueryRewriterBase {

public:
    NicknameRemoveRewriter();
    virtual ~NicknameRemoveRewriter();

    virtual bool Init(QueryRewriter* p);
    virtual Type GetType() { return QueryRewriterBase::PRE_COMPLETE; }
    virtual void Apply(std::string& data);

private:
    common::hash_set<std::string> production_filted_term_;

    QueryRewriter* handler_;

    DISALLOW_COPY_AND_ASSIGN(NicknameRemoveRewriter);
};

} // namespace robot

#endif // ROBOT_PUBLIC_QUERYREWRITER_SENTENCE_REWRITER_H
