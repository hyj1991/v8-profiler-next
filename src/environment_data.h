#include "v8.h"

namespace nodex {
class EnvironmentData {
 public:
  EnvironmentData(v8::Isolate* isolate);
  EnvironmentData* GetCurrent(v8::Isolate* isolate);
  v8::Isolate* isolate() { return isolate_; };
  static void Create(v8::Isolate* isolate);

 private:
  v8::Isolate* isolate_;
};
}  // namespace nodex