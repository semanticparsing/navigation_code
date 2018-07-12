#ifndef ROBOT_PUBLIC_QUERYREWRITER_SEGMENT_PROCESSOR_H
#define ROBOT_PUBLIC_QUERYREWRITER_SEGMENT_PROCESSOR_H

#include <map>
#include <string>
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"

namespace robot {

class QueryRewriter;

class SegmentProcessor{

public:
    SegmentProcessor();
    virtual ~SegmentProcessor();

    virtual bool Init(QueryRewriter* p);
    virtual void Run(robot::RewriteSentence& data);
    virtual void ResegRun(robot::RewriteSentence& data);

private:
    QueryRewriter* handler_;

    DISALLOW_COPY_AND_ASSIGN(SegmentProcessor);
};

} // namespace robot

#endif // ROBOT_PUBLIC_QUERYREWRITER_SEGMENT_REWRITER_H
