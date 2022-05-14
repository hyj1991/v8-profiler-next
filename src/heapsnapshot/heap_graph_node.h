#ifndef NODE_GRAPH_NODE_
#define NODE_GRAPH_NODE_

#include "isolate_aware.h"
#include "nan.h"
#include "v8-profiler.h"

namespace nodex {

#define GRAPH_NODE_METHODS(V) V(GetHeapValue);

#define GRAPH_NODE_GETTER(V) V(GetChildren);

#define NAN_GRAPH_NODE_METHOD(func) \
  CALL_NAN_METHOD(graph_node, GraphNode, InnerGraphNode, func)

#define NAN_GRAPH_NODE_GETTER(func) \
  CALL_NAN_GETTER(graph_node, GraphNode, InnerGraphNode, func)

class GraphNode {
 public:
  static v8::Local<v8::Value> New(v8::Isolate* isolate,
                                  const v8::HeapGraphNode* node);

 private:
  static void Initialize(v8::Isolate* isolate);
  GRAPH_NODE_METHODS(static NAN_METHOD);
  GRAPH_NODE_GETTER(static NAN_GETTER);
};

class InnerGraphNode : IsolateAware {
 public:
  InnerGraphNode(v8::Isolate* isolate) : IsolateAware(isolate){};
  GRAPH_NODE_METHODS(INNER_METHOD);
  GRAPH_NODE_GETTER(INNER_GETTER);
};
}  // namespace nodex
#endif  // NODE_GRAPH_NODE_
