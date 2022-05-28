#ifndef NODE_ISOLATE_AWARE_
#define NODE_ISOLATE_AWARE_

#include "nan.h"

namespace nodex {
#define INNER_METHOD(func) \
  void func(const Nan::FunctionCallbackInfo<v8::Value>& info)

#define INNER_GETTER(func) \
  void func(const Nan::PropertyCallbackInfo<v8::Value>& info)

#define CALL_NAN_METHOD(property, origin, inner, func) \
  NAN_METHOD(origin::func) { CALL_INNER_METHOD(property, inner, func); }

#define CALL_NAN_GETTER(property, origin, inner, func) \
  NAN_GETTER(origin::func) { CALL_INNER_METHOD(property, inner, func); }

#define CALL_INNER_METHOD(property, type, func)                             \
  EnvironmentData* env_data = EnvironmentData::GetCurrent(info);            \
  if (env_data == nullptr) return;                                          \
  if (env_data->property() == nullptr) {                                    \
    env_data->property() = IsolateAware::Create<type>(env_data->isolate()); \
  }                                                                         \
  env_data->property()->func(info);

class IsolateAware {
 public:
  template <typename T>
  static T* Create(v8::Isolate* isolate) {
    return new T(isolate);
  }

  IsolateAware(v8::Isolate* isolate) : isolate_(isolate) {}
  v8::Isolate* isolate() { return isolate_; }

 private:
  v8::Isolate* isolate_;
};
}  // namespace nodex

#endif