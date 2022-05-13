#include "environment_data.h"

namespace nodex {
namespace per_thread {
thread_local EnvironmentData* environment_data = nullptr;
}

EnvironmentData* EnvironmentData::GetCurrent(v8::Isolate* isolate) {
  if (per_thread::environment_data == nullptr ||
      per_thread::environment_data->isolate() != isolate) {
    return nullptr;
  }
  return per_thread::environment_data;
}

EnvironmentData::EnvironmentData(v8::Isolate* isolate) : isolate_(isolate) {
  per_thread::environment_data = this;
}

void EnvironmentData::Create(v8::Isolate* isolate) {
  new EnvironmentData(isolate);
}
}  // namespace nodex