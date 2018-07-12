#include "Robot/Public/Utils/rewriteQueryHelper.h"

namespace robot {

    void RewriteQueryHelper::GetQuery(std::string& query) {
        query.clear();
        if (rewrite_query_.__isset.sentences) {
            for (size_t i = 0; i < rewrite_query_.sentences.size(); i++) {
                if (rewrite_query_.sentences[i].weight > 0) {
                    query += rewrite_query_.sentences[i].senten;
                }
            }
        }
    }
    void RewriteQueryHelper::GetBasicSeglist(std::vector<std::string>& seglist) {
        seglist.clear();
        if (rewrite_query_.__isset.sentences) {
            for (size_t i = 0; i < rewrite_query_.sentences.size(); i++) {
                const RewriteSentence& senten = rewrite_query_.sentences[i];
                if (senten.weight <= 0) {
                    continue;
                }
                for (size_t j = 0; j < senten.basic_segs.size(); j++) {
                    const RewriteWord& word = senten.basic_segs[j];
                    if (word.weight <= 0) {
                        continue;
                    }
                    seglist.push_back(word.word);
                }
            }
        }
    }

    void RewriteQueryHelper::GetPhraseSeglist(std::vector<std::string>& seglist) {
        seglist.clear();
        if (rewrite_query_.__isset.sentences) {
            for (size_t i = 0; i < rewrite_query_.sentences.size(); i++) {
                const RewriteSentence& senten = rewrite_query_.sentences[i];
                if (senten.weight <= 0) {
                    continue;
                }
                for (size_t j = 0; j < senten.phrase_segs.size(); j++) {
                    const RewriteWord& word = senten.phrase_segs[j];
                    if (word.weight <= 0) {
                        continue;
                    }
                    seglist.push_back(word.word);
                }
            }
        }
    }

    void RewriteQueryModifier::RemoveStopWordBasic(const common::hash_set<std::string>& stopwords_set) {  
        if (rewrite_query_.__isset.sentences){
            for (size_t i = 0; i < rewrite_query_.sentences.size(); i++) {
                RewriteSentence& senten = rewrite_query_.sentences[i];
                if (senten.weight <= 0) {
                    continue;
                }
                for (size_t j = 0; j < senten.basic_segs.size(); j++) {
                    RewriteWord& word = senten.basic_segs[j];
                    if (word.weight <= 0) {
                        continue;
                    }
                    if (stopwords_set.find(word.word) != stopwords_set.end()){
                        word.weight = 0;
                    }
                }
            }
        }
    }


} // namespace robot
