#include "sampling_heap_profile.h"

namespace nodex {
using v8::Local;
using v8::Object;
using v8::String;
using v8::Integer;
using v8::Value;
using v8::Number;
using v8::Array;
#if NODE_MODULE_VERSION >= 0x0030
using v8::AllocationProfile;
#endif

SamplingHeapProfile::SamplingHeapProfile () {}
SamplingHeapProfile::~SamplingHeapProfile () {}

#if NODE_MODULE_VERSION >= 0x0030
Local<Object> TranslateAllocationProfile(AllocationProfile::Node* node) {
  Local<Object> js_node = Nan::New<Object>();

  // add call frame
  Local<Object> call_frame = Nan::New<Object>();
  Nan::Set(call_frame, Nan::New<String>("functionName").ToLocalChecked(), node->name);
  Nan::Set(call_frame, Nan::New<String>("scriptId").ToLocalChecked(), Nan::New<Integer>(node->script_id));
  // Local<String> url = String::Concat(node->script_name->ToString(), Nan::New<String>(":").ToLocalChecked());
  // url = String::Concat(url->ToString(), Nan::New<String>(std::to_string(node->line_number)).ToLocalChecked());
  Nan::Set(call_frame, Nan::New<String>("url").ToLocalChecked(), node->script_name);
  Nan::Set(call_frame, Nan::New<String>("lineNumber").ToLocalChecked(), Nan::New<Integer>(node->line_number));
  Nan::Set(call_frame, Nan::New<String>("columnNumber").ToLocalChecked(), Nan::New<Integer>(node->column_number));
  Nan::Set(js_node, Nan::New<String>("callFrame").ToLocalChecked(), call_frame);

  // add self size
  int selfSize = 0;
  for (size_t i = 0; i < node->allocations.size(); i++) {
    AllocationProfile::Allocation alloc = node->allocations[i];
    selfSize += alloc.size * alloc.count;
  }
  Nan::Set(js_node, Nan::New<String>("selfSize").ToLocalChecked(), Nan::New<Integer>(selfSize));

  // add children
  Local<Array> children = Nan::New<Array>(node->children.size());
  for (size_t i = 0; i < node->children.size(); i++) {
    Nan::Set(children, i, TranslateAllocationProfile(node->children[i]));
  }
  Nan::Set(js_node, Nan::New<String>("children").ToLocalChecked(), children);

  return js_node;
}
#endif

void SamplingHeapProfile::Initialize (Local<Object> target) {
  Nan::HandleScope scope;
  // local object
  Local<Object> samplingHeapProfile = Nan::New<Object>();
  // methods
  Nan::SetMethod(samplingHeapProfile, "startSamplingHeapProfiling", SamplingHeapProfile::StartSamplingHeapProfiling);
  Nan::SetMethod(samplingHeapProfile, "stopSamplingHeapProfiling", SamplingHeapProfile::StopSamplingHeapProfiling);
  // set  samplingHeap
  Nan::Set(target, Nan::New<String>("samplingHeap").ToLocalChecked(), samplingHeapProfile);
}

NAN_METHOD(SamplingHeapProfile::StartSamplingHeapProfiling) {
#if NODE_MODULE_VERSION >= 0x0030
  if (info.Length() == 2) {
    if (!info[0]->IsUint32()) {
      return Nan::ThrowTypeError("first argument type must be uint32");
    }
    if (!info[1]->IsNumber()) {
      return Nan::ThrowTypeError("first argument type must be number");
    }
    uint64_t sample_interval = Nan::To<uint32_t>(info[0]).ToChecked();
    int stack_depth = Nan::To<Integer>(info[1]).ToLocalChecked()->Value();
    v8::Isolate::GetCurrent()->GetHeapProfiler()->StartSamplingHeapProfiler(sample_interval, stack_depth);
  } else {
    v8::Isolate::GetCurrent()->GetHeapProfiler()->StartSamplingHeapProfiler();
  }
#endif
}

NAN_METHOD(SamplingHeapProfile::StopSamplingHeapProfiling) {
#if NODE_MODULE_VERSION >= 0x0030
  // return allocationProfile
  AllocationProfile* profile = v8::Isolate::GetCurrent()->GetHeapProfiler()->GetAllocationProfile();
  AllocationProfile::Node* root = profile->GetRootNode();
  // translate
  Local<Object> js_node = TranslateAllocationProfile(root);
  Local<Object> result = Nan::New<Object>();
  Nan::Set(result, Nan::New<String>("head").ToLocalChecked(), js_node);
  info.GetReturnValue().Set(result);
  free(profile);
  // stop sampling heap profile
  v8::Isolate::GetCurrent()->GetHeapProfiler()->StopSamplingHeapProfiler();
#endif
}
}