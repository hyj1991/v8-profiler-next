#ifndef NODE_PROFILE_NODE_
#define NODE_PROFILE_NODE_

#include "nan.h"
#include "v8-inner.h"
#include "v8-profiler.h"
using namespace std;

namespace nodex {

class ProfileNode {
 public:
  static v8::Local<v8::Value> New(v8::Isolate* isolate,
                                  const v8::CpuProfileNode* node,
                                  uint32_t type);

 private:
  static void setNodes_(v8::Isolate* isolate, const v8::CpuProfileNode* node,
                        std::vector<v8::Local<v8::Object> >& list,
                        const EscapableHandleScope& scope);
#if (NODE_MODULE_VERSION >= 42)
  static v8::Local<v8::Value> GetLineTicks_(v8::Isolate* isolate,
                                            const v8::CpuProfileNode* node);
#endif
};

}  // namespace nodex
#endif  // NODE_PROFILE_NODE_