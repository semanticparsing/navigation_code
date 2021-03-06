/**
 * Autogenerated by Thrift Compiler (1.0.0-dev)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef ChinaMobileCRSServing_H
#define ChinaMobileCRSServing_H

#include <thrift/TDispatchProcessor.h>
#include "Projects/ChinaMobile/Proto/ChinaMobile_types.h"

namespace chinaMobile {

class ChinaMobileCRSServingIf {
 public:
  virtual ~ChinaMobileCRSServingIf() {}
  virtual void ChinaMobileCRSProcess( ::chinaMobile::ChinaMobileBundle& _return, const  ::chinaMobile::ChinaMobileBundle& bundle) = 0;
};

class ChinaMobileCRSServingIfFactory {
 public:
  typedef ChinaMobileCRSServingIf Handler;

  virtual ~ChinaMobileCRSServingIfFactory() {}

  virtual ChinaMobileCRSServingIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(ChinaMobileCRSServingIf* /* handler */) = 0;
};

class ChinaMobileCRSServingIfSingletonFactory : virtual public ChinaMobileCRSServingIfFactory {
 public:
  ChinaMobileCRSServingIfSingletonFactory(const boost::shared_ptr<ChinaMobileCRSServingIf>& iface) : iface_(iface) {}
  virtual ~ChinaMobileCRSServingIfSingletonFactory() {}

  virtual ChinaMobileCRSServingIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(ChinaMobileCRSServingIf* /* handler */) {}

 protected:
  boost::shared_ptr<ChinaMobileCRSServingIf> iface_;
};

class ChinaMobileCRSServingNull : virtual public ChinaMobileCRSServingIf {
 public:
  virtual ~ChinaMobileCRSServingNull() {}
  void ChinaMobileCRSProcess( ::chinaMobile::ChinaMobileBundle& /* _return */, const  ::chinaMobile::ChinaMobileBundle& /* bundle */) {
    return;
  }
};

typedef struct _ChinaMobileCRSServing_ChinaMobileCRSProcess_args__isset {
  _ChinaMobileCRSServing_ChinaMobileCRSProcess_args__isset() : bundle(false) {}
  bool bundle;
} _ChinaMobileCRSServing_ChinaMobileCRSProcess_args__isset;

class ChinaMobileCRSServing_ChinaMobileCRSProcess_args {
 public:

  static const char* ascii_fingerprint; // = "1ECD2695596901A04BE0F391F2074AD3";
  static const uint8_t binary_fingerprint[16]; // = {0x1E,0xCD,0x26,0x95,0x59,0x69,0x01,0xA0,0x4B,0xE0,0xF3,0x91,0xF2,0x07,0x4A,0xD3};

  ChinaMobileCRSServing_ChinaMobileCRSProcess_args() {
  }

  virtual ~ChinaMobileCRSServing_ChinaMobileCRSProcess_args() throw() {}

   ::chinaMobile::ChinaMobileBundle bundle;

  _ChinaMobileCRSServing_ChinaMobileCRSProcess_args__isset __isset;

  void __set_bundle(const  ::chinaMobile::ChinaMobileBundle& val) {
    bundle = val;
  }

