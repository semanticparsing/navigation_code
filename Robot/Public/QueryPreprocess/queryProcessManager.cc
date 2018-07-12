#include "queryProcessManager.h"

#include "Common/Base/stringUtil.h"
namespace preprocess {
void DocProcessHandlerManager::InitByNames(const std::string& handler_list) {
  std::vector<std::string> handler_names;
  SplitString(handler_list, ',', &handler_names);
  for (size_t i = 0; i < handler_names.size(); ++i) {
    DocProcessHandler *handler =
      preprocess::DocProcessHandlerRegisterer::GetInstanceByName(handler_names[i]);
    handler->Init();
    AdddHandlers(handler);
    CHECK(handler) << "init handler failed!";
    VLOG(1) << "Init handler:" << handler_names[i];
  }
}

bool DocProcessHandlerManager::ProcessDoc(QueryInfo &query_info) {
  for (size_t i = 0; i < handlers_.size(); ++i) {
    handlers_[i]->ProcessDoc(query_info);
  }
  return true;
}

}
