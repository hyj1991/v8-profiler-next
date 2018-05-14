#ifndef NODE_PROFILE_
#define NODE_PROFILE_

#include "v8-profiler.h"
#include "nan.h"

namespace nodex {

class SamplingHeapProfile {
public:
  static void Initialize(v8::Local<v8::Object> target);
  SamplingHeapProfile();
  virtual ~SamplingHeapProfile();
protected:
  static NAN_METHOD(StartSamplingHeapProfiling);
  static NAN_METHOD(StopSamplingHeapProfiling);
};

} //namespace nodex
#endif  // NODE_PROFILE_
