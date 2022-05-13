#ifndef NODE_PROFILE_NODE_
#define NODE_PROFILE_NODE_

#include "nan.h"
#include "v8-inner.h"
#include "v8-profiler.h"
using namespace std;

namespace nodex {

class ProfileNode {
 public:
  ProfileNode(v8::Isolate* isolate) : isolate_(isolate) {}
  v8::Local<v8::Value> New(const v8::CpuProfileNode* node, uint32_t type);
  v8::Isolate* isolate() { return isolate_; }

 private:
  v8::Isolate* isolate_;
  uint32_t UIDCounter = 1;
  void setNodes_(const v8::CpuProfileNode* node, v8::Local<v8::Array> nodes,
                 int* index);
  void getTotalCount_(const v8::CpuProfileNode* node, int* total);
  void setNodes_(const v8::CpuProfileNode* node,
                 std::vector<v8::Local<v8::Object> >& list,
                 const EscapableHandleScope& scope);
#if (NODE_MODULE_VERSION >= 42)
  v8::Local<v8::Value> GetLineTicks_(const v8::CpuProfileNode* node);
#endif
};

}  // namespace nodex
#endif  // NODE_PROFILE_NODE_
