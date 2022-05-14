#include "heap_graph_node.h"

#include "environment_data.h"
#include "heap_graph_edge.h"
#include "v8-inner.h"

namespace nodex {
using v8::Array;
using v8::FunctionTemplate;
using v8::HeapGraphNode;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::ObjectTemplate;
using v8::String;
using v8::Value;

namespace per_thread {
thread_local Nan::Persistent<Object> graph_node_cache;
thread_local Nan::Persistent<v8::ObjectTemplate> graph_node_template;
}  // namespace per_thread

NAN_METHOD(GraphNode_EmptyMethod) {}

void GraphNode::Initialize(v8::Isolate* isolate) {
  HandleScope scope(isolate);

  Local<FunctionTemplate> f = Nan::New<FunctionTemplate>(GraphNode_EmptyMethod);
  Local<ObjectTemplate> o = f->InstanceTemplate();
  Local<Object> _cache = Nan::New<Object>();
  o->SetInternalFieldCount(1);
#if (NODE_MODULE_VERSION <= 0x000B)
  Nan::SetMethod(o, "getHeapValue", GraphNode::GetHeapValue);
#endif
  Nan::SetAccessor(o, Nan::New<String>("children").ToLocalChecked(),
                   GraphNode::GetChildren);
  per_thread::graph_node_template.Reset(o);
  per_thread::graph_node_cache.Reset(_cache);
}

#if (NODE_MODULE_VERSION <= 0x000B)
GRAPH_NODE_METHODS(NAN_GRAPH_NODE_METHOD);

INNER_METHOD(InnerGraphNode::GetHeapValue) {
  HandleScope scope(this->isolate());

  void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
  HeapGraphNode* node = static_cast<HeapGraphNode*>(ptr);
  info.GetReturnValue().Set(Nan::New(node->GetHeapValue()));
}
#endif

GRAPH_NODE_GETTER(NAN_GRAPH_NODE_GETTER);

INNER_GETTER(InnerGraphNode::GetChildren) {
  HandleScope scope(this->isolate());

  void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
  HeapGraphNode* node = static_cast<HeapGraphNode*>(ptr);
  uint32_t count = node->GetChildrenCount();
  Local<Array> children = Nan::New<Array>(count);
  for (uint32_t index = 0; index < count; ++index) {
    Local<Value> child = GraphEdge::New(this->isolate(), node->GetChild(index));
    Nan::Set(children, index, child);
  }
  Nan::Set(info.This(), Nan::New<String>("children").ToLocalChecked(),
           children);
  info.GetReturnValue().Set(children);
}

Local<Value> GraphNode::New(v8::Isolate* isolate, const HeapGraphNode* node) {
  EscapableHandleScope scope(isolate);

  if (per_thread::graph_node_template.IsEmpty()) {
    GraphNode::Initialize(isolate);
  }

  Local<Object> graph_node;
  Local<Object> _cache = Nan::New(per_thread::graph_node_cache);
  int32_t _id = node->GetId();
  if (Nan::Has(_cache, _id).ToChecked()) {
    graph_node = Nan::To<Object>(Nan::Get(_cache, _id).ToLocalChecked())
                     .ToLocalChecked();
  } else {
#if (NODE_MODULE_VERSION > 0x0040)
    graph_node = Nan::New(per_thread::graph_node_template)
                     ->NewInstance(Nan::GetCurrentContext())
                     .ToLocalChecked();
#else
    graph_node = Nan::New(per_thread::graph_node_template)->NewInstance();
#endif
    Nan::SetInternalFieldPointer(graph_node, 0,
                                 const_cast<HeapGraphNode*>(node));

    Local<Value> type;
    switch (node->GetType()) {
      case HeapGraphNode::kArray:
        type = Nan::New<String>("Array").ToLocalChecked();
        break;
      case HeapGraphNode::kString:
        type = Nan::New<String>("String").ToLocalChecked();
        break;
      case HeapGraphNode::kObject:
        type = Nan::New<String>("Object").ToLocalChecked();
        break;
      case HeapGraphNode::kCode:
        type = Nan::New<String>("Code").ToLocalChecked();
        break;
      case HeapGraphNode::kClosure:
        type = Nan::New<String>("Closure").ToLocalChecked();
        break;
      case HeapGraphNode::kRegExp:
        type = Nan::New<String>("RegExp").ToLocalChecked();
        break;
      case HeapGraphNode::kHeapNumber:
        type = Nan::New<String>("HeapNumber").ToLocalChecked();
        break;
      case HeapGraphNode::kNative:
        type = Nan::New<String>("Native").ToLocalChecked();
        break;
      case HeapGraphNode::kSynthetic:
        type = Nan::New<String>("Synthetic").ToLocalChecked();
        break;
#if (NODE_MODULE_VERSION > 0x000B)
      case HeapGraphNode::kConsString:
        type = Nan::New<String>("ConsString").ToLocalChecked();
        break;
      case HeapGraphNode::kSlicedString:
        type = Nan::New<String>("SlicedString").ToLocalChecked();
        break;
#endif
      default:
        type = Nan::New<String>("Hidden").ToLocalChecked();
    }
#if (NODE_MODULE_VERSION >= 45)
    Local<String> name = node->GetName();
#else
    Local<String> name = Nan::New<String>(node->GetName());
#endif
    Local<Value> id = Nan::New<Number>(_id);
    Nan::Set(graph_node, Nan::New<String>("type").ToLocalChecked(), type);
    Nan::Set(graph_node, Nan::New<String>("name").ToLocalChecked(), name);
    Nan::Set(graph_node, Nan::New<String>("id").ToLocalChecked(), id);

#if (NODE_MODULE_VERSION > 0x000B)
    Local<Value> shallowSize = Nan::New<Number>(node->GetShallowSize());
    Nan::Set(graph_node, Nan::New<String>("shallowSize").ToLocalChecked(),
             shallowSize);
#endif
    Nan::Set(_cache, _id, graph_node);
  }

  return scope.Escape(graph_node);
}
}  // namespace nodex
