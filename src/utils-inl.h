#ifndef UTILS_H
#define UTILS_H

#include "environment_data.h"
#include "nan.h"

namespace nodex {
inline void logger(v8::Isolate* isolate, const char* message) {
  EnvironmentData* env_data = EnvironmentData::GetCurrent(isolate);
  if (env_data == nullptr) return;

  if (env_data->debug()) {
    printf("[thread %d] %s\n", env_data->thread_id(), message);
  }
}

}  // namespace nodex

#endif