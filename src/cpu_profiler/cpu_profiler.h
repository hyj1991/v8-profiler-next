#ifndef NODE_CPU_PROFILER_
#define NODE_CPU_PROFILER_

#include "nan.h"
#include "node.h"
#include "v8-profiler.h"

namespace nodex {
class CpuProfiler {
 public:
  static void Initialize(v8::Local<v8::Object> target);

  CpuProfiler();
  virtual ~CpuProfiler();

 protected:
  static NAN_METHOD(StartProfiling);
  static NAN_METHOD(StopProfiling);
  static NAN_METHOD(SetSamplingInterval);
  static NAN_METHOD(SetGenerateType);
};

class InnerCpuProfiler {
 public:
  static InnerCpuProfiler* Create(v8::Isolate* isolate);
  InnerCpuProfiler(v8::Isolate* isolate) : isolate_(isolate) {}
  int& started_profiles_count() { return started_profiles_count_; }
  int& generate_type() { return generate_type_; }
  uint32_t& sampling_interval() { return sampling_interval_; }
  v8::CpuProfiler*& profiler() { return profiler_; }
  v8::Isolate* isolate() { return isolate_; }

  // methods
  void SetGenerateType(int type);
  void SetSamplingInterval(uint32_t interval);
  void StartProfiling(v8::Local<v8::String> title, bool recsamples = false);
  v8::CpuProfile* StopProfiling(v8::Local<v8::String> title);

 private:
#if (NODE_MODULE_VERSION > 0x0039)
  int started_profiles_count_ = 0;
  v8::CpuProfiler* profiler_ = nullptr;
  v8::Isolate* isolate_;
  uint32_t sampling_interval_ = 0;
  int generate_type_ = 0;
#endif
};
}  // namespace nodex

#endif  // NODE_CPU_PROFILER_H
