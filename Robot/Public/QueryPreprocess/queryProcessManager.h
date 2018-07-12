#ifndef QUERY_PROCESS_HANDLER_REMOVE_STOP_WORD_HANDLER_MANAGER_H
#define QUERY_PROCESS_HANDLER_REMOVE_STOP_WORD_HANDLER_MANAGER_H

#include "removeStopWordHandler.h"
#include "docProcessHandler.h"

#include <vector>

namespace preprocess {
class DocProcessHandlerManager : public DocProcessHandler {
 public:
  DocProcessHandlerManager() {}
  virtual ~DocProcessHandlerManager() {
    for (size_t i =0; i < handlers_.size(); ++i) {
      if (handlers_[i] != NULL) {
        delete handlers_[i];
        handlers_[i] = NULL;
      }
    }
  }

  void InitByNames(const std::string& handler_list);
  void AdddHandlers(DocProcessHandler* handler) {
    handlers_.push_back(handler);
  }

  bool virtual ProcessDoc(QueryInfo &query_info);

protected:
  std::vector<DocProcessHandler*> handlers_;
};

} // namespace preprocess

#endif // QUERY_PROCESS_HANDLER_REMOVE_STOP_WORD_HANDLER_MANAGER_H
