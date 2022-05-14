#ifndef NODE_HEAP_PROFILER_
#define NODE_HEAP_PROFILER_

#include "isolate_aware.h"
#include "nan.h"
#include "node.h"
#include "v8-profiler.h"

namespace nodex {
#define HEAP_METHODS(V)        \
  V(TakeSnapshot);             \
  V(StartTrackingHeapObjects); \
  V(StopTrackingHeapObjects);  \
  V(GetHeapStats);             \
  V(GetObjectByHeapObjectId);  \
  V(GetHeapObjectId);

#define NAN_HEAP_PROFILER(func) \
  CALL_NAN_METHOD(heapsnapshot, HeapProfiler, InnerHeapProfiler, func)

class HeapProfiler {
 public:
  static void Initialize(v8::Local<v8::Object> target);

  HeapProfiler();
  virtual ~HeapProfiler();

 protected:
  HEAP_METHODS(static NAN_METHOD);
};

class InnerHeapProfiler : IsolateAware {
 public:
  InnerHeapProfiler(v8::Isolate* isolate) : IsolateAware(isolate){};
  HEAP_METHODS(INNER_METHOD);
};
}  // namespace nodex

#endif  // NODE_HEAP_PROFILER_H
