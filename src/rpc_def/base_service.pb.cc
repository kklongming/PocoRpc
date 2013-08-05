// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: base_service.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "base_service.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace PocoRpc {

namespace {

const ::google::protobuf::Descriptor* PingReq_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  PingReq_reflection_ = NULL;
const ::google::protobuf::Descriptor* PingReply_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  PingReply_reflection_ = NULL;
const ::google::protobuf::Descriptor* GetServiceListReq_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  GetServiceListReq_reflection_ = NULL;
const ::google::protobuf::Descriptor* GetServiceListReply_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  GetServiceListReply_reflection_ = NULL;
const ::google::protobuf::ServiceDescriptor* BaseService_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_base_5fservice_2eproto() {
  protobuf_AddDesc_base_5fservice_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "base_service.proto");
  GOOGLE_CHECK(file != NULL);
  PingReq_descriptor_ = file->message_type(0);
  static const int PingReq_offsets_[1] = {
  };
  PingReq_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      PingReq_descriptor_,
      PingReq::default_instance_,
      PingReq_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PingReq, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PingReq, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(PingReq));
  PingReply_descriptor_ = file->message_type(1);
  static const int PingReply_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PingReply, status_),
  };
  PingReply_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      PingReply_descriptor_,
      PingReply::default_instance_,
      PingReply_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PingReply, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PingReply, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(PingReply));
  GetServiceListReq_descriptor_ = file->message_type(2);
  static const int GetServiceListReq_offsets_[1] = {
  };
  GetServiceListReq_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      GetServiceListReq_descriptor_,
      GetServiceListReq::default_instance_,
      GetServiceListReq_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GetServiceListReq, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GetServiceListReq, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(GetServiceListReq));
  GetServiceListReply_descriptor_ = file->message_type(3);
  static const int GetServiceListReply_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GetServiceListReply, status_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GetServiceListReply, service_list_),
  };
  GetServiceListReply_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      GetServiceListReply_descriptor_,
      GetServiceListReply::default_instance_,
      GetServiceListReply_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GetServiceListReply, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(GetServiceListReply, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(GetServiceListReply));
  BaseService_descriptor_ = file->service(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_base_5fservice_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    PingReq_descriptor_, &PingReq::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    PingReply_descriptor_, &PingReply::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    GetServiceListReq_descriptor_, &GetServiceListReq::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    GetServiceListReply_descriptor_, &GetServiceListReply::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_base_5fservice_2eproto() {
  delete PingReq::default_instance_;
  delete PingReq_reflection_;
  delete PingReply::default_instance_;
  delete PingReply_reflection_;
  delete GetServiceListReq::default_instance_;
  delete GetServiceListReq_reflection_;
  delete GetServiceListReply::default_instance_;
  delete GetServiceListReply_reflection_;
}

void protobuf_AddDesc_base_5fservice_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\022base_service.proto\022\007PocoRpc\"\t\n\007PingReq"
    "\"\033\n\tPingReply\022\016\n\006status\030\001 \002(\r\"\023\n\021GetServ"
    "iceListReq\";\n\023GetServiceListReply\022\016\n\006sta"
    "tus\030\001 \002(\r\022\024\n\014service_list\030\002 \003(\t2\207\001\n\013Base"
    "Service\022,\n\004Ping\022\020.PocoRpc.PingReq\032\022.Poco"
    "Rpc.PingReply\022J\n\016GetServiceList\022\032.PocoRp"
    "c.GetServiceListReq\032\034.PocoRpc.GetService"
    "ListReplyB\003\200\001\001", 294);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "base_service.proto", &protobuf_RegisterTypes);
  PingReq::default_instance_ = new PingReq();
  PingReply::default_instance_ = new PingReply();
  GetServiceListReq::default_instance_ = new GetServiceListReq();
  GetServiceListReply::default_instance_ = new GetServiceListReply();
  PingReq::default_instance_->InitAsDefaultInstance();
  PingReply::default_instance_->InitAsDefaultInstance();
  GetServiceListReq::default_instance_->InitAsDefaultInstance();
  GetServiceListReply::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_base_5fservice_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_base_5fservice_2eproto {
  StaticDescriptorInitializer_base_5fservice_2eproto() {
    protobuf_AddDesc_base_5fservice_2eproto();
  }
} static_descriptor_initializer_base_5fservice_2eproto_;

// ===================================================================

#ifndef _MSC_VER
#endif  // !_MSC_VER

PingReq::PingReq()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void PingReq::InitAsDefaultInstance() {
}

PingReq::PingReq(const PingReq& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void PingReq::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

PingReq::~PingReq() {
  SharedDtor();
}

void PingReq::SharedDtor() {
  if (this != default_instance_) {
  }
}

void PingReq::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* PingReq::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return PingReq_descriptor_;
}

const PingReq& PingReq::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_base_5fservice_2eproto();
  return *default_instance_;
}

PingReq* PingReq::default_instance_ = NULL;

PingReq* PingReq::New() const {
  return new PingReq;
}

