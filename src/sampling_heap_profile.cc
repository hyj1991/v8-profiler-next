#include "sampling_heap_profile.h"

namespace nodex {
  using v8::Local;
  using v8::Object;
  using v8::String;
  using v8::Integer;
  using v8::Value;
  using v8::Number;
  using v8::Array;
  using v8::AllocationProfile;

  SamplingHeapProfile::SamplingHeapProfile () {}
  SamplingHeapProfile::~SamplingHeapProfile () {}

  Local<Value> TranslateAllocationProfile(AllocationProfile::Node* node) {
    Local<Object> js_node = Nan::New<Object>();
    js_node->Set(Nan::New<String>("name").ToLocalChecked(),
      node->name);
    js_node->Set(Nan::New<String>("scriptName").ToLocalChecked(),
      node->script_name);
    js_node->Set(Nan::New<String>("scriptId").ToLocalChecked(),
      Nan::New<Integer>(node->script_id));
    js_node->Set(Nan::New<String>("lineNumber").ToLocalChecked(),
      Nan::New<Integer>(node->line_number));
    js_node->Set(Nan::New<String>("columnNumber").ToLocalChecked(),
      Nan::New<Integer>(node->column_number));
    Local<Array> children = Nan::New<Array>(node->children.size());
    for (size_t i = 0; i < node->children.size(); i++) {
      children->Set(i, TranslateAllocationProfile(node->children[i]));
    }
    js_node->Set(Nan::New<String>("children").ToLocalChecked(),
      children);
    Local<Array> allocations = Nan::New<Array>(node->allocations.size());
    for (size_t i = 0; i < node->allocations.size(); i++) {
      AllocationProfile::Allocation alloc = node->allocations[i];
      Local<Object> js_alloc = Nan::New<Object>();
      js_alloc->Set(Nan::New<String>("size").ToLocalChecked(),
        Nan::New<Number>(alloc.size));
      js_alloc->Set(Nan::New<String>("count").ToLocalChecked(),
        Nan::New<Number>(alloc.count));
      allocations->Set(i, js_alloc);
    }
    js_node->Set(Nan::New<String>("allocations").ToLocalChecked(),
      allocations);
    return js_node;
  }

  void SamplingHeapProfile::Initialize (Local<Object> target) {
    Nan::HandleScope scope;
    // local object
    Local<Object> samplingHeapProfile = Nan::New<Object>();
    // methods
    Nan::SetMethod(samplingHeapProfile, "startSamplingHeapProfiling", SamplingHeapProfile::StartSamplingHeapProfiling);
    Nan::SetMethod(samplingHeapProfile, "stopSamplingHeapProfiling", SamplingHeapProfile::StopSamplingHeapProfiling);
    // set  samplingHeap
    target->Set(Nan::New<String>("samplingHeap").ToLocalChecked(), samplingHeapProfile);
  }

  NAN_METHOD(SamplingHeapProfile::StartSamplingHeapProfiling) {
    if (info.Length() == 2) {
      if (!info[0]->IsUint32()) {
        return Nan::ThrowTypeError("first argument type must be uint32");
      }
      if (!info[1]->IsNumber()) {
        return Nan::ThrowTypeError("first argument type must be number");
      }
      uint64_t sample_interval = info[0].As<Integer>()->Uint32Value();
      int stack_depth = info[1].As<Integer>()->IntegerValue();
      v8::Isolate::GetCurrent()->GetHeapProfiler()->StartSamplingHeapProfiler(sample_interval, stack_depth);
    } else {
      v8::Isolate::GetCurrent()->GetHeapProfiler()->StartSamplingHeapProfiler();
    }
  }
  
  NAN_METHOD(SamplingHeapProfile::StopSamplingHeapProfiling) {
    // return allocationProfile
    AllocationProfile* profile = v8::Isolate::GetCurrent()->GetHeapProfiler()->GetAllocationProfile();
    AllocationProfile::Node* root = profile->GetRootNode();
    info.GetReturnValue().Set(TranslateAllocationProfile(root));
    free(profile);
    // stop sampling heap profile
    v8::Isolate::GetCurrent()->GetHeapProfiler()->StopSamplingHeapProfiler();
  }
}