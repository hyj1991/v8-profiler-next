#include "cpu_profile_node.h"

namespace nodex {
using v8::CpuProfileNode;
using v8::String;
using v8::Number;
using v8::Integer;
using v8::Value;
using v8::Local;
using v8::Object;
using v8::Array;

uint32_t ProfileNode::UIDCounter = 1;

#if (NODE_MODULE_VERSION >= 42)
Local<Value> ProfileNode::GetLineTicks_(const CpuProfileNode* node) {
  Nan::EscapableHandleScope scope;

  uint32_t count = node->GetHitLineCount();
  v8::CpuProfileNode::LineTick *entries = new v8::CpuProfileNode::LineTick[count];
  bool result = node->GetLineTicks(entries, count);

  Local<Value> lineTicks;
  if (result) {
    Local<Array> array = Nan::New<Array>(count);
    for (uint32_t index = 0; index < count; index++) {
      Local<Object> tick = Nan::New<Object>();
      Nan::Set(tick, Nan::New<String>("line").ToLocalChecked(), Nan::New<Integer>(entries[index].line));
      Nan::Set(tick, Nan::New<String>("hitCount").ToLocalChecked(), Nan::New<Integer>(entries[index].hit_count));
      Nan::Set(array, index, tick);
    }
    lineTicks = array;
  } else {
    lineTicks = Nan::Null();
  }

  delete[] entries;
  return scope.Escape(lineTicks);
}
#endif

Local<Value> ProfileNode::New (const CpuProfileNode* node) {
  Nan::EscapableHandleScope scope;

  int32_t count = node->GetChildrenCount();
  Local<Object> profile_node = Nan::New<Object>();
  Local<Array> children = Nan::New<Array>(count);

  for (int32_t index = 0; index < count; index++) {
    Nan::Set(children, index, ProfileNode::New(node->GetChild(index)));
  }

  Nan::Set(profile_node, Nan::New<String>("functionName").ToLocalChecked(), node->GetFunctionName());
  Nan::Set(profile_node, Nan::New<String>("url").ToLocalChecked(), node->GetScriptResourceName());
  Nan::Set(profile_node, Nan::New<String>("lineNumber").ToLocalChecked(), Nan::New<Integer>(node->GetLineNumber()));
  Nan::Set(profile_node, Nan::New<String>("columnNumber").ToLocalChecked(), Nan::New<Integer>(node->GetColumnNumber()));
#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION < 8)
  Nan::Set(profile_node, Nan::New<String>("callUID").ToLocalChecked(), Nan::New<Number>(node->GetCallUid()));
#endif
#if (NODE_MODULE_VERSION > 0x000B)
  Nan::Set(profile_node, Nan::New<String>("bailoutReason").ToLocalChecked(), Nan::New<String>(node->GetBailoutReason()).ToLocalChecked());
  Nan::Set(profile_node, Nan::New<String>("id").ToLocalChecked(), Nan::New<Integer>(node->GetNodeId()));
  Nan::Set(profile_node, Nan::New<String>("scriptId").ToLocalChecked(), Nan::New<Integer>(node->GetScriptId()));
  Nan::Set(profile_node, Nan::New<String>("hitCount").ToLocalChecked(), Nan::New<Integer>(node->GetHitCount()));
#else
  Nan::Set(profile_node, Nan::New<String>("bailoutReason").ToLocalChecked(), Nan::New<String>("no reason").ToLocalChecked());
  Nan::Set(profile_node, Nan::New<String>("id").ToLocalChecked(), Nan::New<Integer>(UIDCounter++));
  Nan::Set(profile_node, Nan::New<String>("hitCount").ToLocalChecked(), Nan::New<Integer>(static_cast<uint32_t>(node->GetSelfSamplesCount())));
  //TODO(3y3): Nan::Set(profile_node, Nan::New<String>("scriptId").ToLocalChecked(), Nan::New<Integer>(node->GetScriptId()));
#endif
  Nan::Set(profile_node, Nan::New<String>("children").ToLocalChecked(), children);

#if (NODE_MODULE_VERSION >= 42)
  Local<Value> lineTicks = GetLineTicks_(node);
  if (!lineTicks->IsNull()) {
    Nan::Set(profile_node, Nan::New<String>("lineTicks").ToLocalChecked(), lineTicks);
  }
#endif

  return scope.Escape(profile_node);
}

}
