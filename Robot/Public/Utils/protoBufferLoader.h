// Copyright (c) 2017 Trio.ai. All rights reserved.

#ifndef ROBOT_PUBLIC_UTILS_PROTOBUFFERLOADER_H_
#define ROBOT_PUBLIC_UTILS_PROTOBUFFERLOADER_H_

#include <string>
#include "Common/Base/trio.h"
#include "Common/ThirdParty/file_monitor/include/fileMonitor.h"
#include "Common/Util/Thread/mutex.h"
#include "Common/Util/ProtoBuf/protoUtil.h"
#include "Common/Util/Thread/callbackSpec.h"
#include "Robot/Public/Utils/trioLog.h"

namespace robot {
template <class PB>
class ProtoBufferLoader {
 public:
  ProtoBufferLoader() {}
  ~ProtoBufferLoader() {}
  bool Init(
      const std::string& path,
      common::Callback1<const PB&>* callback) {
    if (path.empty() || callback == NULL) {
      return false;
    }
    path_ = path;
    callback_.reset(callback);
    CHECK(monitor_.Init());
    monitor_.AddFile(path_,
        common::NewPermanentCallback(this,
          &ProtoBufferLoader::ReloadProtoBuffer));
    ReloadProtoBuffer();
    return true;
  }

 private:
  std::string path_;
  file_monitor::TrioFileMonitor monitor_;
  scoped_ptr<common::Callback1<const PB&> > callback_;

 private:
  void ReloadProtoBuffer() {
    VLOG(TRIO_LOG_CUSTOM) << "proto buffer path:" << path_;
    if (!path_.empty() && callback_ != NULL) {
      std::string pb_string;
      if (file::File::ReadFileToString(path_, &pb_string)) {
        PB pb_obj;
        if (common::proto_util::ParseFromASCII(pb_string, &pb_obj)) {
          callback_->Run(pb_obj);
          VLOG(2) << "load proto buffer ok";
        } else {
          LOG(ERROR) << "Parse proto buffer error";
        }
      }
    }
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(ProtoBufferLoader);
};
}  // namespace robot

#endif  // ROBOT_PUBLIC_UTILS_PROTOBUFFERLOADER_H_
