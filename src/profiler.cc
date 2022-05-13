#include "cpu_profiler/cpu_profiler.h"
#include "environment_data.h"
#include "heap_profiler/sampling_heap_profiler.h"
#include "heapsnapshot/heap_profiler.h"
#include "nan.h"
#include "node.h"
#include "v8-inner.h"

namespace nodex {
void InitializeProfiler(v8::Local<v8::Object> target) {
  v8::Isolate* isolate = target->GetIsolate();
  HandleScope scope(isolate);

  // create enviroment data
  EnvironmentData::Create(isolate);

  // register profiler
  HeapProfiler::Initialize(target);
  CpuProfiler::Initialize(target);
  SamplingHeapProfile::Initialize(target);
}

NODE_MODULE(profiler, InitializeProfiler)
}  // namespace nodex
