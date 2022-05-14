#ifndef NODE_GRAPH_EDGE_
#define NODE_GRAPH_EDGE_

#include "nan.h"
#include "v8-profiler.h"

namespace nodex {

class GraphEdge {
 public:
  static v8::Local<v8::Value> New(v8::Isolate* isolate,
                                  const v8::HeapGraphEdge* node);
};
}  // namespace nodex
#endif  // NODE_GRAPH_EDGE_
