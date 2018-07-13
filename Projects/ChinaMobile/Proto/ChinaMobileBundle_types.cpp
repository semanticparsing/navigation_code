/**
 * Autogenerated by Thrift Compiler (1.0.0-dev)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "Projects/ChinaMobile/Proto/ChinaMobileBundle_types.h"

#include <algorithm>

namespace chinaMobile {

const char* InParams::ascii_fingerprint = "87E4CDFDA65073C99F07C481ACD6AAB7";
const uint8_t InParams::binary_fingerprint[16] = {0x87,0xE4,0xCD,0xFD,0xA6,0x50,0x73,0xC9,0x9F,0x07,0xC4,0x81,0xAC,0xD6,0xAA,0xB7};

uint32_t InParams::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->call_id);
          this->__isset.call_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->inter_idx);
          this->__isset.inter_idx = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->call_sor_id);
          this->__isset.call_sor_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->call_dst_id);
          this->__isset.call_dst_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->flow);
          this->__isset.flow = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->auth_token);
          this->__isset.auth_token = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 7:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->entrance_id);
          this->__isset.entrance_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 8:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->start_time);
          this->__isset.start_time = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 9:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->ses_time);
          this->__isset.ses_time = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 10:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->suilu_region_id);
          this->__isset.suilu_region_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 11:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->cc_id);
          this->__isset.cc_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 12:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->role_label);
          this->__isset.role_label = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 13:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->begin_play);
          this->__isset.begin_play = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 14:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->end_play);
          this->__isset.end_play = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 15:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->result_time);
          this->__isset.result_time = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 16:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->flow_result_type);
          this->__isset.flow_result_type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 17:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->input);
          this->__isset.input = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 24:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->inter_no);
          this->__isset.inter_no = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 18:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->org);
          this->__isset.org = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 19:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->newsess);
          this->__isset.newsess = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 20:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->trans_idx);
          this->__isset.trans_idx = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 21:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->begin_trans);
          this->__isset.begin_trans = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 22:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->end_trans);
          this->__isset.end_trans = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 23:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->trans_result);
          this->__isset.trans_result = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t InParams::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("InParams");

  if (this->__isset.call_id) {
    xfer += oprot->writeFieldBegin("call_id", ::apache::thrift::protocol::T_STRING, 1);
    xfer += oprot->writeString(this->call_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.inter_idx) {
    xfer += oprot->writeFieldBegin("inter_idx", ::apache::thrift::protocol::T_STRING, 2);
    xfer += oprot->writeString(this->inter_idx);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.call_sor_id) {
    xfer += oprot->writeFieldBegin("call_sor_id", ::apache::thrift::protocol::T_STRING, 3);
    xfer += oprot->writeString(this->call_sor_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.call_dst_id) {
    xfer += oprot->writeFieldBegin("call_dst_id", ::apache::thrift::protocol::T_STRING, 4);
    xfer += oprot->writeString(this->call_dst_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.flow) {
    xfer += oprot->writeFieldBegin("flow", ::apache::thrift::protocol::T_STRING, 5);
    xfer += oprot->writeString(this->flow);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.auth_token) {
    xfer += oprot->writeFieldBegin("auth_token", ::apache::thrift::protocol::T_STRING, 6);
    xfer += oprot->writeString(this->auth_token);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.entrance_id) {
    xfer += oprot->writeFieldBegin("entrance_id", ::apache::thrift::protocol::T_STRING, 7);
    xfer += oprot->writeString(this->entrance_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.start_time) {
    xfer += oprot->writeFieldBegin("start_time", ::apache::thrift::protocol::T_STRING, 8);
    xfer += oprot->writeString(this->start_time);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.ses_time) {
    xfer += oprot->writeFieldBegin("ses_time", ::apache::thrift::protocol::T_STRING, 9);
    xfer += oprot->writeString(this->ses_time);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.suilu_region_id) {
    xfer += oprot->writeFieldBegin("suilu_region_id", ::apache::thrift::protocol::T_STRING, 10);
    xfer += oprot->writeString(this->suilu_region_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.cc_id) {
    xfer += oprot->writeFieldBegin("cc_id", ::apache::thrift::protocol::T_STRING, 11);
    xfer += oprot->writeString(this->cc_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.role_label) {
    xfer += oprot->writeFieldBegin("role_label", ::apache::thrift::protocol::T_STRING, 12);
    xfer += oprot->writeString(this->role_label);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.begin_play) {
    xfer += oprot->writeFieldBegin("begin_play", ::apache::thrift::protocol::T_STRING, 13);
    xfer += oprot->writeString(this->begin_play);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.end_play) {
    xfer += oprot->writeFieldBegin("end_play", ::apache::thrift::protocol::T_STRING, 14);
    xfer += oprot->writeString(this->end_play);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.result_time) {
    xfer += oprot->writeFieldBegin("result_time", ::apache::thrift::protocol::T_STRING, 15);
    xfer += oprot->writeString(this->result_time);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.flow_result_type) {
    xfer += oprot->writeFieldBegin("flow_result_type", ::apache::thrift::protocol::T_STRING, 16);
    xfer += oprot->writeString(this->flow_result_type);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.input) {
    xfer += oprot->writeFieldBegin("input", ::apache::thrift::protocol::T_STRING, 17);
    xfer += oprot->writeString(this->input);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.org) {
    xfer += oprot->writeFieldBegin("org", ::apache::thrift::protocol::T_STRING, 18);
    xfer += oprot->writeString(this->org);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.newsess) {
    xfer += oprot->writeFieldBegin("newsess", ::apache::thrift::protocol::T_STRING, 19);
    xfer += oprot->writeString(this->newsess);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.trans_idx) {
    xfer += oprot->writeFieldBegin("trans_idx", ::apache::thrift::protocol::T_STRING, 20);
    xfer += oprot->writeString(this->trans_idx);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.begin_trans) {
    xfer += oprot->writeFieldBegin("begin_trans", ::apache::thrift::protocol::T_STRING, 21);
    xfer += oprot->writeString(this->begin_trans);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.end_trans) {
    xfer += oprot->writeFieldBegin("end_trans", ::apache::thrift::protocol::T_STRING, 22);
    xfer += oprot->writeString(this->end_trans);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.trans_result) {
    xfer += oprot->writeFieldBegin("trans_result", ::apache::thrift::protocol::T_STRING, 23);
    xfer += oprot->writeString(this->trans_result);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.inter_no) {
    xfer += oprot->writeFieldBegin("inter_no", ::apache::thrift::protocol::T_STRING, 24);
    xfer += oprot->writeString(this->inter_no);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(InParams &a, InParams &b) {
  using ::std::swap;
  swap(a.call_id, b.call_id);
  swap(a.inter_idx, b.inter_idx);
  swap(a.call_sor_id, b.call_sor_id);
  swap(a.call_dst_id, b.call_dst_id);
  swap(a.flow, b.flow);
  swap(a.auth_token, b.auth_token);
  swap(a.entrance_id, b.entrance_id);
  swap(a.start_time, b.start_time);
  swap(a.ses_time, b.ses_time);
  swap(a.suilu_region_id, b.suilu_region_id);
  swap(a.cc_id, b.cc_id);
  swap(a.role_label, b.role_label);
  swap(a.begin_play, b.begin_play);
  swap(a.end_play, b.end_play);
  swap(a.result_time, b.result_time);
  swap(a.flow_result_type, b.flow_result_type);
  swap(a.input, b.input);
  swap(a.inter_no, b.inter_no);
  swap(a.org, b.org);
  swap(a.newsess, b.newsess);
  swap(a.trans_idx, b.trans_idx);
  swap(a.begin_trans, b.begin_trans);
  swap(a.end_trans, b.end_trans);
  swap(a.trans_result, b.trans_result);
  swap(a.__isset, b.__isset);
}

const char* OutParams::ascii_fingerprint = "976BD24F768BEBA7FA4B65B743DDF088";
const uint8_t OutParams::binary_fingerprint[16] = {0x97,0x6B,0xD2,0x4F,0x76,0x8B,0xEB,0xA7,0xFA,0x4B,0x65,0xB7,0x43,0xDD,0xF0,0x88};

uint32_t OutParams::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->call_id);
          this->__isset.call_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->inter_idx);
          this->__isset.inter_idx = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->flow_type);
          this->__isset.flow_type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->flow_node_name);
          this->__isset.flow_node_name = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->model_type);
          this->__isset.model_type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->prompt_type);
          this->__isset.prompt_type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 7:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->prompt_wav);
          this->__isset.prompt_wav = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 8:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->prompt_text);
          this->__isset.prompt_text = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 9:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->timeout);
          this->__isset.timeout = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 10:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->grammar);
          this->__isset.grammar = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 11:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->org);
          this->__isset.org = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 12:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->newsess);
          this->__isset.newsess = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 13:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->delay);
          this->__isset.delay = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 14:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->act_params);
          this->__isset.act_params = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 15:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->call_sor_id);
          this->__isset.call_sor_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 16:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->trans_idx);
          this->__isset.trans_idx = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 17:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->trans_type);
          this->__isset.trans_type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 18:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->route_value);
          this->__isset.route_value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 19:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->call_dst_id);
          this->__isset.call_dst_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 20:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->start_time);
          this->__isset.start_time = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 21:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->end_time);
          this->__isset.end_time = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 22:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->region_id);
          this->__isset.region_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 23:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->entrance_id);
          this->__isset.entrance_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 24:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->exit_id);
          this->__isset.exit_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 25:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->user_type_id);
          this->__isset.user_type_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 26:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->suilu_region_id);
          this->__isset.suilu_region_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t OutParams::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("OutParams");

  if (this->__isset.call_id) {
    xfer += oprot->writeFieldBegin("call_id", ::apache::thrift::protocol::T_STRING, 1);
    xfer += oprot->writeString(this->call_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.inter_idx) {
    xfer += oprot->writeFieldBegin("inter_idx", ::apache::thrift::protocol::T_STRING, 2);
    xfer += oprot->writeString(this->inter_idx);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.flow_type) {
    xfer += oprot->writeFieldBegin("flow_type", ::apache::thrift::protocol::T_STRING, 3);
    xfer += oprot->writeString(this->flow_type);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.flow_node_name) {
    xfer += oprot->writeFieldBegin("flow_node_name", ::apache::thrift::protocol::T_STRING, 4);
    xfer += oprot->writeString(this->flow_node_name);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.model_type) {
    xfer += oprot->writeFieldBegin("model_type", ::apache::thrift::protocol::T_STRING, 5);
    xfer += oprot->writeString(this->model_type);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.prompt_type) {
    xfer += oprot->writeFieldBegin("prompt_type", ::apache::thrift::protocol::T_STRING, 6);
    xfer += oprot->writeString(this->prompt_type);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.prompt_wav) {
    xfer += oprot->writeFieldBegin("prompt_wav", ::apache::thrift::protocol::T_STRING, 7);
    xfer += oprot->writeString(this->prompt_wav);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.prompt_text) {
    xfer += oprot->writeFieldBegin("prompt_text", ::apache::thrift::protocol::T_STRING, 8);
    xfer += oprot->writeString(this->prompt_text);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.timeout) {
    xfer += oprot->writeFieldBegin("timeout", ::apache::thrift::protocol::T_STRING, 9);
    xfer += oprot->writeString(this->timeout);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.grammar) {
    xfer += oprot->writeFieldBegin("grammar", ::apache::thrift::protocol::T_STRING, 10);
    xfer += oprot->writeString(this->grammar);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.org) {
    xfer += oprot->writeFieldBegin("org", ::apache::thrift::protocol::T_STRING, 11);
    xfer += oprot->writeString(this->org);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.newsess) {
    xfer += oprot->writeFieldBegin("newsess", ::apache::thrift::protocol::T_STRING, 12);
    xfer += oprot->writeString(this->newsess);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.delay) {
    xfer += oprot->writeFieldBegin("delay", ::apache::thrift::protocol::T_STRING, 13);
    xfer += oprot->writeString(this->delay);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.act_params) {
    xfer += oprot->writeFieldBegin("act_params", ::apache::thrift::protocol::T_STRING, 14);
    xfer += oprot->writeString(this->act_params);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.call_sor_id) {
    xfer += oprot->writeFieldBegin("call_sor_id", ::apache::thrift::protocol::T_STRING, 15);
    xfer += oprot->writeString(this->call_sor_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.trans_idx) {
    xfer += oprot->writeFieldBegin("trans_idx", ::apache::thrift::protocol::T_STRING, 16);
    xfer += oprot->writeString(this->trans_idx);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.trans_type) {
    xfer += oprot->writeFieldBegin("trans_type", ::apache::thrift::protocol::T_STRING, 17);
    xfer += oprot->writeString(this->trans_type);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.route_value) {
    xfer += oprot->writeFieldBegin("route_value", ::apache::thrift::protocol::T_STRING, 18);
    xfer += oprot->writeString(this->route_value);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.call_dst_id) {
    xfer += oprot->writeFieldBegin("call_dst_id", ::apache::thrift::protocol::T_STRING, 19);
    xfer += oprot->writeString(this->call_dst_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.start_time) {
    xfer += oprot->writeFieldBegin("start_time", ::apache::thrift::protocol::T_STRING, 20);
    xfer += oprot->writeString(this->start_time);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.end_time) {
    xfer += oprot->writeFieldBegin("end_time", ::apache::thrift::protocol::T_STRING, 21);
    xfer += oprot->writeString(this->end_time);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.region_id) {
    xfer += oprot->writeFieldBegin("region_id", ::apache::thrift::protocol::T_STRING, 22);
    xfer += oprot->writeString(this->region_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.entrance_id) {
    xfer += oprot->writeFieldBegin("entrance_id", ::apache::thrift::protocol::T_STRING, 23);
    xfer += oprot->writeString(this->entrance_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.exit_id) {
    xfer += oprot->writeFieldBegin("exit_id", ::apache::thrift::protocol::T_STRING, 24);
    xfer += oprot->writeString(this->exit_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.user_type_id) {
    xfer += oprot->writeFieldBegin("user_type_id", ::apache::thrift::protocol::T_STRING, 25);
    xfer += oprot->writeString(this->user_type_id);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.suilu_region_id) {
    xfer += oprot->writeFieldBegin("suilu_region_id", ::apache::thrift::protocol::T_STRING, 26);
    xfer += oprot->writeString(this->suilu_region_id);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(OutParams &a, OutParams &b) {
  using ::std::swap;
  swap(a.call_id, b.call_id);
  swap(a.inter_idx, b.inter_idx);
  swap(a.flow_type, b.flow_type);
  swap(a.flow_node_name, b.flow_node_name);
  swap(a.model_type, b.model_type);
  swap(a.prompt_type, b.prompt_type);
  swap(a.prompt_wav, b.prompt_wav);
  swap(a.prompt_text, b.prompt_text);
  swap(a.timeout, b.timeout);
  swap(a.grammar, b.grammar);
  swap(a.org, b.org);
  swap(a.newsess, b.newsess);
  swap(a.delay, b.delay);
  swap(a.act_params, b.act_params);
  swap(a.call_sor_id, b.call_sor_id);
  swap(a.trans_idx, b.trans_idx);
  swap(a.trans_type, b.trans_type);
  swap(a.route_value, b.route_value);
  swap(a.call_dst_id, b.call_dst_id);
  swap(a.start_time, b.start_time);
  swap(a.end_time, b.end_time);
  swap(a.region_id, b.region_id);
  swap(a.entrance_id, b.entrance_id);
  swap(a.exit_id, b.exit_id);
  swap(a.user_type_id, b.user_type_id);
  swap(a.suilu_region_id, b.suilu_region_id);
  swap(a.__isset, b.__isset);
}

const char* ChinaMobileBundle::ascii_fingerprint = "BA22A9C42BD1978EC4A6377C4FC08315";
const uint8_t ChinaMobileBundle::binary_fingerprint[16] = {0xBA,0x22,0xA9,0xC4,0x2B,0xD1,0x97,0x8E,0xC4,0xA6,0x37,0x7C,0x4F,0xC0,0x83,0x15};

uint32_t ChinaMobileBundle::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->userid);
          this->__isset.userid = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->inaction);
          this->__isset.inaction = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->inparams.read(iprot);
          this->__isset.inparams = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->ret);
          this->__isset.ret = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->outaction);
          this->__isset.outaction = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->outparams.read(iprot);
          this->__isset.outparams = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 7:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->authResult);
          this->__isset.authResult = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 8:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->NLUResult);
          this->__isset.NLUResult = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 9:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->ASRResult);
          this->__isset.ASRResult = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 10:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->trace_id);
          this->__isset.trace_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t ChinaMobileBundle::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("ChinaMobileBundle");

  if (this->__isset.userid) {
    xfer += oprot->writeFieldBegin("userid", ::apache::thrift::protocol::T_STRING, 1);
    xfer += oprot->writeString(this->userid);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.inaction) {
    xfer += oprot->writeFieldBegin("inaction", ::apache::thrift::protocol::T_I32, 2);
    xfer += oprot->writeI32(this->inaction);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.inparams) {
    xfer += oprot->writeFieldBegin("inparams", ::apache::thrift::protocol::T_STRUCT, 3);
    xfer += this->inparams.write(oprot);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.ret) {
    xfer += oprot->writeFieldBegin("ret", ::apache::thrift::protocol::T_I32, 4);
    xfer += oprot->writeI32(this->ret);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.outaction) {
    xfer += oprot->writeFieldBegin("outaction", ::apache::thrift::protocol::T_I32, 5);
    xfer += oprot->writeI32(this->outaction);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.outparams) {
    xfer += oprot->writeFieldBegin("outparams", ::apache::thrift::protocol::T_STRUCT, 6);
    xfer += this->outparams.write(oprot);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.authResult) {
    xfer += oprot->writeFieldBegin("authResult", ::apache::thrift::protocol::T_STRING, 7);
    xfer += oprot->writeString(this->authResult);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.NLUResult) {
    xfer += oprot->writeFieldBegin("NLUResult", ::apache::thrift::protocol::T_STRING, 8);
    xfer += oprot->writeString(this->NLUResult);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.ASRResult) {
    xfer += oprot->writeFieldBegin("ASRResult", ::apache::thrift::protocol::T_STRING, 9);
    xfer += oprot->writeString(this->ASRResult);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.trace_id) {
    xfer += oprot->writeFieldBegin("trace_id", ::apache::thrift::protocol::T_STRING, 10);
    xfer += oprot->writeString(this->trace_id);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(ChinaMobileBundle &a, ChinaMobileBundle &b) {
  using ::std::swap;
  swap(a.userid, b.userid);
  swap(a.inaction, b.inaction);
  swap(a.inparams, b.inparams);
  swap(a.ret, b.ret);
  swap(a.outaction, b.outaction);
  swap(a.outparams, b.outparams);
  swap(a.authResult, b.authResult);
  swap(a.NLUResult, b.NLUResult);
  swap(a.ASRResult, b.ASRResult);
  swap(a.trace_id, b.trace_id);
  swap(a.__isset, b.__isset);
}

} // namespace
