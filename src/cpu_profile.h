#ifndef NODE_PROFILE_
#define NODE_PROFILE_

#include "v8-profiler.h"
#include "nan.h"

namespace nodex {

class Profile {
public:
  static v8::Local<v8::Object> New(const v8::CpuProfile* node, uint32_t type);
  static Nan::Persistent<v8::Object> profiles;
private:
  static NAN_METHOD(Delete);
  static void Initialize();
  static Nan::Persistent<v8::ObjectTemplate> profile_template_;
  static uint32_t uid_counter;
};

} //namespace nodex
#endif  // NODE_PROFILE_
