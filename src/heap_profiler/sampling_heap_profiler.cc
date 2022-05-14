#include "sampling_heap_profiler.h"

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

// class SamplingHeapProfiler
SamplingHeapProfiler::SamplingHeapProfiler() {}
SamplingHeapProfiler::~SamplingHeapProfiler() {}

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

NAN_METHOD(SamplingHeapProfiler::StartSamplingHeapProfiling) {
  EnvironmentData* env_data = EnvironmentData::GetCurrent(info);
  if (env_data == nullptr) return;

  if (env_data->heap_profiler() == nullptr) {
    env_data->heap_profiler() =
        InnerSamplingHeapProfiler::Create(env_data->isolate());
  }

  {
    HandleScope scope(env_data->isolate());
    uint64_t sample_interval = -1;
    int stack_depth = -1;

    if (info.Length() == 2) {
      if (!info[0]->IsUint32()) {
        return Nan::ThrowTypeError("first argument type must be uint32");
      }
      if (!info[1]->IsNumber()) {
        return Nan::ThrowTypeError("first argument type must be number");
      }
      sample_interval = Nan::To<uint32_t>(info[0]).ToChecked();
      stack_depth =
          static_cast<int>(Nan::To<Integer>(info[1]).ToLocalChecked()->Value());
    }

    env_data->heap_profiler()->StartSamplingHeapProfiling(sample_interval,
                                                          stack_depth);
  }
}

NAN_METHOD(SamplingHeapProfiler::StopSamplingHeapProfiling) {
  EnvironmentData* env_data = EnvironmentData::GetCurrent(info);
  if (env_data == nullptr) return;

  if (env_data->heap_profiler() == nullptr) {
    env_data->heap_profiler() =
        InnerSamplingHeapProfiler::Create(env_data->isolate());
  }

  {
    HandleScope scope(env_data->isolate());
    // return allocationProfile
    AllocationProfile* profile =
        env_data->heap_profiler()->GetAllocationProfile();
    AllocationProfile::Node* root = profile->GetRootNode();
    // translate
    Local<Object> js_node =
        env_data->heap_profiler()->TranslateAllocationProfile(root);
    Local<Object> result = Nan::New<Object>();
    Nan::Set(result, Nan::New<String>("head").ToLocalChecked(), js_node);
    info.GetReturnValue().Set(result);
    env_data->heap_profiler()->CheckProfile(profile);
  }
}

// class InnerSamplingHeapProfiler
InnerSamplingHeapProfiler* InnerSamplingHeapProfiler::Create(
    v8::Isolate* isolate) {
  return new InnerSamplingHeapProfiler(isolate);
}

void InnerSamplingHeapProfiler::StartSamplingHeapProfiling(
    uint64_t sample_interval, int stack_depth) {
#if NODE_MODULE_VERSION >= 0x0030
  if (sample_interval >= 0 && stack_depth >= 0) {
    this->isolate()->GetHeapProfiler()->StartSamplingHeapProfiler(
        sample_interval, stack_depth);
  } else {
    this->isolate()->GetHeapProfiler()->StartSamplingHeapProfiler();
  }
#endif
}

v8::AllocationProfile* InnerSamplingHeapProfiler::GetAllocationProfile() {
#if NODE_MODULE_VERSION >= 0x0030
  AllocationProfile* profile =
      this->isolate()->GetHeapProfiler()->GetAllocationProfile();
  return profile;
#endif
}

void InnerSamplingHeapProfiler::CheckProfile(v8::AllocationProfile* profile) {
#if NODE_MODULE_VERSION >= 0x0030
  free(profile);
  this->isolate()->GetHeapProfiler()->StopSamplingHeapProfiler();
#endif
}

Local<Object> InnerSamplingHeapProfiler::TranslateAllocationProfile(
    AllocationProfile::Node* node) {
#if NODE_MODULE_VERSION >= 0x0030
  EscapableHandleScope scope(this->isolate());

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
    Nan::Set(children, i, TranslateAllocationProfile(node->children[i]));
  }
  Nan::Set(js_node, Nan::New<String>("children").ToLocalChecked(), children);

  return scope.Escape(js_node);
#endif
}
}  // namespace nodex