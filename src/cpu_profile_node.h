#ifndef NODE_PROFILE_NODE_
#define NODE_PROFILE_NODE_

#include "v8-profiler.h"
#include "nan.h"
using namespace std;

namespace nodex {

class ProfileNode {
public:
  static v8::Local<v8::Value> New(const v8::CpuProfileNode* node, uint32_t type);
  static uint32_t UIDCounter;

private:
  static void setNodes_(const v8::CpuProfileNode* node, v8::Local<v8::Array> nodes, int* index);
  static void getTotalCount_(const v8::CpuProfileNode* node, int* total);
  static void setNodes_(const v8::CpuProfileNode* node, std::vector<v8::Local<v8::Object> >& list, const Nan::EscapableHandleScope& scope);
#if (NODE_MODULE_VERSION >= 42)
  static v8::Local<v8::Value> GetLineTicks_(const v8::CpuProfileNode* node);
#endif
};

}
#endif  // NODE_PROFILE_NODE_
