#include "cpu_profiler/cpu_profiler.h"
#include "environment_data.h"
#include "heap_profiler/sampling_heap_profiler.h"
#include "heapsnapshot/heap_profiler.h"
#include "nan.h"
#include "node.h"
#include "v8-inner.h"

namespace nodex {
#define CREATE_JS_BINDING(js_func, native_func)                          \
  Nan::Set(target, Nan::New<v8::String>(#js_func).ToLocalChecked(),      \
           Nan::GetFunction(Nan::New<v8::FunctionTemplate>(native_func)) \
               .ToLocalChecked())

void InitializeProfiler(v8::Local<v8::Object> target) {
  v8::Isolate* isolate = target->GetIsolate();
  HandleScope scope(isolate);

  // create enviroment data
  EnvironmentData::Create(isolate);

  CREATE_JS_BINDING(setup, EnvironmentData::JsSetupEnvironmentData);

  // register profiler
  HeapProfiler::Initialize(target);
  CpuProfiler::Initialize(target);
  SamplingHeapProfiler::Initialize(target);
}

NODE_MODULE_CONTEXT_AWARE(profiler, InitializeProfiler)
}  // namespace nodex
