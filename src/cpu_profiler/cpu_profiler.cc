#include "cpu_profiler.h"

#include "cpu_profile.h"
#include "environment_data.h"
#include "v8-inner.h"

namespace nodex {
using v8::Array;
using v8::CpuProfile;
using v8::Local;
using v8::Object;
using v8::String;

// class CpuProfiler
void CpuProfiler::Initialize(Local<Object> target) {
  HandleScope scope(target->GetIsolate());

  Local<Object> cpuProfiler = Nan::New<Object>();
  Local<Array> profiles = Nan::New<Array>();

  Nan::SetMethod(cpuProfiler, "startProfiling", CpuProfiler::StartProfiling);
  Nan::SetMethod(cpuProfiler, "stopProfiling", CpuProfiler::StopProfiling);
  Nan::SetMethod(cpuProfiler, "setSamplingInterval",
                 CpuProfiler::SetSamplingInterval);
  Nan::SetMethod(cpuProfiler, "setGenerateType", CpuProfiler::SetGenerateType);
  Nan::Set(cpuProfiler, Nan::New<String>("profiles").ToLocalChecked(),
           profiles);

  per_thread::profiles.Reset(profiles);
  Nan::Set(target, Nan::New<String>("cpu").ToLocalChecked(), cpuProfiler);
}

NAN_METHOD(CpuProfiler::SetGenerateType) {
  EnvironmentData* env_data = EnvironmentData::GetCurrent(info);
  if (env_data->cpu_profiler() == nullptr) {
    env_data->cpu_profiler() = InnerCpuProfiler::Create(env_data->isolate());
  }

  {
    HandleScope scope(env_data->isolate());
    env_data->cpu_profiler()->generate_type() =
        Nan::To<uint32_t>(info[0]).ToChecked();
  }
}

NAN_METHOD(CpuProfiler::SetSamplingInterval) {
  EnvironmentData* env_data = EnvironmentData::GetCurrent(info);
  if (env_data->cpu_profiler() == nullptr) {
    env_data->cpu_profiler() = InnerCpuProfiler::Create(env_data->isolate());
  }

  {
    HandleScope scope(env_data->isolate());
    env_data->cpu_profiler()->SetSamplingInterval(
        Nan::To<uint32_t>(info[0]).ToChecked());
  }
}

NAN_METHOD(CpuProfiler::StartProfiling) {
  EnvironmentData* env_data = EnvironmentData::GetCurrent(info);
  if (env_data->cpu_profiler() == nullptr) {
    env_data->cpu_profiler() = InnerCpuProfiler::Create(env_data->isolate());
  }

  {
    HandleScope scope(env_data->isolate());
    Local<String> title = Nan::To<String>(info[0]).ToLocalChecked();

#if (NODE_MODULE_VERSION > 0x000B)
    bool recsamples = Nan::To<bool>(info[1]).ToChecked();
    env_data->cpu_profiler()->StartProfiling(title, recsamples);
#else
    env_data->cpu_profiler()->StartProfiling(title);
#endif
  }
}

NAN_METHOD(CpuProfiler::StopProfiling) {
  EnvironmentData* env_data = EnvironmentData::GetCurrent(info);
  if (env_data->cpu_profiler() == nullptr) {
    env_data->cpu_profiler() = InnerCpuProfiler::Create(env_data->isolate());
  }

  {
    HandleScope scope(env_data->isolate());
    Local<String> title = Nan::EmptyString();
    if (info.Length()) {
      if (info[0]->IsString()) {
        title = Nan::To<String>(info[0]).ToLocalChecked();
      } else if (!info[0]->IsUndefined()) {
        return Nan::ThrowTypeError("Wrong argument [0] type (wait String)");
      }
    }
    CpuProfile* profile = env_data->cpu_profiler()->StopProfiling(title);
    Profile format(env_data->isolate());
    Local<Object> result =
        format.New(profile, env_data->cpu_profiler()->generate_type());
    env_data->cpu_profiler()->CheckProfile(profile);
    info.GetReturnValue().Set(result);
  }
}

// class InnerCpuProfiler
void InnerCpuProfiler::CheckProfile(v8::CpuProfile* profile) {
#if (NODE_MODULE_VERSION > 0x0039)
  profile->Delete();
  --this->started_profiles_count();
  if (this->started_profiles_count() == 0) {
    this->profiler()->Dispose();
    this->profiler() = nullptr;
  }
#endif
}

InnerCpuProfiler* InnerCpuProfiler::Create(v8::Isolate* isolate) {
  return new InnerCpuProfiler(isolate);
}

void InnerCpuProfiler::SetGenerateType(int type) {
  this->generate_type() = type;
}

void InnerCpuProfiler::SetSamplingInterval(uint32_t interval) {
#if (NODE_MODULE_VERSION > 0x0039)
  this->sampling_interval() = interval;
#elif (NODE_MODULE_VERSION > 0x000B)
  this->isolate()->GetCpuProfiler()->SetSamplingInterval(interval);
#endif
}

void InnerCpuProfiler::StartProfiling(v8::Local<v8::String> title,
                                      bool recsamples) {
#if (NODE_MODULE_VERSION > 0x0039)
  if (this->profiler() == nullptr) {
    this->profiler() = v8::CpuProfiler::New(v8::Isolate::GetCurrent());
  }
  this->started_profiles_count()++;
  if (this->sampling_interval() != 0) {
    this->profiler()->SetSamplingInterval(this->sampling_interval());
  }
  this->profiler()->StartProfiling(title, recsamples);
#elif (NODE_MODULE_VERSION > 0x000B)
  this->isolate()->GetCpuProfiler()->StartProfiling(title, recsamples);
#else
  v8::CpuProfiler::StartProfiling(title);
#endif
}

v8::CpuProfile* InnerCpuProfiler::StopProfiling(v8::Local<v8::String> title) {
  CpuProfile* profile = nullptr;
#if (NODE_MODULE_VERSION > 0x0039)
  if (this->profiler() == nullptr) {
    return nullptr;
  }
  profile = this->profiler()->StopProfiling(title);
#elif (NODE_MODULE_VERSION > 0x000B)
  profile = this->isolate()->GetCpuProfiler()->StopProfiling(title);
#else
  profile = v8::CpuProfiler::StopProfiling(title);
#endif
  return profile;
}

}  // namespace nodex
