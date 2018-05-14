#include "node.h"
#include "nan.h"
#include "heap_profiler.h"
#include "cpu_profiler.h"
#include "sampling_heap_profile.h"

namespace nodex {
void InitializeProfiler(v8::Local<v8::Object> target) {
  Nan::HandleScope scope;
  HeapProfiler::Initialize(target);
  CpuProfiler::Initialize(target);
  SamplingHeapProfile::Initialize(target);
}

NODE_MODULE(profiler, InitializeProfiler)
}
