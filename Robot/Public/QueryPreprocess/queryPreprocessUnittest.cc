#include <fstream>
#include <iostream>
#include "Common/ThirdParty/gtest/gtest.h"
#include "Common/Base/logging.h"
#include "Common/Base/flags.h"
#include "Common/Base/time.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Util/ThriftClientWrapper/thrift.h"

#include "Robot/Public/Proto/struct_types.h"
#include "removeStopWordHandler.h"

DEFINE_string(test_input, "", "");

using namespace preprocess;

TEST(RemoveStopWordHandler, Process) {
  RemoveStopWordHandler remove_handler;
  EXPECT_EQ(remove_handler.Init(), true);
  file::SimpleLineReader reader(FLAGS_test_input);
  
  std::vector<std::string> lines;
  reader.ReadLines(&lines);

  
  for (size_t i = 0; i < lines.size(); ++i) {
    QueryInfo query_info;
    query_info.query = lines[i];
    remove_handler.ProcessDoc(query_info);
  }
}


