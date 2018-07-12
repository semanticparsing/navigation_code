// Copyright (c) 2017 Trio.com, Inc. All Rights Reserved.
/// \file queryRewriter.cc
/// \brief Query预处理模块
/// \author YuHui, yuhui@trio.com
/// \version 1.0
/// \date 2017-08-07
#include "Robot/Public/QueryRewriter/queryRewriter.h"
#include <string>
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"
#include "Common/Base/stringUtil.h"
#include "Robot/Public/UserContext/userContext.h"

namespace robot {

QueryRewriter::QueryRewriter() {
}

QueryRewriter::~QueryRewriter() {
    for (size_t i =0; i < rewrters_.size(); ++i) {
        if (rewrters_[i] != NULL) {
            delete rewrters_[i];
            rewrters_[i] = NULL;
        }
    }
}

void QueryRewriter::Init(const RewriterPolicyBase* policy) {
    CHECK(InitCommonResource()) <<
        "query rewriter init common resource failed" << endl;

    CHECK(auto_complete_.Init(this)) << "auto complete init failed" << endl;

    CHECK(sentence_.Init(this)) << "sentence init failed" << endl;

    CHECK(segment_.Init(this)) << "segmentor init failed" << endl;

    CHECK(NULL != policy) << "rewriter policy is null" << endl;

    const std::vector<std::string>& rewriter_names = policy->rewriter_names;

    QueryRewriterBase* rewriter = NULL;
    for (size_t i = 0; i < rewriter_names.size(); ++i) {
        std::string name = rewriter_names[i];
        rewriter = QueryRewriterBaseRegisterer::GetInstanceByName(name);
        CHECK(NULL != rewriter) << "query rewriter instance null " <<
            name << endl;
        CHECK(rewriter->Init(this)) "query rewriter init failed " <<
            name << endl;
        PushRewriter(rewriter);
        VLOG(TRIO_LOG_STAT) << "query rewriter init sucess " << name;
    }
}

void QueryRewriter::Apply(robot::TrioDataBundle& data) {
    std::string raw_query = data.query;

    std::string pre_multi_query = data.query;
    for (size_t i = 0; i < rewrters_.size(); i++) {
        if (rewrters_[i]->GetType() == QueryRewriterBase::PRE_COMPLETE) {
            rewrters_[i]->Apply(pre_multi_query);
        }
    }

    // auto complete
    std::string auto_complete_query = pre_multi_query;
    query_autocomplete::CompleteType auto_complete_type =
        query_autocomplete::UNKNOWN_COMPLETE;
    if (data.__isset.user_ctx && data.user_ctx.__isset.user_context
        && !data.user_ctx.user_context.empty()) {
        string last_query, last_reply;
        UserContextHelper::GetLastQueryReply(data.user_ctx,
            last_query, last_reply);
        auto_complete_type = auto_complete_.Run(pre_multi_query,
            last_query, last_reply, &auto_complete_query);
        // for safe, run should do this when not auto complete
        if (auto_complete_query.empty()) {
            VLOG(TRIO_LOG_ERR) << "auto complete return empty string " <<
                pre_multi_query << ", " << last_query << ", " <<
                last_reply << ", " << auto_complete_type;
            auto_complete_query = pre_multi_query;
        }
    }

    for (size_t i = 0; i < rewrters_.size(); i++) {
        if (rewrters_[i]->GetType() == QueryRewriterBase::POST_COMPETE) {
            rewrters_[i]->Apply(pre_multi_query);
            rewrters_[i]->Apply(auto_complete_query);
        }
    }

    RewriteQuery rewrite_raw;
    sentence_.Run(raw_query, false, rewrite_raw);

    RewriteQuery rewrite_prep;
    sentence_.Run(pre_multi_query, false, rewrite_prep);

    RewriteQuery rewrite_auto_complete_raw;
    sentence_.Run(auto_complete_query, auto_complete_type !=
                  query_autocomplete::UNKNOWN_COMPLETE,
                  rewrite_auto_complete_raw);

    RewriteQuery rewrite_auto_complete_prep;
    sentence_.Run(auto_complete_query, auto_complete_type !=
                  query_autocomplete::UNKNOWN_COMPLETE,
                  rewrite_auto_complete_prep);

    std::vector<RewriteQuery*> rewrite_querys;
    rewrite_querys.push_back(&rewrite_raw);
    rewrite_querys.push_back(&rewrite_auto_complete_raw);
    rewrite_querys.push_back(&rewrite_prep);
    rewrite_querys.push_back(&rewrite_auto_complete_prep);

    for (size_t i = 0; i < rewrite_querys.size(); i++) {
        for (size_t s = 0; s < rewrite_querys[i]->sentences.size(); s++) {
            RewriteSentence& senten = rewrite_querys[i]->sentences[s];
            // zero and one is raw, not need presegment rewrite
            if (i > 1) {
                for (size_t k = 0; k < rewrters_.size(); k++) {
                    if (rewrters_[k]->GetType() ==
                        QueryRewriterBase::PRE_SEGMENT) {
                        rewrters_[k]->ApplySentence(senten);
                    }
                }
            }
            segment_.Run(senten);
            // zero and one is raw, not need postsegment rewrite
            if (i > 1) {
                for (size_t k = 0; k < rewrters_.size(); k++) {
                    if (rewrters_[k]->GetType() ==
                        QueryRewriterBase::POST_SEGMENT) {
                        rewrters_[k]->ApplySentence(senten);
                    }
                }
                segment_.ResegRun(senten);
            }
        }
    }

    robot::MultiRewrite multi;
    multi.__set_raw(rewrite_raw);
    multi.__set_prep(rewrite_prep);
    multi.__set_auto_complete_raw(rewrite_auto_complete_raw);
    multi.__set_auto_complete_prep(rewrite_auto_complete_prep);
    multi.__set_auto_complete_type(static_cast<int>(auto_complete_type));

    data.__set_rewrite_data(multi);
}

void QueryRewriter::PushRewriter(QueryRewriterBase* rewriter) {
    if (NULL != rewriter) {
        rewrters_.push_back(rewriter);
    }
}

bool QueryRewriter::InitCommonResource() {
    WordSegPuncTagging* t = SingletonWordSegTag::get();
    if (NULL == t) {
        VLOG(TRIO_LOG_ERR) <<
            "segment rewriter get wordsegtag singleton failed" << endl;
        return false;
    }
    if (!t->Init()) {
        VLOG(TRIO_LOG_ERR) <<
            "segment rewriter wordsegtag signleton init failed" << endl;
        return false;
    }
    return true;
}

}  // namespace robot
