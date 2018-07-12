#!/bin/bash

#ulimit -c 10240000000

basepath=$(cd `dirname $0`; pwd)

data_root=${basepath}/../../../Release/data/QueryRewriter/
lib_path=${basepath}/../../../Common/ThirdParty/tensorflow/lib64_release/

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${lib_path}

blade test --testargs "
    --wordsegtag_data_dir=${data_root}/../WordSegTag/data 
    --hot_sentence_file=${data_root}/QueryAutoComplete/other/hot_sentence.data 
    --interr_badcase=${data_root}/QueryAutoComplete/sentence_type_data/norm_interr_badcase.data 
    --interr_re_file=${data_root}/QueryAutoComplete/sentence_type_data/interr.data 
    --nega_badcase_file=${data_root}/QueryAutoComplete/sentence_type_data/negative_badcase.data 
    --nega_re_file=${data_root}/QueryAutoComplete/sentence_type_data/negative.data 
    --norm_interr_re_file=${data_root}/QueryAutoComplete/sentence_type_data/norm_interr.data 
    --norm_interr_special_word=${data_root}/QueryAutoComplete/other/norm_interr_special_word.data 
    --norm_interr_tail_word=${data_root}/QueryAutoComplete/sentence_type_data/rtrim.data 
    --punc_file=${data_root}/QueryAutoComplete/other/punctuation.dat 
    --query_to_complete_file=${data_root}/QueryAutoComplete/query/query_need_complete.data 
    --selected_interr_re_file=${data_root}/QueryAutoComplete/sentence_type_data/selected_interr.data 
    --seperate_file=${data_root}/QueryAutoComplete/other/separator.dat 
    --unpronoun_rev_word=${data_root}/QueryAutoComplete/other/unpronoun_rev.data 
    --global_pinyin_file=${data_root}/QueryPreprocess/pinyin.data 
    --global_punctuation_file=${data_root}/QueryPreprocess/punctuation_utf8.txt 
    --qpp_stop_subsentences=${data_root}/QueryPreprocess/qpp_stop_subsentences.dat 
    --qpp_sentence_separators=${data_root}/QueryPreprocess/sentence_sepatators.dat 
    --reduplication=${data_root}/QueryPreprocess/reduplication.utf8.dat 
    --last_is_most_important=true 
    --qpp_synonyms_transfer=${data_root}/QueryPreprocess/qpp_synonyms_transfer.dat 
    --qpp_synonyms_term=${data_root}/QueryPreprocess/qpp_synonyms_term.dat
    --emotion_file=${data_root}/QueryPreprocess/emoji.txt \
    --nickname_term_file=${data_root}/QueryPreprocess/qpp_production_filted_term.dat \
    --rewriter_chinese_dict_file=${data_root}/../hanzi/chinese.dict
    "