  bool operator == (const ChinaMobileCRSServing_ChinaMobileCRSProcess_args & rhs) const
  {
    if (!(bundle == rhs.bundle))
      return false;
    return true;
  }
  bool operator != (const ChinaMobileCRSServing_ChinaMobileCRSProcess_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ChinaMobileCRSServing_ChinaMobileCRSProcess_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class ChinaMobileCRSServing_ChinaMobileCRSProcess_pargs {
 public:

  static const char* ascii_fingerprint; // = "1ECD2695596901A04BE0F391F2074AD3";
  static const uint8_t binary_fingerprint[16]; // = {0x1E,0xCD,0x26,0x95,0x59,0x69,0x01,0xA0,0x4B,0xE0,0xF3,0x91,0xF2,0x07,0x4A,0xD3};


  virtual ~ChinaMobileCRSServing_ChinaMobileCRSProcess_pargs() throw() {}

  const  ::chinaMobile::ChinaMobileBundle* bundle;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _ChinaMobileCRSServing_ChinaMobileCRSProcess_result__isset {
  _ChinaMobileCRSServing_ChinaMobileCRSProcess_result__isset() : success(false) {}
  bool success;
} _ChinaMobileCRSServing_ChinaMobileCRSProcess_result__isset;

class ChinaMobileCRSServing_ChinaMobileCRSProcess_result {
 public:

  static const char* ascii_fingerprint; // = "0E5E3EFA53952BB870C07E7C5C9C7934";
  static const uint8_t binary_fingerprint[16]; // = {0x0E,0x5E,0x3E,0xFA,0x53,0x95,0x2B,0xB8,0x70,0xC0,0x7E,0x7C,0x5C,0x9C,0x79,0x34};

  ChinaMobileCRSServing_ChinaMobileCRSProcess_result() {
  }

  virtual ~ChinaMobileCRSServing_ChinaMobileCRSProcess_result() throw() {}

   ::chinaMobile::ChinaMobileBundle success;

  _ChinaMobileCRSServing_ChinaMobileCRSProcess_result__isset __isset;

  void __set_success(const  ::chinaMobile::ChinaMobileBundle& val) {
    success = val;
  }

  bool operator == (const ChinaMobileCRSServing_ChinaMobileCRSProcess_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const ChinaMobileCRSServing_ChinaMobileCRSProcess_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ChinaMobileCRSServing_ChinaMobileCRSProcess_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _ChinaMobileCRSServing_ChinaMobileCRSProcess_presult__isset {
  _ChinaMobileCRSServing_ChinaMobileCRSProcess_presult__isset() : success(false) {}
  bool success;
} _ChinaMobileCRSServing_ChinaMobileCRSProcess_presult__isset;

class ChinaMobileCRSServing_ChinaMobileCRSProcess_presult {
 public:

  static const char* ascii_fingerprint; // = "0E5E3EFA53952BB870C07E7C5C9C7934";
  static const uint8_t binary_fingerprint[16]; // = {0x0E,0x5E,0x3E,0xFA,0x53,0x95,0x2B,0xB8,0x70,0xC0,0x7E,0x7C,0x5C,0x9C,0x79,0x34};


  virtual ~ChinaMobileCRSServing_ChinaMobileCRSProcess_presult() throw() {}

   ::chinaMobile::ChinaMobileBundle* success;

  _ChinaMobileCRSServing_ChinaMobileCRSProcess_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class ChinaMobileCRSServingClient : virtual public ChinaMobileCRSServingIf {
 public:
  ChinaMobileCRSServingClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  ChinaMobileCRSServingClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void ChinaMobileCRSProcess( ::chinaMobile::ChinaMobileBundle& _return, const  ::chinaMobile::ChinaMobileBundle& bundle);
  void send_ChinaMobileCRSProcess(const  ::chinaMobile::ChinaMobileBundle& bundle);
  void recv_ChinaMobileCRSProcess( ::chinaMobile::ChinaMobileBundle& _return);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class ChinaMobileCRSServingProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<ChinaMobileCRSServingIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (ChinaMobileCRSServingProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_ChinaMobileCRSProcess(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  ChinaMobileCRSServingProcessor(boost::shared_ptr<ChinaMobileCRSServingIf> iface) :
    iface_(iface) {
    processMap_["ChinaMobileCRSProcess"] = &ChinaMobileCRSServingProcessor::process_ChinaMobileCRSProcess;
  }

  virtual ~ChinaMobileCRSServingProcessor() {}
};

class ChinaMobileCRSServingProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  ChinaMobileCRSServingProcessorFactory(const ::boost::shared_ptr< ChinaMobileCRSServingIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< ChinaMobileCRSServingIfFactory > handlerFactory_;
};

class ChinaMobileCRSServingMultiface : virtual public ChinaMobileCRSServingIf {
 public:
  ChinaMobileCRSServingMultiface(std::vector<boost::shared_ptr<ChinaMobileCRSServingIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~ChinaMobileCRSServingMultiface() {}
 protected:
  std::vector<boost::shared_ptr<ChinaMobileCRSServingIf> > ifaces_;
  ChinaMobileCRSServingMultiface() {}
  void add(boost::shared_ptr<ChinaMobileCRSServingIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void ChinaMobileCRSProcess( ::chinaMobile::ChinaMobileBundle& _return, const  ::chinaMobile::ChinaMobileBundle& bundle) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->ChinaMobileCRSProcess(_return, bundle);
    }
    ifaces_[i]->ChinaMobileCRSProcess(_return, bundle);
    return;
  }

};

} // namespace

#endif
