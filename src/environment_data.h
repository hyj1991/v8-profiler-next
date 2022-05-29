#ifndef ENVIRONMENT_DATA_H
#define ENVIRONMENT_DATA_H

#include "cpu_profiler/cpu_profile.h"
#include "cpu_profiler/cpu_profiler.h"
#include "heap_profiler/sampling_heap_profiler.h"
#include "heapsnapshot/heap_graph_node.h"
#include "heapsnapshot/heap_profiler.h"
#include "heapsnapshot/heap_snapshot.h"
#include "nan.h"
#include "v8.h"

namespace nodex {
#define DELETE_INNER(private_name) \
  delete private_name;             \
  private_name = nullptr;

#define GET_JS_ENVIRONMENT_DATA(data, variable, convert_to)                \
  v8::Local<v8::convert_to> variable =                                     \
      Nan::To<v8::convert_to>(                                             \
          Nan::Get(data, Nan::New<v8::String>(#variable).ToLocalChecked()) \
              .ToLocalChecked())                                           \
          .ToLocalChecked()

using ThreadId = int;

class EnvironmentData {
 public:
  static void Create(v8::Isolate* isolate);
  static EnvironmentData* GetCurrent(v8::Isolate* isolate);
  static EnvironmentData* GetCurrent(
      const Nan::FunctionCallbackInfo<v8::Value>& info);
  static EnvironmentData* GetCurrent(
      const Nan::PropertyCallbackInfo<v8::Value>& info);
  static void JsSetupEnvironmentData(
      const Nan::FunctionCallbackInfo<v8::Value>& info);

  EnvironmentData(v8::Isolate* isolate);
  ~EnvironmentData() {
    DELETE_INNER(cpu_profiler_);
    DELETE_INNER(cpu_profile_);
    DELETE_INNER(heap_profiler_);
    DELETE_INNER(heapsnapshot_);
    DELETE_INNER(snapshot_);
    DELETE_INNER(graph_node_);
  }
  v8::Isolate* isolate() { return isolate_; };
  ThreadId& thread_id() { return thread_id_; }
  bool& debug() { return debug_; }
  bool& greater_than_12_15_0() { return greater_than_12_15_0_; }
  InnerCpuProfiler*& cpu_profiler() { return cpu_profiler_; }
  InnerProfile*& cpu_profile() { return cpu_profile_; }
  InnerSamplingHeapProfiler*& heap_profiler() { return heap_profiler_; }
  InnerHeapProfiler*& heapsnapshot() { return heapsnapshot_; }
  InnerSnapshot*& snapshot() { return snapshot_; }
  InnerGraphNode*& graph_node() { return graph_node_; }

 private:
  static void AtExit(void* arg);
  v8::Isolate* isolate_ = nullptr;
  ThreadId thread_id_ = -1;
  bool debug_ = false;
  bool greater_than_12_15_0_ = true;

  // Inner classes
  InnerCpuProfiler* cpu_profiler_ = nullptr;
  InnerProfile* cpu_profile_ = nullptr;
  InnerSamplingHeapProfiler* heap_profiler_ = nullptr;
  InnerHeapProfiler* heapsnapshot_ = nullptr;
  InnerSnapshot* snapshot_ = nullptr;
  InnerGraphNode* graph_node_ = nullptr;
};
}  // namespace nodex

#endif