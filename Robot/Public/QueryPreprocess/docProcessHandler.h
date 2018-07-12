
#ifndef ROBOT_INTERFACE_DOC_PROCESS_HANDLER_H
#define ROBOT_INTERFACE_DOC_PROCESS_HANDLER_H

#include <string>
#include "Common/Base/trio.h"
#include "Common/Util/Registerer/registerer.h"

namespace preprocess {
struct QueryInfo {
  std::string query;
  std::string query_pinyin;
  std::string basic_seg;
  std::string phrase_seg;
  std::string preprocessed_query;
  std::string preprocessed_basic_seg;
  std::string preprocessed_phrase_seg;
  bool is_hit_people;
  std::string people_info;
};

class DocProcessHandler {
 public:
   DocProcessHandler() {}
   virtual ~DocProcessHandler(){}

   virtual bool ProcessDoc(QueryInfo& doc) = 0;
   virtual bool Init() {
     return true;
   };

 private:
  DISALLOW_COPY_AND_ASSIGN(DocProcessHandler);
};

REGISTER_REGISTERER(DocProcessHandler);
#define REGISTER_DOCPROCESSHANDLER(name) REGISTER_CLASS(DocProcessHandler, name)

} // namespace preprocess

#endif // ROBOT_INTERFACE_DOC_PROCESS_HANDLER_H
