#ifndef NODE_HEAP_OUTPUT_STREAM_
#define NODE_HEAP_OUTPUT_STREAM_

#include "nan.h"
#include "v8-profiler.h"

namespace nodex {

class OutputStreamAdapter : public v8::OutputStream {
 public:
  OutputStreamAdapter(v8::Isolate* isolate, v8::Local<v8::Function> _iterator,
                      v8::Local<v8::Function> _callback)
      : isolate_(isolate),
        abort(Nan::False()),
        iterator(_iterator),
        callback(_callback){};

  void EndOfStream();

  int GetChunkSize();

  WriteResult WriteAsciiChunk(char* data, int size);

  WriteResult WriteHeapStatsChunk(v8::HeapStatsUpdate* data, int count);

  v8::Isolate* isolate() { return isolate_; }

 private:
  v8::Isolate* isolate_;
  v8::Local<v8::Value> abort;
  v8::Local<v8::Function> iterator;
  v8::Local<v8::Function> callback;
};
}  // namespace nodex
#endif  // NODE_PROFILE_
