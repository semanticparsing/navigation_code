// Copyright (c) 2017 Trio.com, Inc. All Rights Reserved.
#ifndef ROBOT_PUBLIC_QUERYREWRITER_QUERYREWRITER_H_
#define ROBOT_PUBLIC_QUERYREWRITER_QUERYREWRITER_H_

#include <vector>
#include "Common/Util/Registerer/singleton.h"
#include "Robot/Public/Utils/singletonWordSegTag.h"
#include "Robot/Public/QueryRewriter/rewriterPolicy.h"
#include "Robot/Public/QueryRewriter/autoCompleteProcessor.h"
#include "Robot/Public/QueryRewriter/segmentProcessor.h"
#include "Robot/Public/QueryRewriter/sentenceProcessor.h"

namespace robot {

class QueryRewriterBase;
class TrioDataBundle;

/// \brief Query预处理类
class QueryRewriter {
public:
    QueryRewriter();
    virtual ~QueryRewriter();

    /// \brief Init 初始化，失败时会抛出异常退出程序
    ///
    /// \param policy 预处理策略
    void Init(const RewriterPolicyBase* policy);

    /// \brief Apply 执行预处理操作
    ///
    /// \param query_info 需要预处理的对象
    void Apply(robot::TrioDataBundle& query_info);

private:
    bool InitCommonResource();
    void PushRewriter(QueryRewriterBase* rewriter);

private:
    std::vector<QueryRewriterBase*> rewrters_;
    AutoCompleteProcessor auto_complete_;
    SentenceProcessor sentence_;
    SegmentProcessor segment_;
};

typedef Singleton<QueryRewriter> SingletonQueryRewriter;

}  // namespace robot

#endif  // ROBOT_PUBLIC_QUERYREWRITER_QUERYREWRITER_H_
