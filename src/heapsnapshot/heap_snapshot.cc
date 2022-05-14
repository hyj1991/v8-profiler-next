#include "heap_snapshot.h"

#include "environment_data.h"
#include "heap_graph_node.h"
#include "heap_output_stream.h"
#include "v8-inner.h"

namespace nodex {
using v8::Array;
using v8::Function;
using v8::FunctionTemplate;
using v8::HeapGraphEdge;
using v8::HeapGraphNode;
using v8::HeapSnapshot;
using v8::Integer;
using v8::Local;
using v8::Object;
using v8::ObjectTemplate;
using v8::SnapshotObjectId;
using v8::String;
using v8::Value;

namespace per_thread {
thread_local Nan::Persistent<ObjectTemplate> snapshot_template;
thread_local Nan::Persistent<Object> snapshots;
}  // namespace per_thread

NAN_METHOD(Snapshot_EmptyMethod) {}

void Snapshot::Initialize(v8::Isolate* isolate) {
  HandleScope scope(isolate);

  Local<FunctionTemplate> f = Nan::New<FunctionTemplate>(Snapshot_EmptyMethod);
  Local<ObjectTemplate> o = f->InstanceTemplate();
  o->SetInternalFieldCount(1);
  Nan::SetAccessor(o, Nan::New<String>("root").ToLocalChecked(),
                   Snapshot::GetRoot);
  Nan::SetMethod(o, "getNode", Snapshot::GetNode);
  Nan::SetMethod(o, "getNodeById", Snapshot::GetNodeById);
  Nan::SetMethod(o, "delete", Snapshot::Delete);
  Nan::SetMethod(o, "serialize", Snapshot::Serialize);
  per_thread::snapshot_template.Reset(o);
}

SNAPSHOT_GETTER(NAN_HEAP_SNAPSHOT_GETTER);

INNER_GETTER(InnerSnapshot::GetRoot) {
  HandleScope scope(this->isolate());

  Local<Object> _root;
  Local<String> __root = Nan::New<String>("_root").ToLocalChecked();
  if (Nan::Has(info.This(), __root).ToChecked()) {
    Local<Value> root = Nan::GetPrivate(info.This(), __root).ToLocalChecked();
    info.GetReturnValue().Set(root);
  } else {
    void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
    Local<Value> _root = GraphNode::New(
        this->isolate(), static_cast<HeapSnapshot*>(ptr)->GetRoot());
    Nan::SetPrivate(info.This(), __root, _root);
    info.GetReturnValue().Set(_root);
  }
}

SNAPSHOT_METHODS(NAN_HEAP_SNAPSHOT_METHOD);

INNER_METHOD(InnerSnapshot::GetNode) {
  HandleScope scope(this->isolate());

  if (!info.Length()) {
    return Nan::ThrowError("No index specified");
  } else if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Argument must be an integer");
  }

  int32_t index = Nan::To<int32_t>(info[0]).ToChecked();
  void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
  info.GetReturnValue().Set(GraphNode::New(
      this->isolate(), static_cast<HeapSnapshot*>(ptr)->GetNode(index)));
}

INNER_METHOD(InnerSnapshot::GetNodeById) {
  HandleScope scope(this->isolate());

  if (!info.Length()) {
    return Nan::ThrowError("No id specified");
  } else if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Argument must be an integer");
  }

  SnapshotObjectId id = Nan::To<int32_t>(info[0]).ToChecked();
  void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
  info.GetReturnValue().Set(GraphNode::New(
      this->isolate(), static_cast<HeapSnapshot*>(ptr)->GetNodeById(id)));
}

INNER_METHOD(InnerSnapshot::Serialize) {
  HandleScope scope(this->isolate());

  void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
  if (info.Length() < 2) {
    return Nan::ThrowError("Invalid number of arguments");
  } else if (!info[0]->IsFunction() || !info[1]->IsFunction()) {
    return Nan::ThrowTypeError("Arguments must be a functions");
  }

  Local<Function> iterator = Local<Function>::Cast(info[0]);
  Local<Function> callback = Local<Function>::Cast(info[1]);

  OutputStreamAdapter* stream =
      new OutputStreamAdapter(this->isolate(), iterator, callback);
  static_cast<HeapSnapshot*>(ptr)->Serialize(stream, HeapSnapshot::kJSON);
}

INNER_METHOD(InnerSnapshot::Delete) {
  HandleScope scope(this->isolate());

  void* ptr = Nan::GetInternalFieldPointer(info.Holder(), 0);

  Local<Object> snapshots = Nan::New<Object>(per_thread::snapshots);

  Local<String> __uid = Nan::New<String>("uid").ToLocalChecked();
  Local<Integer> _uid =
      Nan::To<Integer>(Nan::Get(info.Holder(), __uid).ToLocalChecked())
          .ToLocalChecked();
  Nan::Delete(snapshots, static_cast<int>(_uid->Value()));

  static_cast<HeapSnapshot*>(ptr)->Delete();
  info.GetReturnValue().Set(snapshots);
}

Local<Value> Snapshot::New(v8::Isolate* isolate, const HeapSnapshot* node) {
  EscapableHandleScope scope(isolate);

  if (per_thread::snapshot_template.IsEmpty()) {
    Snapshot::Initialize(isolate);
  }

  Local<Object> snapshot;
#if (NODE_MODULE_VERSION > 0x0040)
  snapshot = Nan::New(per_thread::snapshot_template)
                 ->NewInstance(Nan::GetCurrentContext())
                 .ToLocalChecked();
#else
  snapshot = Nan::New(per_thread::snapshot_template)->NewInstance();
#endif
  Nan::SetInternalFieldPointer(snapshot, 0, const_cast<HeapSnapshot*>(node));

  Local<Value> HEAP = Nan::New<String>("HEAP").ToLocalChecked();

  // starting with iojs 3 GetUid() and GetTitle() APIs were removed
  uint32_t _uid = node->GetMaxSnapshotJSObjectId();

  char _title[32];
  sprintf(_title, "Snapshot %i", _uid);
  Local<String> title = Nan::New<String>(_title).ToLocalChecked();

  Local<Value> uid = Nan::New<Integer>(_uid);
  Local<Integer> nodesCount = Nan::New<Integer>(node->GetNodesCount());
  Local<Integer> objectId = Nan::New<Integer>(node->GetMaxSnapshotJSObjectId());

  Nan::Set(snapshot, Nan::New<String>("typeId").ToLocalChecked(), HEAP);
  Nan::Set(snapshot, Nan::New<String>("title").ToLocalChecked(), title);
  Nan::Set(snapshot, Nan::New<String>("uid").ToLocalChecked(), uid);
  Nan::Set(snapshot, Nan::New<String>("nodesCount").ToLocalChecked(),
           nodesCount);
  Nan::Set(snapshot, Nan::New<String>("maxSnapshotJSObjectId").ToLocalChecked(),
           objectId);

  Local<Object> snapshots = Nan::New<Object>(per_thread::snapshots);
  Nan::Set(snapshots, _uid, snapshot);

  return scope.Escape(snapshot);
}
}  // namespace nodex
