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
}  // namespace nodex

#endif  // NODE_CPU_PROFILER_H
