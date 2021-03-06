/**
 * Autogenerated by Thrift Compiler (1.0.0-dev)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef NLUServing_H
#define NLUServing_H

#include <thrift/TDispatchProcessor.h>
#include "Projects/ChinaMobile/Proto/ChinaMobile_types.h"

namespace chinaMobile {

class NLUServingIf {
 public:
  virtual ~NLUServingIf() {}
  virtual void NLUProcess( ::chinaMobile::ChinaMobileBundle& _return, const  ::chinaMobile::ChinaMobileBundle& bundle) = 0;
};

class NLUServingIfFactory {
 public:
  typedef NLUServingIf Handler;

  virtual ~NLUServingIfFactory() {}

  virtual NLUServingIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(NLUServingIf* /* handler */) = 0;
};

class NLUServingIfSingletonFactory : virtual public NLUServingIfFactory {
 public:
  NLUServingIfSingletonFactory(const boost::shared_ptr<NLUServingIf>& iface) : iface_(iface) {}
  virtual ~NLUServingIfSingletonFactory() {}

  virtual NLUServingIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(NLUServingIf* /* handler */) {}

 protected:
  boost::shared_ptr<NLUServingIf> iface_;
};

class NLUServingNull : virtual public NLUServingIf {
 public:
  virtual ~NLUServingNull() {}
  void NLUProcess( ::chinaMobile::ChinaMobileBundle& /* _return */, const  ::chinaMobile::ChinaMobileBundle& /* bundle */) {
    return;
  }
};

typedef struct _NLUServing_NLUProcess_args__isset {
  _NLUServing_NLUProcess_args__isset() : bundle(false) {}
  bool bundle;
} _NLUServing_NLUProcess_args__isset;

class NLUServing_NLUProcess_args {
 public:

  static const char* ascii_fingerprint; // = "1ECD2695596901A04BE0F391F2074AD3";
  static const uint8_t binary_fingerprint[16]; // = {0x1E,0xCD,0x26,0x95,0x59,0x69,0x01,0xA0,0x4B,0xE0,0xF3,0x91,0xF2,0x07,0x4A,0xD3};

  NLUServing_NLUProcess_args() {
  }

  virtual ~NLUServing_NLUProcess_args() throw() {}

   ::chinaMobile::ChinaMobileBundle bundle;

  _NLUServing_NLUProcess_args__isset __isset;

  void __set_bundle(const  ::chinaMobile::ChinaMobileBundle& val) {
    bundle = val;
  }

  bool operator == (const NLUServing_NLUProcess_args & rhs) const
  {
    if (!(bundle == rhs.bundle))
      return false;
    return true;
  }
  bool operator != (const NLUServing_NLUProcess_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const NLUServing_NLUProcess_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class NLUServing_NLUProcess_pargs {
 public:

  static const char* ascii_fingerprint; // = "1ECD2695596901A04BE0F391F2074AD3";
  static const uint8_t binary_fingerprint[16]; // = {0x1E,0xCD,0x26,0x95,0x59,0x69,0x01,0xA0,0x4B,0xE0,0xF3,0x91,0xF2,0x07,0x4A,0xD3};


  virtual ~NLUServing_NLUProcess_pargs() throw() {}

  const  ::chinaMobile::ChinaMobileBundle* bundle;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _NLUServing_NLUProcess_result__isset {
  _NLUServing_NLUProcess_result__isset() : success(false) {}
  bool success;
} _NLUServing_NLUProcess_result__isset;

class NLUServing_NLUProcess_result {
 public:

  static const char* ascii_fingerprint; // = "0E5E3EFA53952BB870C07E7C5C9C7934";
  static const uint8_t binary_fingerprint[16]; // = {0x0E,0x5E,0x3E,0xFA,0x53,0x95,0x2B,0xB8,0x70,0xC0,0x7E,0x7C,0x5C,0x9C,0x79,0x34};

  NLUServing_NLUProcess_result() {
  }

  virtual ~NLUServing_NLUProcess_result() throw() {}

   ::chinaMobile::ChinaMobileBundle success;

  _NLUServing_NLUProcess_result__isset __isset;

  void __set_success(const  ::chinaMobile::ChinaMobileBundle& val) {
    success = val;
  }

  bool operator == (const NLUServing_NLUProcess_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const NLUServing_NLUProcess_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const NLUServing_NLUProcess_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _NLUServing_NLUProcess_presult__isset {
  _NLUServing_NLUProcess_presult__isset() : success(false) {}
  bool success;
} _NLUServing_NLUProcess_presult__isset;

class NLUServing_NLUProcess_presult {
 public:

  static const char* ascii_fingerprint; // = "0E5E3EFA53952BB870C07E7C5C9C7934";
  static const uint8_t binary_fingerprint[16]; // = {0x0E,0x5E,0x3E,0xFA,0x53,0x95,0x2B,0xB8,0x70,0xC0,0x7E,0x7C,0x5C,0x9C,0x79,0x34};


  virtual ~NLUServing_NLUProcess_presult() throw() {}

   ::chinaMobile::ChinaMobileBundle* success;

  _NLUServing_NLUProcess_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class NLUServingClient : virtual public NLUServingIf {
 public:
  NLUServingClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  NLUServingClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
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
  void NLUProcess( ::chinaMobile::ChinaMobileBundle& _return, const  ::chinaMobile::ChinaMobileBundle& bundle);
  void send_NLUProcess(const  ::chinaMobile::ChinaMobileBundle& bundle);
  void recv_NLUProcess( ::chinaMobile::ChinaMobileBundle& _return);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class NLUServingProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<NLUServingIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (NLUServingProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_NLUProcess(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  NLUServingProcessor(boost::shared_ptr<NLUServingIf> iface) :
    iface_(iface) {
    processMap_["NLUProcess"] = &NLUServingProcessor::process_NLUProcess;
  }

  virtual ~NLUServingProcessor() {}
};

class NLUServingProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  NLUServingProcessorFactory(const ::boost::shared_ptr< NLUServingIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< NLUServingIfFactory > handlerFactory_;
};

class NLUServingMultiface : virtual public NLUServingIf {
 public:
  NLUServingMultiface(std::vector<boost::shared_ptr<NLUServingIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~NLUServingMultiface() {}
 protected:
  std::vector<boost::shared_ptr<NLUServingIf> > ifaces_;
  NLUServingMultiface() {}
  void add(boost::shared_ptr<NLUServingIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void NLUProcess( ::chinaMobile::ChinaMobileBundle& _return, const  ::chinaMobile::ChinaMobileBundle& bundle) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->NLUProcess(_return, bundle);
    }
    ifaces_[i]->NLUProcess(_return, bundle);
    return;
  }

};

} // namespace

#endif
