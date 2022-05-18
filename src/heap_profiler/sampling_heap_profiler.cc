#include "heap_profiler/sampling_heap_profiler.h"

#include "environment_data.h"
#include "v8-inner.h"

namespace nodex {
using v8::Array;
using v8::Integer;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;
#if NODE_MODULE_VERSION >= 0x0030
using v8::AllocationProfile;
#endif

SamplingHeapProfiler::SamplingHeapProfiler() {}
SamplingHeapProfiler::~SamplingHeapProfiler() {}

#if NODE_MODULE_VERSION >= 0x0030
Local<Object> TranslateAllocationProfile(v8::Isolate* isolate,
                                         AllocationProfile::Node* node) {
  EscapableHandleScope scope(isolate);

  Local<Object> js_node = Nan::New<Object>();

  // add call frame
  Local<Object> call_frame = Nan::New<Object>();
  Nan::Set(call_frame, Nan::New<String>("functionName").ToLocalChecked(),
           node->name);
  Nan::Set(call_frame, Nan::New<String>("scriptId").ToLocalChecked(),
           Nan::New<Integer>(node->script_id));
  // Local<String> url = String::Concat(node->script_name->ToString(),
  // Nan::New<String>(":").ToLocalChecked()); url =
  // String::Concat(url->ToString(),
  // Nan::New<String>(std::to_string(node->line_number)).ToLocalChecked());
  Nan::Set(call_frame, Nan::New<String>("url").ToLocalChecked(),
           node->script_name);
  Nan::Set(call_frame, Nan::New<String>("lineNumber").ToLocalChecked(),
           Nan::New<Integer>(node->line_number));
  Nan::Set(call_frame, Nan::New<String>("columnNumber").ToLocalChecked(),
           Nan::New<Integer>(node->column_number));
  Nan::Set(js_node, Nan::New<String>("callFrame").ToLocalChecked(), call_frame);

  // add self size
  int selfSize = 0;
  for (size_t i = 0; i < node->allocations.size(); i++) {
    AllocationProfile::Allocation alloc = node->allocations[i];
    selfSize += alloc.size * alloc.count;
  }
  Nan::Set(js_node, Nan::New<String>("selfSize").ToLocalChecked(),
           Nan::New<Integer>(selfSize));

  // add children
  Local<Array> children =
      Nan::New<Array>(static_cast<int>(node->children.size()));
  for (uint32_t i = 0; i < node->children.size(); i++) {
    Nan::Set(children, i,
             TranslateAllocationProfile(isolate, node->children[i]));
  }
  Nan::Set(js_node, Nan::New<String>("children").ToLocalChecked(), children);

  return scope.Escape(js_node);
}
#endif

void SamplingHeapProfiler::Initialize(Local<Object> target) {
  HandleScope scope(target->GetIsolate());
  // local object
  Local<Object> samplingHeapProfile = Nan::New<Object>();
  // methods
  Nan::SetMethod(samplingHeapProfile, "startSamplingHeapProfiling",
                 SamplingHeapProfiler::StartSamplingHeapProfiling);
  Nan::SetMethod(samplingHeapProfile, "stopSamplingHeapProfiling",
                 SamplingHeapProfiler::StopSamplingHeapProfiling);
  // set  samplingHeap
  Nan::Set(target, Nan::New<String>("samplingHeap").ToLocalChecked(),
           samplingHeapProfile);
}

SAMPLING_HEAP_PROFILER_METHODS(NAN_SAMPLING_HEAP_PROFILER_METHOD);

INNER_METHOD(InnerSamplingHeapProfiler::StartSamplingHeapProfiling) {
  HandleScope scope(this->isolate());

#if NODE_MODULE_VERSION >= 0x0030
  if (info.Length() == 2) {
    if (!info[0]->IsUint32()) {
      return Nan::ThrowTypeError("first argument type must be uint32");
    }
    if (!info[1]->IsNumber()) {
      return Nan::ThrowTypeError("first argument type must be number");
    }
    uint64_t sample_interval = Nan::To<uint32_t>(info[0]).ToChecked();
    int stack_depth =
        static_cast<int>(Nan::To<Integer>(info[1]).ToLocalChecked()->Value());
    v8::Isolate::GetCurrent()->GetHeapProfiler()->StartSamplingHeapProfiler(
        sample_interval, stack_depth);
  } else {
    v8::Isolate::GetCurrent()->GetHeapProfiler()->StartSamplingHeapProfiler();
  }
#endif
}

INNER_METHOD(InnerSamplingHeapProfiler::StopSamplingHeapProfiling) {
  HandleScope scope(this->isolate());

#if NODE_MODULE_VERSION >= 0x0030
  // return allocationProfile
  AllocationProfile* profile =
      v8::Isolate::GetCurrent()->GetHeapProfiler()->GetAllocationProfile();
  AllocationProfile::Node* root = profile->GetRootNode();
  // translate
  Local<Object> js_node = TranslateAllocationProfile(this->isolate(), root);
  Local<Object> result = Nan::New<Object>();
  Nan::Set(result, Nan::New<String>("head").ToLocalChecked(), js_node);
  info.GetReturnValue().Set(result);
  free(profile);
  // stop sampling heap profile
  v8::Isolate::GetCurrent()->GetHeapProfiler()->StopSamplingHeapProfiler();
#endif
}
}  // namespace nodex