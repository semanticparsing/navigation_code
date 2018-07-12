
#ifndef ROBOT_PUBLIC_UTILS_DATETIMENER_DATETIMENER_H_
#define ROBOT_PUBLIC_UTILS_DATETIMENER_DATETIMENER_H_

#include <map>
#include <string>

#include "Robot/Public/FrameDman/SLU/frame_semantic_parser/Parser.h"
#include "Robot/Public/FrameDman/SLU/frame_semantic_parser/LenScorer.h"
#include "Robot/Public/FrameDman/SLU/frame_semantic_parser/QueryPreProcesserWrapper.h"
#include "Common/Base/trio.h"
#include <iostream>

namespace robot {

class DateTimeNER {
 public:
  DateTimeNER() {}
  virtual ~DateTimeNER() {}
  bool Init(
      const std::string& dir_path,
      const std::string& dic_file,
      const std::string& net_file,
      const std::string& frame_file);
  bool Recognise(const std::string& query, std::map<std::string, std::string>* slot_value);

 private:
  scoped_ptr<FrameSemantic::GrammarGraph> grammar_graph_;
  std::map<std::string, std::string> mapping_;
  DISALLOW_COPY_AND_ASSIGN(DateTimeNER);
};

} // namespace robot

#endif // ROBOT_PUBLIC_UTILS_DATETIMENER_DATETIMENER_H_
