#include "cpu_profiler.h"
#include "cpu_profile.h"

namespace nodex {
using v8::CpuProfile;
using v8::Local;
using v8::Object;
using v8::Array;
using v8::String;

CpuProfiler::CpuProfiler () {}
CpuProfiler::~CpuProfiler () {}

#if (NODE_MODULE_VERSION > 0x0039)
v8::CpuProfiler* current_cpuprofiler = v8::CpuProfiler::New(v8::Isolate::GetCurrent());
#endif

void CpuProfiler::Initialize (Local<Object> target) {
  Nan::HandleScope scope;

  Local<Object> cpuProfiler = Nan::New<Object>();
  Local<Array> profiles = Nan::New<Array>();

  Nan::SetMethod(cpuProfiler, "startProfiling", CpuProfiler::StartProfiling);
  Nan::SetMethod(cpuProfiler, "stopProfiling", CpuProfiler::StopProfiling);
  Nan::SetMethod(cpuProfiler, "setSamplingInterval", CpuProfiler::SetSamplingInterval);
  Nan::Set(cpuProfiler, Nan::New<String>("profiles").ToLocalChecked(), profiles);

  Profile::profiles.Reset(profiles);
  Nan::Set(target, Nan::New<String>("cpu").ToLocalChecked(), cpuProfiler);
}

NAN_METHOD(CpuProfiler::StartProfiling) {
  Local<String> title = Nan::To<String>(info[0]).ToLocalChecked();

#if (NODE_MODULE_VERSION > 0x0039)
  bool recsamples = Nan::To<bool>(info[1]).ToChecked();
  current_cpuprofiler->StartProfiling(title, recsamples);
#elif (NODE_MODULE_VERSION > 0x000B)
  bool recsamples = Nan::To<bool>(info[1]).ToChecked();
  v8::Isolate::GetCurrent()->GetCpuProfiler()->StartProfiling(title, recsamples);
#else
  v8::CpuProfiler::StartProfiling(title);
#endif
}

NAN_METHOD(CpuProfiler::StopProfiling) {
  const CpuProfile* profile;

  Local<String> title = Nan::EmptyString();
  if (info.Length()) {
    if (info[0]->IsString()) {
      title = Nan::To<String>(info[0]).ToLocalChecked();
    } else if (!info[0]->IsUndefined()) {
      return Nan::ThrowTypeError("Wrong argument [0] type (wait String)");
    }
  }

#if (NODE_MODULE_VERSION > 0x0039)
  profile = current_cpuprofiler->StopProfiling(title);
#elif (NODE_MODULE_VERSION > 0x000B)
  profile = v8::Isolate::GetCurrent()->GetCpuProfiler()->StopProfiling(title);
#else
  profile = v8::CpuProfiler::StopProfiling(title);
#endif

  info.GetReturnValue().Set(Profile::New(profile));
}

NAN_METHOD(CpuProfiler::SetSamplingInterval) {
#if (NODE_MODULE_VERSION > 0x0039)
  current_cpuprofiler->SetSamplingInterval(Nan::To<uint32_t>(info[0]).ToChecked());
#elif (NODE_MODULE_VERSION > 0x000B)
  v8::Isolate::GetCurrent()->GetCpuProfiler()->SetSamplingInterval(Nan::To<uint32_t>(info[0]).ToChecked());
#endif
}
} //namespace nodex
