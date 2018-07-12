#include "dateTimeNER.h"
#include "Common/Base/stringUtil.h"
#include "Common/Util/File/simpleLineReader.h"

namespace robot {
DEFINE_string(field_mapping,
    "Inform.日期:Date,Inform.时间:Time","");

bool DateTimeNER::Init(
    const std::string& dir_path,
    const std::string& dic_file,
    const std::string& net_file,
    const std::string& frame_file) {
  grammar_graph_.reset(new FrameSemantic::GrammarGraph());
  CHECK(grammar_graph_.get());
  grammar_graph_->init(
      dir_path.c_str(),
      dic_file.c_str(),
      net_file.c_str(),
      frame_file.c_str(),
      "");
  vector<string> segs;
  SplitString(FLAGS_field_mapping, ',', &segs);
  for (size_t i = 0; i != segs.size(); ++i) {
    vector<string> terminal2slot;
    SplitString(segs[i], ':', &terminal2slot);
    if (terminal2slot.size() == 2) {
      mapping_[terminal2slot[0]] = terminal2slot[1];
    }
  }
  return true;
}

bool DateTimeNER::Recognise(const std::string& query, std::map<std::string, std::string>* slot_value) {
  FrameSemantic::PhraseLenScorer len_scorer;
  FrameSemantic::Parser parser(*grammar_graph_, len_scorer, "^", "$");
  parser.reset();
  parser.parse(query.c_str());
  std::ostringstream oss;
  oss.str("");
  parser.extract_parses(oss, '.');
  string extract_result(oss.str());
  VLOG(5) << "extract_info:" << extract_result << std::endl;
  std::vector<string> terminals;
  SplitString(extract_result, '\001', &terminals);
  for (size_t i = 0; i != terminals.size(); ++i) {
    vector<string> kv;
    SplitString(terminals[i], '=', &kv);
    if (kv.size() != 2) continue;
    slot_value->insert(make_pair(
          mapping_.count(kv[0]) ? mapping_[kv[0]] : kv[0], kv[1]));
  }
  return true;
}

} // namespace robot
