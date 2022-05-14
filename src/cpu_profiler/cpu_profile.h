#ifndef NODE_PROFILE_
#define NODE_PROFILE_

#include "nan.h"
#include "v8-profiler.h"

namespace nodex {

class Profile {
 public:
  Profile(v8::Isolate* isolate) : isolate_(isolate) {}
  v8::Local<v8::Object> New(const v8::CpuProfile* node, uint32_t type);
  v8::Isolate* isolate() { return isolate_; }

 private:
  static NAN_METHOD(Delete);
  void Initialize();
  Nan::Persistent<v8::ObjectTemplate> profile_template_;
  uint32_t uid_counter = -1;
  v8::Isolate* isolate_;
};

}  // namespace nodex
#endif  // NODE_PROFILE_
