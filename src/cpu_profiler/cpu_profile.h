#ifndef NODE_PROFILE_
#define NODE_PROFILE_

#include "isolate_aware.h"
#include "nan.h"
#include "v8-profiler.h"

namespace nodex {

#define CPU_PROFILE_METHODS(V) V(Delete);

#define NAN_CPU_PROFILE_METHOD(func) \
  CALL_NAN_METHOD(cpu_profile, Profile, InnerProfile, func)

class Profile {
 public:
  static v8::Local<v8::Object> New(v8::Isolate* isolate,
                                   const v8::CpuProfile* node, uint32_t type);

 private:
  static void Initialize(v8::Isolate* isolate);
  CPU_PROFILE_METHODS(static NAN_METHOD);
};

class InnerProfile : IsolateAware {
 public:
  InnerProfile(v8::Isolate* isolate) : IsolateAware(isolate){};
  CPU_PROFILE_METHODS(INNER_METHOD);
  uint32_t& uid_counter() { return uid_counter_; }

 private:
  uint32_t uid_counter_ = -1;
};

}  // namespace nodex
#endif  // NODE_PROFILE_