#include "cpu_profiler.h"

#include "cpu_profile.h"
#include "environment_data.h"
#include "utils-inl.h"
#include "v8-inner.h"

namespace nodex {
using v8::Array;
using v8::CpuProfile;
using v8::Local;
using v8::Object;
using v8::String;

namespace per_thread {
extern thread_local Nan::Persistent<Object> profiles;
}

CpuProfiler::CpuProfiler() {}
CpuProfiler::~CpuProfiler() {}

void CpuProfiler::Initialize(Local<Object> target) {
  HandleScope scope(target->GetIsolate());

  Local<Object> cpuProfiler = Nan::New<Object>();
  Local<Array> profiles = Nan::New<Array>();

  Nan::SetMethod(cpuProfiler, "startProfiling", CpuProfiler::StartProfiling);
  Nan::SetMethod(cpuProfiler, "stopProfiling", CpuProfiler::StopProfiling);
  Nan::SetMethod(cpuProfiler, "setSamplingInterval",
                 CpuProfiler::SetSamplingInterval);
  Nan::SetMethod(cpuProfiler, "collectSample", CpuProfiler::CollectSample);
  Nan::SetMethod(cpuProfiler, "setGenerateType", CpuProfiler::SetGenerateType);
  Nan::SetMethod(cpuProfiler, "setProfilesCleanupLimit",
                 CpuProfiler::SetProfilesCleanupLimit);
  Nan::Set(cpuProfiler, Nan::New<String>("profiles").ToLocalChecked(),
           profiles);

  per_thread::profiles.Reset(profiles);
  Nan::Set(target, Nan::New<String>("cpu").ToLocalChecked(), cpuProfiler);
}

CPU_PROFILER_METHODS(NAN_CPU_PROFILER_METHOD);

INNER_METHOD(InnerCpuProfiler::SetGenerateType) {
  HandleScope scope(this->isolate());
  generateType = Nan::To<uint32_t>(info[0]).ToChecked();
}

INNER_METHOD(InnerCpuProfiler::SetProfilesCleanupLimit) {
#if (NODE_MODULE_VERSION > 0x0039)
  HandleScope scope(this->isolate());
  profiles_clean_limit_ = Nan::To<uint32_t>(info[0]).ToChecked();
#endif
}

INNER_METHOD(InnerCpuProfiler::StartProfiling) {
  HandleScope scope(this->isolate());

  Local<String> title = Nan::To<String>(info[0]).ToLocalChecked();

#if (NODE_MODULE_VERSION > NODE_8_0_MODULE_VERSION)
  bool recsamples = Nan::To<bool>(info[1]).ToChecked();
  if (!cpu_profiler_) {
#if (NODE_MODULE_VERSION > NODE_11_0_MODULE_VERSION)
    EnvironmentData* env_data = EnvironmentData::GetCurrent(this->isolate());
    if (env_data->greater_than_12_15_0()) {
      int logging_mode_check = 0;
      if (info[2]->IsNumber()) {
        logging_mode_check = Nan::To<uint32_t>(info[2]).ToChecked();
      }
      v8::CpuProfilingLoggingMode logging_mode =
          logging_mode_check == 1 ? v8::kEagerLogging : v8::kLazyLogging;
      logger(this->isolate(), "cpuprofiler using loggingMode: %d\n",
             logging_mode);
      cpu_profiler_ = v8::CpuProfiler::New(v8::Isolate::GetCurrent(),
                                           v8::kDebugNaming, logging_mode);
    } else {
      cpu_profiler_ = v8::CpuProfiler::New(v8::Isolate::GetCurrent());
    }
#else
    cpu_profiler_ = v8::CpuProfiler::New(v8::Isolate::GetCurrent());
#endif
  }
  ++started_profiles_count_;
  ++profiles_since_last_cleanup_;
  if (sampling_interval_) {
    cpu_profiler_->SetSamplingInterval(sampling_interval_);
  }
  cpu_profiler_->StartProfiling(title, recsamples);
#elif (NODE_MODULE_VERSION > 0x000B)
  bool recsamples = Nan::To<bool>(info[1]).ToChecked();
  v8::Isolate::GetCurrent()->GetCpuProfiler()->StartProfiling(title,
                                                              recsamples);
#else
  v8::CpuProfiler::StartProfiling(title);
#endif
}

INNER_METHOD(InnerCpuProfiler::StopProfiling) {
  HandleScope scope(this->isolate());

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
  profile = cpu_profiler_->StopProfiling(title);
#elif (NODE_MODULE_VERSION > 0x000B)
  profile = v8::Isolate::GetCurrent()->GetCpuProfiler()->StopProfiling(title);
#else
  profile = v8::CpuProfiler::StopProfiling(title);
#endif

  Local<Object> result = Profile::New(this->isolate(), profile, generateType);
  info.GetReturnValue().Set(result);

#if (NODE_MODULE_VERSION > 0x0039)
  profile->Delete();
  --started_profiles_count_;
  if (!started_profiles_count_ &&
      profiles_since_last_cleanup_ > profiles_clean_limit_) {
    logger(this->isolate(), "clear cpuprofiler: %d\n", profiles_clean_limit_);
    cpu_profiler_->Dispose();
    cpu_profiler_ = nullptr;
    profiles_since_last_cleanup_ = 0;
  } else {
    logger(this->isolate(), "not clear cpuprofiler: %d, %d\n",
           started_profiles_count_, profiles_clean_limit_);
  }
#endif
}

INNER_METHOD(InnerCpuProfiler::SetSamplingInterval) {
  HandleScope scope(this->isolate());

#if (NODE_MODULE_VERSION > 0x0039)
  sampling_interval_ = Nan::To<uint32_t>(info[0]).ToChecked();
#elif (NODE_MODULE_VERSION > 0x000B)
  v8::Isolate::GetCurrent()->GetCpuProfiler()->SetSamplingInterval(
      Nan::To<uint32_t>(info[0]).ToChecked());
#endif
}

INNER_METHOD(InnerCpuProfiler::CollectSample) {
  HandleScope scope(this->isolate());

#if (NODE_MODULE_VERSION > 0x003B)
  v8::CpuProfiler::CollectSample(this->isolate());
#elif (NODE_MODULE_VERSION >= 0x0030)
  v8::Isolate::GetCurrent()->GetCpuProfiler()->CollectSample();
#endif
}
}  // namespace nodex