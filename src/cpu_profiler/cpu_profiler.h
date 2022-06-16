#ifndef NODE_CPU_PROFILER_
#define NODE_CPU_PROFILER_

#include "isolate_aware.h"
#include "nan.h"
#include "node.h"
#include "v8-profiler.h"

namespace nodex {

#define CPU_PROFILER_METHODS(V) \
  V(StartProfiling);            \
  V(StopProfiling);             \
  V(SetSamplingInterval);       \
  V(CollectSample);             \
  V(SetGenerateType);           \
  V(SetProfilesCleanupLimit);

#define NAN_CPU_PROFILER_METHOD(func) \
  CALL_NAN_METHOD(cpu_profiler, CpuProfiler, InnerCpuProfiler, func)

class CpuProfiler {
 public:
  static void Initialize(v8::Local<v8::Object> target);

  CpuProfiler();
  virtual ~CpuProfiler();

 protected:
  CPU_PROFILER_METHODS(static NAN_METHOD);
};

class InnerCpuProfiler : IsolateAware {
 public:
  InnerCpuProfiler(v8::Isolate* isolate) : IsolateAware(isolate){};
  CPU_PROFILER_METHODS(INNER_METHOD);

 private:
#if (NODE_MODULE_VERSION > 0x0039)
  int started_profiles_count_ = 0;
  int profiles_since_last_cleanup_ = 0;
  int profiles_clean_limit_ = 0;
  v8::CpuProfiler* cpu_profiler_ = nullptr;
  uint32_t sampling_interval_ = 0;
#endif

  int generateType = 0;
};
}  // namespace nodex

#endif  // NODE_CPU_PROFILER_H