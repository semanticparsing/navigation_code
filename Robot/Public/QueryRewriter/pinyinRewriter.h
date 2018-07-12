
#ifndef ROBOT_PUBLIC_QUERYREWRITER_PINYINREWRITER_H
#define ROBOT_PUBLIC_QUERYREWRITER_PINYINREWRITER_H

#include "Common/Base/scopedPtr.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"
#include "Robot/Public/Utils/pinyinConvert.h"

namespace robot {

class PinyinRewriter : public QueryRewriterBase {

public:
    PinyinRewriter();
    virtual ~PinyinRewriter();

    virtual bool Init(QueryRewriter* p);
    virtual void ApplySentence(robot::RewriteSentence& data);
    virtual Type GetType() { return QueryRewriterBase::PRE_SEGMENT; }

private:
    DISALLOW_COPY_AND_ASSIGN(PinyinRewriter);
};

} // namespace preprocess

#endif // ROBOT_QUERY_PROCESS_HANDLER_REMOVE_STOP_WORD_HANDLER_H
