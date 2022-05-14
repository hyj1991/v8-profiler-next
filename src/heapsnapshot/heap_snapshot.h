#ifndef NODE_SNAPSHOT_
#define NODE_SNAPSHOT_

#include "isolate_aware.h"
#include "nan.h"
#include "v8-profiler.h"

namespace nodex {

#define SNAPSHOT_METHODS(V) \
  V(GetNode);               \
  V(GetNodeById);           \
  V(Delete);                \
  V(Serialize);

#define SNAPSHOT_GETTER(V) V(GetRoot);

#define NAN_HEAP_SNAPSHOT_METHOD(func) \
  CALL_NAN_METHOD(snapshot, Snapshot, InnerSnapshot, func)

#define NAN_HEAP_SNAPSHOT_GETTER(func) \
  CALL_NAN_GETTER(snapshot, Snapshot, InnerSnapshot, func)

class Snapshot {
 public:
  static v8::Local<v8::Value> New(v8::Isolate* isolate,
                                  const v8::HeapSnapshot* node);

 private:
  static void Initialize(v8::Isolate* isolate);
  SNAPSHOT_GETTER(static NAN_GETTER);
  SNAPSHOT_METHODS(static NAN_METHOD);
};

class InnerSnapshot : IsolateAware {
 public:
  InnerSnapshot(v8::Isolate* isolate) : IsolateAware(isolate){};
  SNAPSHOT_GETTER(INNER_GETTER);
  SNAPSHOT_METHODS(INNER_METHOD);
};
}  // namespace nodex
#endif  // NODE_SNAPSHOT_
