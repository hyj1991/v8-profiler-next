#ifndef NODE_SAMPLING_HEAP_PROFILE_
#define NODE_SAMPLING_HEAP_PROFILE_

#include "nan.h"
#include "v8-profiler.h"

namespace nodex {

class SamplingHeapProfiler {
 public:
  static void Initialize(v8::Local<v8::Object> target);
  SamplingHeapProfiler();
  virtual ~SamplingHeapProfiler();

 protected:
  static NAN_METHOD(StartSamplingHeapProfiling);
  static NAN_METHOD(StopSamplingHeapProfiling);
};

class InnerSamplingHeapProfiler {
 public:
  static InnerSamplingHeapProfiler* Create(v8::Isolate* isolate);
  InnerSamplingHeapProfiler(v8::Isolate* isolate) : isolate_(isolate) {}
  v8::Isolate* isolate() { return isolate_; }

  void StartSamplingHeapProfiling(uint64_t sample_interval = -1,
                                  int stack_depth = -1);
  void CheckProfile(v8::AllocationProfile*);
  v8::AllocationProfile* GetAllocationProfile();
  v8::Local<v8::Object> TranslateAllocationProfile(
      v8::AllocationProfile::Node* node);

 private:
  v8::Isolate* isolate_;
};

}  // namespace nodex
#endif  // NODE_SAMPLING_HEAP_PROFILE_
