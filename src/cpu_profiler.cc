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
int m_startedProfilesCount = 0;
v8::CpuProfiler* m_profiler = nullptr;
uint32_t samplingInterval = 0;
#endif

int generateType = 0;

void CpuProfiler::Initialize (Local<Object> target) {
  Nan::HandleScope scope;

  Local<Object> cpuProfiler = Nan::New<Object>();
  Local<Array> profiles = Nan::New<Array>();

  Nan::SetMethod(cpuProfiler, "startProfiling", CpuProfiler::StartProfiling);
  Nan::SetMethod(cpuProfiler, "stopProfiling", CpuProfiler::StopProfiling);
  Nan::SetMethod(cpuProfiler, "setSamplingInterval", CpuProfiler::SetSamplingInterval);
  Nan::SetMethod(cpuProfiler, "setGenerateType", CpuProfiler::SetGenerateType);
  Nan::Set(cpuProfiler, Nan::New<String>("profiles").ToLocalChecked(), profiles);

  Profile::profiles.Reset(profiles);
  Nan::Set(target, Nan::New<String>("cpu").ToLocalChecked(), cpuProfiler);
}

NAN_METHOD(CpuProfiler::SetGenerateType) {
  generateType = Nan::To<uint32_t>(info[0]).ToChecked();
}

NAN_METHOD(CpuProfiler::StartProfiling) {
  Local<String> title = Nan::To<String>(info[0]).ToLocalChecked();

#if (NODE_MODULE_VERSION > 0x0039)
  bool recsamples = Nan::To<bool>(info[1]).ToChecked();
  if (!m_startedProfilesCount) {
      m_profiler = v8::CpuProfiler::New(v8::Isolate::GetCurrent());
  }
  ++m_startedProfilesCount;
  if (samplingInterval){
      m_profiler->SetSamplingInterval(samplingInterval);
  }
  m_profiler->StartProfiling(title, recsamples);
#elif (NODE_MODULE_VERSION > 0x000B)
  bool recsamples = Nan::To<bool>(info[1]).ToChecked();
  v8::Isolate::GetCurrent()->GetCpuProfiler()->StartProfiling(title, recsamples);
#else
  v8::CpuProfiler::StartProfiling(title);
#endif
}

NAN_METHOD(CpuProfiler::StopProfiling) {
  CpuProfile* profile;
  Local<String> title = Nan::EmptyString();
  if (info.Length()) {
    if (info[0]->IsString()) {
      title = Nan::To<String>(info[0]).ToLocalChecked();
    } else if (!info[0]->IsUndefined()) {
      return Nan::ThrowTypeError("Wrong argument [0] type (wait String)");
    }
  }

#if (NODE_MODULE_VERSION > 0x0039)
  profile = m_profiler->StopProfiling(title);
#elif (NODE_MODULE_VERSION > 0x000B)
  profile = v8::Isolate::GetCurrent()->GetCpuProfiler()->StopProfiling(title);
#else
  profile = v8::CpuProfiler::StopProfiling(title);
#endif

  Local<Object> result = Profile::New(profile, generateType);
  info.GetReturnValue().Set(result);

#if (NODE_MODULE_VERSION > 0x0039)
  profile->Delete();
  --m_startedProfilesCount;
  if (!m_startedProfilesCount) {
    m_profiler->Dispose();
    m_profiler = nullptr;
  }
#endif
}

NAN_METHOD(CpuProfiler::SetSamplingInterval) {
#if (NODE_MODULE_VERSION > 0x0039)
  samplingInterval = Nan::To<uint32_t>(info[0]).ToChecked();
#elif (NODE_MODULE_VERSION > 0x000B)
  v8::Isolate::GetCurrent()->GetCpuProfiler()->SetSamplingInterval(Nan::To<uint32_t>(info[0]).ToChecked());
#endif
}
} //namespace nodex
