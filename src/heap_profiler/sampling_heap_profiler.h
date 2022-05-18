#ifndef NODE_SAMPLING_HEAP_PROFILE_
#define NODE_SAMPLING_HEAP_PROFILE_

#include "isolate_aware.h"
#include "nan.h"
#include "v8-profiler.h"

namespace nodex {

#define SAMPLING_HEAP_PROFILER_METHODS(V) \
  V(StartSamplingHeapProfiling);          \
  V(StopSamplingHeapProfiling);

#define NAN_SAMPLING_HEAP_PROFILER_METHOD(func)        \
  CALL_NAN_METHOD(heap_profiler, SamplingHeapProfiler, \
                  InnerSamplingHeapProfiler, func)

class SamplingHeapProfiler {
 public:
  static void Initialize(v8::Local<v8::Object> target);
  SamplingHeapProfiler();
  virtual ~SamplingHeapProfiler();

 protected:
  SAMPLING_HEAP_PROFILER_METHODS(static NAN_METHOD);
};

class InnerSamplingHeapProfiler : IsolateAware {
 public:
  InnerSamplingHeapProfiler(v8::Isolate* isolate) : IsolateAware(isolate){};
  SAMPLING_HEAP_PROFILER_METHODS(INNER_METHOD);
};

}  // namespace nodex
#endif  // NODE_SAMPLING_HEAP_PROFILE_