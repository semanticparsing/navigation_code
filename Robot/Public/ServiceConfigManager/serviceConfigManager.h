// Copyright (c) 2017 Trio.ai. All rights reserved.
#include <map>
#include <string>
#include "Common/Base/trio.h"
#include "Common/ThirdParty/boost/boost/shared_ptr.hpp"
#include "Common/Util/Registerer/singleton.h"
#include "Robot/Public/Proto/serviceConfig.pb.h"
#include "Robot/Public/Proto/serviceParams.pb.h"
#include "Robot/Public/Utils/protoBufferLoader.h"

#ifndef ROBOT_PUBLIC_SERVICECONFIGMANAGER_SERVICECONFIGMANAGER_H_
#define ROBOT_PUBLIC_SERVICECONFIGMANAGER_SERVICECONFIGMANAGER_H_
namespace robot {
class ServiceConfigManager {
 public:
  ServiceConfigManager();
  virtual ~ServiceConfigManager() {}
  boost::shared_ptr<ServiceParams> GetServiceParams(
      const std::string& bot_name) const;
  void GetRealBotName(
      const std::string& raw_name,
      std::string* real_name) const;

 private:
  void Init();
  void ReloadRobotServiceConfig(const RobotServiceConfig& config_obj);
  void ReloadDefaultParams(const ServiceParams& config_obj);

 private:
  std::map<string, boost::shared_ptr<RobotServiceInfo> >
    service_info_dict_;
  scoped_ptr<ProtoBufferLoader<RobotServiceConfig> > service_config_loader_;
  scoped_ptr<ProtoBufferLoader<ServiceParams> > default_params_loader_;
  ServiceParams default_params_;
  mutable common::RwMutex service_info_mutex_;
};
typedef Singleton<ServiceConfigManager> SingletonServiceConfigManager;
}  // namespace robot
#endif  // ROBOT_PUBLIC_SERVICECONFIGMANAGER_SERVICECONFIGMANAGER_H_
