#ifndef INNER_V8_H_
#define INNER_V8_H_

#include "nan.h"

namespace nodex {
/**
 * A compatible class for v8::HandleScope. Not using Nan::HandleScope because of
 * its implicit v8::Isolate::GetCurrent.
 */
class HandleScope {
  v8::HandleScope scope;

 public:
#if NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
  inline HandleScope(v8::Isolate* isolate) : scope(isolate) {}
  inline static int NumberOfHandles(v8::Isolate* isolate) {
    return v8::HandleScope::NumberOfHandles(isolate);
  }
#else
  inline HandleScope() : scope() {}
  inline static int NumberOfHandles() {
    return v8::HandleScope::NumberOfHandles();
  }
#endif

 private:
  // Make it hard to create heap-allocated or illegal handle scopes by
  // disallowing certain operations.
  HandleScope(const HandleScope&);
  void operator=(const HandleScope&);
  void* operator new(size_t size);
  void operator delete(void*, size_t) { abort(); }
};

class EscapableHandleScope {
 public:
#if NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION
  inline EscapableHandleScope(v8::Isolate* isolate) : scope(isolate) {}

  inline static int NumberOfHandles(v8::Isolate* isolate) {
    return v8::EscapableHandleScope::NumberOfHandles(isolate);
  }

  template <typename T>
  inline v8::Local<T> Escape(v8::Local<T> value) {
    return scope.Escape(value);
  }

 private:
  v8::EscapableHandleScope scope;
#else
  inline EscapableHandleScope() : scope() {}

  inline static int NumberOfHandles() {
    return v8::HandleScope::NumberOfHandles();
  }

  template <typename T>
  inline v8::Local<T> Escape(v8::Local<T> value) {
    return scope.Close(value);
  }

 private:
  v8::HandleScope scope;
#endif

 private:
  // Make it hard to create heap-allocated or illegal handle scopes by
  // disallowing certain operations.
  EscapableHandleScope(const EscapableHandleScope&);
  void operator=(const EscapableHandleScope&);
  void* operator new(size_t size);
  void operator delete(void*, size_t) { abort(); }
};

}  // namespace nodex

#endif /* INNER_V8_H_ */