void PingReq::Clear() {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool PingReq::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
        ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
      return true;
    }
    DO_(::google::protobuf::internal::WireFormat::SkipField(
          input, tag, mutable_unknown_fields()));
  }
  return true;
#undef DO_
}

void PingReq::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* PingReq::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int PingReq::ByteSize() const {
  int total_size = 0;

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void PingReq::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const PingReq* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const PingReq*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void PingReq::MergeFrom(const PingReq& from) {
  GOOGLE_CHECK_NE(&from, this);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void PingReq::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void PingReq::CopyFrom(const PingReq& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool PingReq::IsInitialized() const {

  return true;
}

void PingReq::Swap(PingReq* other) {
  if (other != this) {
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata PingReq::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = PingReq_descriptor_;
  metadata.reflection = PingReq_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int PingReply::kStatusFieldNumber;
#endif  // !_MSC_VER

PingReply::PingReply()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void PingReply::InitAsDefaultInstance() {
}

PingReply::PingReply(const PingReply& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void PingReply::SharedCtor() {
  _cached_size_ = 0;
  status_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

PingReply::~PingReply() {
  SharedDtor();
}

void PingReply::SharedDtor() {
  if (this != default_instance_) {
  }
}

void PingReply::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* PingReply::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return PingReply_descriptor_;
}

const PingReply& PingReply::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_base_5fservice_2eproto();
  return *default_instance_;
}

PingReply* PingReply::default_instance_ = NULL;

PingReply* PingReply::New() const {
  return new PingReply;
}

void PingReply::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    status_ = 0u;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool PingReply::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required uint32 status = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &status_)));
          set_has_status();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void PingReply::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required uint32 status = 1;
  if (has_status()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->status(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* PingReply::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required uint32 status = 1;
  if (has_status()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->status(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int PingReply::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required uint32 status = 1;
    if (has_status()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->status());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void PingReply::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const PingReply* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const PingReply*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void PingReply::MergeFrom(const PingReply& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_status()) {
      set_status(from.status());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void PingReply::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void PingReply::CopyFrom(const PingReply& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool PingReply::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void PingReply::Swap(PingReply* other) {
  if (other != this) {
    std::swap(status_, other->status_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata PingReply::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = PingReply_descriptor_;
  metadata.reflection = PingReply_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
#endif  // !_MSC_VER

GetServiceListReq::GetServiceListReq()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void GetServiceListReq::InitAsDefaultInstance() {
}

GetServiceListReq::GetServiceListReq(const GetServiceListReq& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void GetServiceListReq::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

GetServiceListReq::~GetServiceListReq() {
  SharedDtor();
}

void GetServiceListReq::SharedDtor() {
  if (this != default_instance_) {
  }
}

void GetServiceListReq::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* GetServiceListReq::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return GetServiceListReq_descriptor_;
}

const GetServiceListReq& GetServiceListReq::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_base_5fservice_2eproto();
  return *default_instance_;
}

GetServiceListReq* GetServiceListReq::default_instance_ = NULL;

GetServiceListReq* GetServiceListReq::New() const {
  return new GetServiceListReq;
}

void GetServiceListReq::Clear() {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool GetServiceListReq::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
        ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
      return true;
    }
    DO_(::google::protobuf::internal::WireFormat::SkipField(
          input, tag, mutable_unknown_fields()));
  }
  return true;
#undef DO_
}

void GetServiceListReq::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* GetServiceListReq::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int GetServiceListReq::ByteSize() const {
  int total_size = 0;

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void GetServiceListReq::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const GetServiceListReq* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const GetServiceListReq*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void GetServiceListReq::MergeFrom(const GetServiceListReq& from) {
  GOOGLE_CHECK_NE(&from, this);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void GetServiceListReq::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void GetServiceListReq::CopyFrom(const GetServiceListReq& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GetServiceListReq::IsInitialized() const {

  return true;
}

void GetServiceListReq::Swap(GetServiceListReq* other) {
  if (other != this) {
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata GetServiceListReq::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = GetServiceListReq_descriptor_;
  metadata.reflection = GetServiceListReq_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int GetServiceListReply::kStatusFieldNumber;
const int GetServiceListReply::kServiceListFieldNumber;
#endif  // !_MSC_VER

GetServiceListReply::GetServiceListReply()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void GetServiceListReply::InitAsDefaultInstance() {
}

GetServiceListReply::GetServiceListReply(const GetServiceListReply& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void GetServiceListReply::SharedCtor() {
  _cached_size_ = 0;
  status_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

GetServiceListReply::~GetServiceListReply() {
  SharedDtor();
}

void GetServiceListReply::SharedDtor() {
  if (this != default_instance_) {
  }
}

void GetServiceListReply::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* GetServiceListReply::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return GetServiceListReply_descriptor_;
}

const GetServiceListReply& GetServiceListReply::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_base_5fservice_2eproto();
  return *default_instance_;
}

GetServiceListReply* GetServiceListReply::default_instance_ = NULL;

GetServiceListReply* GetServiceListReply::New() const {
  return new GetServiceListReply;
}

void GetServiceListReply::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    status_ = 0u;
  }
  service_list_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool GetServiceListReply::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required uint32 status = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &status_)));
          set_has_status();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_service_list;
        break;
      }

      // repeated string service_list = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_service_list:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_service_list()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->service_list(this->service_list_size() - 1).data(),
            this->service_list(this->service_list_size() - 1).length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_service_list;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void GetServiceListReply::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required uint32 status = 1;
  if (has_status()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->status(), output);
  }

  // repeated string service_list = 2;
  for (int i = 0; i < this->service_list_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->service_list(i).data(), this->service_list(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->service_list(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* GetServiceListReply::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required uint32 status = 1;
  if (has_status()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->status(), target);
  }

  // repeated string service_list = 2;
  for (int i = 0; i < this->service_list_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->service_list(i).data(), this->service_list(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(2, this->service_list(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int GetServiceListReply::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required uint32 status = 1;
    if (has_status()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->status());
    }

  }
  // repeated string service_list = 2;
  total_size += 1 * this->service_list_size();
  for (int i = 0; i < this->service_list_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->service_list(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void GetServiceListReply::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const GetServiceListReply* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const GetServiceListReply*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void GetServiceListReply::MergeFrom(const GetServiceListReply& from) {
  GOOGLE_CHECK_NE(&from, this);
  service_list_.MergeFrom(from.service_list_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_status()) {
      set_status(from.status());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void GetServiceListReply::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void GetServiceListReply::CopyFrom(const GetServiceListReply& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GetServiceListReply::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void GetServiceListReply::Swap(GetServiceListReply* other) {
  if (other != this) {
    std::swap(status_, other->status_);
    service_list_.Swap(&other->service_list_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata GetServiceListReply::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = GetServiceListReply_descriptor_;
  metadata.reflection = GetServiceListReply_reflection_;
  return metadata;
}


// ===================================================================

BaseService::~BaseService() {}

const ::google::protobuf::ServiceDescriptor* BaseService::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return BaseService_descriptor_;
}

const ::google::protobuf::ServiceDescriptor* BaseService::GetDescriptor() {
  protobuf_AssignDescriptorsOnce();
  return BaseService_descriptor_;
}

void BaseService::Ping(::google::protobuf::RpcController* controller,
                         const ::PocoRpc::PingReq*,
                         ::PocoRpc::PingReply*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method Ping() not implemented.");
  done->Run();
}

void BaseService::GetServiceList(::google::protobuf::RpcController* controller,
                         const ::PocoRpc::GetServiceListReq*,
                         ::PocoRpc::GetServiceListReply*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method GetServiceList() not implemented.");
  done->Run();
}

void BaseService::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                             ::google::protobuf::RpcController* controller,
                             const ::google::protobuf::Message* request,
                             ::google::protobuf::Message* response,
                             ::google::protobuf::Closure* done) {
  GOOGLE_DCHECK_EQ(method->service(), BaseService_descriptor_);
  switch(method->index()) {
    case 0:
      Ping(controller,
             ::google::protobuf::down_cast<const ::PocoRpc::PingReq*>(request),
             ::google::protobuf::down_cast< ::PocoRpc::PingReply*>(response),
             done);
      break;
    case 1:
      GetServiceList(controller,
             ::google::protobuf::down_cast<const ::PocoRpc::GetServiceListReq*>(request),
             ::google::protobuf::down_cast< ::PocoRpc::GetServiceListReply*>(response),
             done);
      break;
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      break;
  }
}

const ::google::protobuf::Message& BaseService::GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::PocoRpc::PingReq::default_instance();
    case 1:
      return ::PocoRpc::GetServiceListReq::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *reinterpret_cast< ::google::protobuf::Message*>(NULL);
  }
}

const ::google::protobuf::Message& BaseService::GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::PocoRpc::PingReply::default_instance();
    case 1:
      return ::PocoRpc::GetServiceListReply::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *reinterpret_cast< ::google::protobuf::Message*>(NULL);
  }
}

BaseService_Stub::BaseService_Stub(::google::protobuf::RpcChannel* channel)
  : channel_(channel), owns_channel_(false) {}
BaseService_Stub::BaseService_Stub(
    ::google::protobuf::RpcChannel* channel,
    ::google::protobuf::Service::ChannelOwnership ownership)
  : channel_(channel),
    owns_channel_(ownership == ::google::protobuf::Service::STUB_OWNS_CHANNEL) {}
BaseService_Stub::~BaseService_Stub() {
  if (owns_channel_) delete channel_;
}

void BaseService_Stub::Ping(::google::protobuf::RpcController* controller,
                              const ::PocoRpc::PingReq* request,
                              ::PocoRpc::PingReply* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
}
void BaseService_Stub::GetServiceList(::google::protobuf::RpcController* controller,
                              const ::PocoRpc::GetServiceListReq* request,
                              ::PocoRpc::GetServiceListReply* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(1),
                       controller, request, response, done);
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace PocoRpc

// @@protoc_insertion_point(global_scope)
