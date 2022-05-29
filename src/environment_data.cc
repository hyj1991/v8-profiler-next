#include "environment_data.h"

#include "node-inner.h"
#include "utils-inl.h"
#include "v8-inner.h"

namespace nodex {
namespace per_thread {
thread_local EnvironmentData* environment_data = nullptr;
}

// static
EnvironmentData* EnvironmentData::GetCurrent(v8::Isolate* isolate) {
  if (per_thread::environment_data == nullptr ||
      per_thread::environment_data->isolate() != isolate) {
    return nullptr;
  }
  return per_thread::environment_data;
}

EnvironmentData* EnvironmentData::GetCurrent(
    const Nan::FunctionCallbackInfo<v8::Value>& info) {
  return EnvironmentData::GetCurrent(info.GetIsolate());
}

EnvironmentData* EnvironmentData::GetCurrent(
    const Nan::PropertyCallbackInfo<v8::Value>& info) {
  return EnvironmentData::GetCurrent(info.GetIsolate());
}

void EnvironmentData::JsSetupEnvironmentData(
    const Nan::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  EnvironmentData* env_data = EnvironmentData::GetCurrent(isolate);
  if (env_data == nullptr) return;

  // if env_data is not nullptr
  // v8::Isolate::GetCurrent() equal to info.GetIsolate()
  HandleScope scope(isolate);
  v8::Local<v8::Object> data = Nan::To<v8::Object>(info[0]).ToLocalChecked();

  // set thread_id
  GET_JS_ENVIRONMENT_DATA(data, thread_id, Number);
  env_data->thread_id() = static_cast<int>(thread_id->Value());

  // set debug
  GET_JS_ENVIRONMENT_DATA(data, debug, Boolean);
  env_data->debug() = debug->Value();

  // set global_check
  v8::Local<v8::Function> global_check = v8::Local<v8::Function>::Cast(info[1]);
  v8::Local<v8::Object> check_result =
      Nan::To<v8::Object>(Nan::Call(global_check,
                                    isolate->GetCurrentContext()->Global(), 0,
                                    NULL)
                              .ToLocalChecked())
          .ToLocalChecked();
  GET_JS_ENVIRONMENT_DATA(check_result, greater_than_12_15_0, Boolean);
  env_data->greater_than_12_15_0() = greater_than_12_15_0->Value();
}

EnvironmentData::EnvironmentData(v8::Isolate* isolate) : isolate_(isolate) {
  per_thread::environment_data = this;
}

void EnvironmentData::Create(v8::Isolate* isolate) {
  new EnvironmentData(isolate);
  NodeAtExit(isolate, AtExit, isolate);
}

void EnvironmentData::AtExit(void* arg) {
  v8::Isolate* isolate = reinterpret_cast<v8::Isolate*>(arg);
  EnvironmentData* env_data = GetCurrent(isolate);
  if (env_data == nullptr) return;

  logger(isolate, "clear env_data before exit");
  delete env_data;
}
}  // namespace nodex