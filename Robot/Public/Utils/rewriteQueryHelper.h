
#ifndef ROBOT_PUBLIC_UTILS_REWRITE_QUERY_HELPER_H
#define ROBOT_PUBLIC_UTILS_REWRITE_QUERY_HELPER_H

#include <map>
#include <string>

#include "Common/Base/basictypes.h"
#include "Common/Base/hashTables.h"
#include "Robot/Public/serviceHead.h"

namespace robot {

class RewriteQueryHelper {
public:
    RewriteQueryHelper(const RewriteQuery& rewrite_query):rewrite_query_(rewrite_query){}
public:
    void GetQuery(std::string& query);
    void GetBasicSeglist(std::vector<std::string>& seglist);
    void GetPhraseSeglist(std::vector<std::string>& seglist);

private:
    DISALLOW_COPY_AND_ASSIGN(RewriteQueryHelper);

private:
    const RewriteQuery& rewrite_query_;
};


class RewriteQueryModifier {
public:
    RewriteQueryModifier(RewriteQuery& rewrite_query) :rewrite_query_(rewrite_query) {}
public:
    void RemoveStopWordBasic(const common::hash_set<std::string>& stopwords_set);

private:
    DISALLOW_COPY_AND_ASSIGN(RewriteQueryModifier);

private:
    RewriteQuery& rewrite_query_;
};

} // namespace robot

#endif // ROBOT_PUBLIC_UTILS_DEBUG_HELPER_H
