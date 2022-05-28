#ifndef INNER_NODE_H
#define INNER_NODE_H

#include "nan.h"

namespace nodex {
inline void NodeAtExit(v8::Isolate* isolate, void (*cb)(void* arg), void* arg) {
#if NODE_MAJOR_VERSION >= 10
  // node::GetCurrentEnvironment is available since v10.x.
  // We don't need to support multiple environments before v10.x.
  node::Environment* env =
      node::GetCurrentEnvironment(isolate->GetCurrentContext());
  node::AtExit(env, cb, arg);
#else
  node::AtExit(cb, arg);
#endif
}
}  // namespace nodex

#endif