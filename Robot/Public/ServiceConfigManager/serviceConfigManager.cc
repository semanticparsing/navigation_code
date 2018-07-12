// Copyright (c) 2017 Trio.ai. All rights reserved.
#include <map>
#include <string>
#include "Common/Base/stringUtil.h"
#include "Common/ThirdParty/boost/boost/shared_ptr.hpp"
#include "Robot/Public/ServiceConfigManager/serviceConfigManager.h"
#include "Robot/Public/Utils/trioLog.h"

DECLARE_string(default_bot_name);
DEFINE_string(service_config_file,
    "", "protobuf RobotServiceConfig file");
DEFINE_string(default_params_file,
    "", "ServiceParams default value reassign file");

namespace robot {
  ServiceConfigManager::ServiceConfigManager() {
    Init();
  }
  void ServiceConfigManager::Init() {
    service_config_loader_.reset(new ProtoBufferLoader<RobotServiceConfig>());
    CHECK(service_config_loader_ != NULL);
    CHECK(service_config_loader_->Init(
          FLAGS_service_config_file,
          common::NewPermanentCallback(
            this, &ServiceConfigManager::ReloadRobotServiceConfig)));

    if (!FLAGS_default_params_file.empty()) {
      default_params_loader_.reset(new ProtoBufferLoader<ServiceParams>());
      CHECK(default_params_loader_ != NULL);
      CHECK(default_params_loader_->Init(
            FLAGS_default_params_file,
            common::NewPermanentCallback(
              this, &ServiceConfigManager::ReloadDefaultParams)));
    }
  }

  boost::shared_ptr<ServiceParams>
    ServiceConfigManager::GetServiceParams(
        const std::string& bot_name) const {
    boost::shared_ptr<ServiceParams> service_params(
        new ServiceParams(default_params_));
    const string& real_name =
      bot_name.empty() ? FLAGS_default_bot_name : bot_name;
    common::ReaderMutexLock lock(&service_info_mutex_);
    map<string, boost::shared_ptr<RobotServiceInfo> >::const_iterator
      it = service_info_dict_.find(real_name);
    if (it != service_info_dict_.end() &&
        it->second->has_params()) {
      service_params->CopyFrom(it->second->params());
    }
    return service_params;
  }

  void ServiceConfigManager::ReloadRobotServiceConfig(
      const RobotServiceConfig& config_obj) {
    map<string, boost::shared_ptr<RobotServiceInfo> > reloading_dict;
    for (size_t i = 0; i < config_obj.services_size(); ++i) {
      boost::shared_ptr<RobotServiceInfo> service_info(
          new RobotServiceInfo(config_obj.services(i)));
      for (size_t k = 0; k < service_info->nick_name_size(); ++k) {
        reloading_dict[StringToLowerASCII(service_info->nick_name(k))] = service_info;
      }
      reloading_dict[StringToLowerASCII(service_info->bot_name())] = service_info;
      VLOG(TRIO_LOG_STAT)
        << "botname:" << StringToLowerASCII(service_info->bot_name())
        <<  " param:" << service_info->Utf8DebugString();
    }
    common::WriterMutexLock lock(&service_info_mutex_);
    service_info_dict_.swap(reloading_dict);
    VLOG(TRIO_LOG_STAT) << "reload size:" << service_info_dict_.size();
  }

  void ServiceConfigManager::ReloadDefaultParams(
      const ServiceParams& config_obj) {
    default_params_.CopyFrom(config_obj);
  }

  void ServiceConfigManager::GetRealBotName(
      const std::string& raw_name,
      std::string* real_name) const {
    const string& bot_name =
      raw_name.empty() ? FLAGS_default_bot_name : StringToLowerASCII(raw_name);
    common::ReaderMutexLock lock(&service_info_mutex_);
    map<string, boost::shared_ptr<RobotServiceInfo> >::const_iterator
      it = service_info_dict_.find(bot_name);
    if (it != service_info_dict_.end()) {
      *real_name = it->second->bot_name();
    } else {
      *real_name = bot_name;
    }
  }
}  // namespace robot
