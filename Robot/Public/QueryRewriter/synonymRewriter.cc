#include "Robot/Public/QueryRewriter/synonymRewriter.h"
#include <vector>
#include <string>
#include "Common/Base/flags.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Util/Encode/iconvWrapper.hpp"
#include "Common/Util/Encode/encodeUtil.h"
#include "Common/Base/logging.h"
#include "Common/Base/stringUtil.h"
#include "Robot/Public/Declare/declearHead.h"
#include "Robot/Public/QueryRewriter/queryRewriter.h"


DEFINE_string(qpp_synonyms_transfer, "../QueryRewriter/QueryPreprocess/qpp_synonyms_transfer.dat", "the synonyms transfer file path");
DEFINE_string(qpp_synonyms_term, "../QueryRewriter/QueryPreprocess/qpp_synonyms_term.dat", "the synonyms term file path");

namespace robot {

SynonymRewriter::SynonymRewriter(){
}

SynonymRewriter::~SynonymRewriter() {
}

bool SynonymRewriter::Init(QueryRewriter* p) {
    handler_ = p;
    if (!BuildSynonymsTermTrie(FLAGS_qpp_synonyms_term, synonyms_term_trie_)) {
        VLOG(0) << "synonym rewriter build term trie failed: " << FLAGS_qpp_synonyms_term << endl;
        return false;
    }
    if (!BuildSynonymsTermTrie(FLAGS_qpp_synonyms_transfer, synonyms_transfer_trie_)) {
        VLOG(0) << "synonym rewriter build transfer trie failed: " << FLAGS_qpp_synonyms_transfer << endl;
        return false;
    }
    return true;
}

bool SynonymRewriter::BuildSynonymsTermTrie(const string& synonym_file, trie::KvJumpTrie<string>& jump_trie) {
    if (synonym_file.empty()) {
        VLOG(0) << "synonym rewriter file empty: " << synonym_file << endl;
        return false;
    }
    file::SimpleLineReader reader(synonym_file, true);
    vector<string> lines;
    reader.ReadLines(&lines);
    std::string terms_str;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (StartsWithASCII(lines[i], "#", false)) {
            continue;
        }
        vector<string> eles;
        SplitString(lines[i], '\t', &eles);
        if (eles.size() < 2) {
            VLOG(0) << "synonym rewriter read line error: " << lines[i];
            return false;
        }
        SingletonWordSegTag::get()->Segment(eles[0], terms_str);
        jump_trie.AddString(" " + terms_str + " ", eles[1]);
    }
    jump_trie.Build();
    return true;
}

void SynonymRewriter::ApplySentence(robot::RewriteSentence& data){

    std::vector<RewriteWord>& words = data.basic_segs;
    RewriteWord word;
    std::vector<std::string> basic_segs;
    for (size_t i = 0; i < words.size(); i++) {
        basic_segs.push_back(words[i].word);
    }
    std::string basic_seg = JoinVector(basic_segs, ' ');
    
    ReplaceSynonymsStrByTrie(basic_seg);
    TransferSynonymsStrByTrie(basic_seg);
    
    basic_segs.clear();
    SplitString(basic_seg, ' ', &basic_segs);
    words.clear();
    for (size_t i=0; i<basic_segs.size(); i++){
        word.__set_weight(1);
        word.__set_word(basic_segs[i]);
        words.push_back(word);
    }
}

void SynonymRewriter::ReplaceSynonymsStrByTrie(std::string& basic_seg) {      
    std::vector<std::pair<const std::string*, const std::string*> > synonyms_terms;
    synonyms_term_trie_.LongestMatch(basic_seg, &synonyms_terms);
    for (size_t i = 0; i < synonyms_terms.size(); ++i) {
        size_t pos = basic_seg.find(*(synonyms_terms[i].first), 0);
        while(pos != std::string::npos) {
            VLOG(5) << "synonym from " << *(synonyms_terms[i].first) << " to " << synonyms_terms[i].second;
            basic_seg.replace(pos, synonyms_terms[i].first->size() - 1, *(synonyms_terms[i].second));
            pos = basic_seg.find(*(synonyms_terms[i].first), pos + synonyms_terms[i].second->size());
        }
    }
}

void SynonymRewriter::TransferSynonymsStrByTrie(std::string& basic_seg) {
    std::vector<std::pair<const std::string*, const std::string*> > synonyms_transfer;
    synonyms_transfer_trie_.LongestMatch(basic_seg, &synonyms_transfer);
    string last_str("");
    string first_str("");
    for (size_t i = 0; i < synonyms_transfer.size(); ++i) {
        string value = *(synonyms_transfer[i].second);
        vector<string> items;
        SplitString(value, '\001', &items);
        if (items.size() < 2) {
            VLOG(0) << "transfer synonym match item size error " << items.size() << endl;
            continue;
        }
        if (items.size() == 2) {
            vector<string> sub_items;
            SplitString(items[1], '\002', &sub_items);
            if (sub_items.size() == 2) {
                first_str = sub_items[1];
                last_str = sub_items[0];
            }
        }
        size_t pos = basic_seg.find(*(synonyms_transfer[i].first), 0);
        while(pos != std::string::npos) {
            VLOG(5) << "transfer synonym touch " << *(synonyms_transfer[i].first);
            basic_seg.replace(pos, synonyms_transfer[i].first->size(), items[0]);
            pos = basic_seg.find(*(synonyms_transfer[i].first), pos + items[0].size());
        }
    }
    if (!last_str.empty()) {
        basic_seg += last_str;
    }
    if (!first_str.empty()) {
        basic_seg = first_str + basic_seg;
    }
}

REGISTER_REWRITER_PROCESSHANDLER(SynonymRewriter)
} // namespace robot
