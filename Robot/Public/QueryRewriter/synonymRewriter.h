#ifndef ROBOT_PUBLIC_QUERYREWRITER_SYNONYM_REWRITER_H
#define ROBOT_PUBLIC_QUERYREWRITER_SYNONYM_REWRITER_H

#include <map>
#include <string>
#include "Common/Base/hashTables.h"
#include "Common/Util/Trie/jumpTrie.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"

namespace robot {

class SynonymRewriter : public QueryRewriterBase {

public:
    SynonymRewriter();
    virtual ~SynonymRewriter();

    virtual bool Init(QueryRewriter* p);
    virtual void ApplySentence(robot::RewriteSentence& data);
    virtual Type GetType() { return QueryRewriterBase::POST_SEGMENT; }

private:
    bool BuildSynonymsTermTrie(const string& synonym_file, trie::KvJumpTrie<string>& jump_trie);

    void ReplaceSynonymsStrByTrie(std::string& words);
    void TransferSynonymsStrByTrie(std::string& basic_seg);

private:
    trie::KvJumpTrie<string> synonyms_term_trie_;
    trie::KvJumpTrie<string> synonyms_transfer_trie_;
    QueryRewriter* handler_;

    DISALLOW_COPY_AND_ASSIGN(SynonymRewriter);
};

} // namespace robot

#endif // ROBOT_PUBLIC_QUERYREWRITER_SENTENCE_REWRITER_H
