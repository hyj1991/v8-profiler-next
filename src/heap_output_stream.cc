#include "heap_output_stream.h"

namespace nodex {
using v8::Array;
using v8::HeapStatsUpdate;
using v8::Local;
using v8::Value;
using v8::String;
using v8::OutputStream;
using v8::Function;
using Nan::TryCatch;;
using v8::Integer;

void OutputStreamAdapter::EndOfStream() {
  Nan::HandleScope scope;
  TryCatch try_catch;
  Nan::Call(callback, Nan::GetCurrentContext()->Global(), 0, NULL);

  if (try_catch.HasCaught()) {
    Nan::ThrowError(try_catch.Exception());
  }
}

int OutputStreamAdapter::GetChunkSize() {
  return 51200;
}

OutputStream::WriteResult OutputStreamAdapter::WriteAsciiChunk(char* data, int size) {
  Nan::HandleScope scope;

  Local<Value> argv[2] = {
    Nan::New<String>(data, size).ToLocalChecked(),
    Nan::New<Integer>(size)
  };

  TryCatch try_catch;
  abort = (Nan::Call(iterator, Nan::GetCurrentContext()->Global(), 2, argv)).ToLocalChecked();

  if (try_catch.HasCaught()) {
    Nan::ThrowError(try_catch.Exception());
    return kAbort;
  }

  return abort->IsFalse() ? kAbort : kContinue;
}

OutputStream::WriteResult OutputStreamAdapter::WriteHeapStatsChunk(HeapStatsUpdate* data, int count) {
  Nan::HandleScope scope;

  Local<Array> samples = Nan::New<Array>();
  for (int index = 0; index < count; index++) {
    int offset = index * 3;
    Nan::Set(samples, offset, Nan::New<Integer>(data[index].index));
    Nan::Set(samples, offset + 1, Nan::New<Integer>(data[index].count));
    Nan::Set(samples, offset + 2, Nan::New<Integer>(data[index].size));
  }

  Local<Value> argv[1] = {samples};

  TryCatch try_catch;
  abort = (Nan::Call(iterator, Nan::GetCurrentContext()->Global(), 1, argv)).ToLocalChecked();

  if (try_catch.HasCaught()) {
    Nan::ThrowError(try_catch.Exception());
    return kAbort;
  }

  return abort->IsFalse() ? kAbort : kContinue;
}
} //namespace nodex
