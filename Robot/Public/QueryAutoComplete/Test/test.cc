#include <string>
#include <vector>
#include "Robot/Public/QueryAutoComplete/Src/query_autocomplete.h"
#include "Common/Base/stringUtil.h"
#include "Common/Base/flags.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Common/Util/File/simpleLineWriter.h"

DEFINE_string(input_file, "", "");
DEFINE_string(output_file, "", "");
DEFINE_string(output_file1, "", "");

using std::vector;
using std::string;

int main(int argc, char** argv) {
  common::ParseCommandLineFlags(&argc, &argv, false);
  query_autocomplete::QueryAutoComplete auto_complete;
  auto_complete.Init();
  file::SimpleLineReader reader(true);
  if (!reader.Open(FLAGS_input_file)) {
    VLOG(1) << "open input file failed.";
    return -1;
  }
  vector<string> lines;
  reader.ReadLines(&lines);
  file::SimpleLineWriter writer(FLAGS_output_file, false);
  file::SimpleLineWriter writer1(FLAGS_output_file1, false);
  for (size_t i = 0; i < lines.size(); ++i) {
    vector<string> items;
    SplitString(lines[i], '\t', &items);
    if (items.size() != 3) {
      continue;
    }
    vector<string> query_secs;
    SplitString(items[2], '\002', &query_secs);
    string query = query_secs[0];
    string last_query = items[0];
    string last_reply = items[1];
    string complete_query("");
    query_autocomplete::CompleteType complete_type =
        auto_complete.Run(query, last_query, last_reply, &complete_query);
    if (query_autocomplete::UNKNOWN_COMPLETE != complete_type) {
      writer.WriteLine(last_query + '\t' + last_reply + '\t' +
                       query +'\t' + complete_query);
    } else {
      writer1.WriteLine(lines[i]);
	}
  }
  return 0;
}
