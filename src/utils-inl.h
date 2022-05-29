#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>

#include "environment_data.h"
#include "nan.h"

namespace nodex {
inline void logger(v8::Isolate* isolate, const char* format, ...) {
  EnvironmentData* env_data = EnvironmentData::GetCurrent(isolate);
  if (env_data == nullptr) return;

  if (env_data->debug()) {
    va_list args;
    va_start(args, format);
    char tmp_log[2048];
    vsnprintf(tmp_log, sizeof(tmp_log), format, args);
    printf("[thread %d] %s\n", env_data->thread_id(), tmp_log);
    va_end(args);
  }
}

}  // namespace nodex

#endif