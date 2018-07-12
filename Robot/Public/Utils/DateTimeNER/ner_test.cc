/***************************************************************************
 * 
 * Copyright (c) 2017 Trio.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file parser_test.cc
 * @author liwei(liwei@trio.ai)
 * @date 2017/09/11 10:24:59
 * @brief 
 *  
 **/

#include "Common/Base/Internal/versionPrint.h"
#include "Robot/Public/Utils/DateTimeNER/dateTimeNER.h"
// #include "Common/Util/FrameSemanticParser/Utils.h"
// #include "Common/Util/FrameSemanticParser/Parser.h"
// #include "Common/Util/FrameSemanticParser/GrammarRuleScorer.h"
#include <iostream>

// DEFINE_string(pre_path, "../Data/Parser", "");
// DEFINE_string(punct, "punct.dic", "");
// DEFINE_string(en_recongizer, "", "");
// DEFINE_string(num_recongizer, "num.dic", "");
// DEFINE_string(spec_punct, "spec_punct.dic", "");

DEFINE_string(dir_path, "../Data", "");
DEFINE_string(dic_file, "base.dic", "");
DEFINE_string(net_file, "DateTimeNER.net", "");
DEFINE_string(frame_file, "frames", "");

int main(int argc, char** argv) {
  InitFlagsAndVersion(&argc, &argv);

  scoped_ptr<robot::DateTimeNER> ner;
  ner.reset(new robot::DateTimeNER());
  CHECK(ner.get());
  CHECK(ner->Init(
        FLAGS_dir_path,
        FLAGS_dic_file,
        FLAGS_net_file,
        FLAGS_frame_file));

  // scoped_ptr<FrameSemantic::GrammarGraph> grammar_graph;
  // grammar_graph.reset(new FrameSemantic::GrammarGraph());
  // CHECK(grammar_graph.get());
  // grammar_graph->init(
  //         FLAGS_dir_path.c_str(),
  //         FLAGS_dic_file.c_str(),
  //         FLAGS_net_file.c_str(),
  //         FLAGS_frame_file.c_str(),
  //         FLAGS_tree_files.c_str());

  string query("");
  std::cout << ">>";
  while (getline(std::cin, query)) {
    std::ostringstream oss;
    map<string, string> slot_value;
    ner->Recognise(query, &slot_value);
    for (auto it = slot_value.begin(); it != slot_value.end(); ++it) {
      oss << " slot:" << it->first << " value:" << it->second << ",";
    }
    string result(oss.str());
    std::cout << "match info:" << result << std::endl;

    std::cout << ">>";
  }
  return 0;
}

