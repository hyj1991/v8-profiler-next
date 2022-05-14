#include "cpu_profiler/cpu_profiler.h"
#include "heap_profiler/sampling_heap_profiler.h"
#include "heapsnapshot/heap_graph_node.h"
#include "heapsnapshot/heap_profiler.h"
#include "heapsnapshot/heap_snapshot.h"
#include "nan.h"
#include "v8.h"

namespace nodex {
class EnvironmentData {
 public:
  static void Create(v8::Isolate* isolate);
  static EnvironmentData* GetCurrent(v8::Isolate* isolate);
  static EnvironmentData* GetCurrent(
      const Nan::FunctionCallbackInfo<v8::Value>& info);
  static EnvironmentData* GetCurrent(
      const Nan::PropertyCallbackInfo<v8::Value>& info);

  EnvironmentData(v8::Isolate* isolate);
  v8::Isolate* isolate() { return isolate_; };
  InnerCpuProfiler*& cpu_profiler() { return cpu_profiler_; }
  InnerSamplingHeapProfiler*& heap_profiler() { return heap_profiler_; }
  InnerHeapProfiler*& heapsnapshot() { return heapsnapshot_; }
  InnerSnapshot*& snapshot() { return snapshot_; }
  InnerGraphNode*& graph_node() { return graph_node_; }

 private:
  v8::Isolate* isolate_ = nullptr;
  InnerCpuProfiler* cpu_profiler_ = nullptr;
  InnerSamplingHeapProfiler* heap_profiler_ = nullptr;
  InnerHeapProfiler* heapsnapshot_ = nullptr;
  InnerSnapshot* snapshot_ = nullptr;
  InnerGraphNode* graph_node_ = nullptr;
};
}  // namespace nodex