#include "Bang/AudioSource.h"

#include <AL/al.h>
#include <vector>

#include "Bang/Array.tcc"
#include "Bang/AudioClip.h"
#include "Bang/AudioManager.h"
#include "Bang/EventListener.tcc"
#include "Bang/Extensions.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/ICloneable.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "Bang/Transform.h"
#include "Bang/TypeTraits.h"

using namespace Bang;

AudioSource::AudioSource()
{
    CONSTRUCT_CLASS_ID(AudioSource)
}

AudioSource::~AudioSource()
{
    Stop();
}

void AudioSource::OnStart()
{
    Component::OnStart();
    if (GetPlayOnStart() && !AudioManager::GetPlayOnStartBlocked())
    {
        Play();
    }
}

void AudioSource::OnUpdate()
{
    Component::OnUpdate();

    if (GetAudioClip())
    {
        if (m_currentAudioClipALBufferId != GetAudioClip()->GetALBufferId())
        {
            SetAudioClip(GetAudioClip());
        }
    }

    Transform *tr = GetGameObject()->GetTransform();
    if (tr)
    {
        ALAudioSource::SetPosition(tr->GetPosition());
    }
}

void AudioSource::SetAudioClip(AudioClip *audioClip)
{
    p_audioClip.Set(audioClip);
    if (GetAudioClip())
    {
        SetALBufferId(audioClip->GetALBufferId());
        m_currentAudioClipALBufferId = audioClip->GetALBufferId();
    }
    else
    {
        SetALBufferId(0);
        m_currentAudioClipALBufferId = 0;
    }
}

void AudioSource::SetPlayOnStart(bool playOnStart)
{
    m_playOnStart = playOnStart;
}

void AudioSource::Play()
{
    Play(0.0f);
}

void AudioSource::Play(float delay)
{
    AudioManager::Play(GetAudioClip(), this, delay);
}

bool AudioSource::GetPlayOnStart() const
{
    return m_playOnStart;
}

AudioClip *AudioSource::GetAudioClip() const
{
    return p_audioClip.Get();
}

float AudioSource::GetPlayProgress() const
{
    float secondsOffset;
    alGetSourcef(GetALSourceId(), AL_SEC_OFFSET, &secondsOffset);
    alGetSourcef(GetALSourceId(), AL_SEC_OFFSET, &secondsOffset);
    return secondsOffset / GetAudioClip()->GetLength();
}

void AudioSource::Reflect()
{
    Component::Reflect();

    ReflectVar<float>("Volume",
                      [this](float v) { SetVolume(v); },
                      [this]() -> float { return GetVolume(); },
                      BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    ReflectVar<float>("Pitch",
                      [this](float p) { SetPitch(p); },
                      [this]() -> float { return GetPitch(); },
                      BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    ReflectVar<float>("Range",
                      [this](float r) { SetRange(r); },
                      [this]() -> float { return GetRange(); },
                      BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    ReflectVar<bool>("Looping",
                     [this](bool looping) { SetLooping(looping); },
                     [this]() -> bool { return GetLooping(); },
                     BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    ReflectVar<bool>("PlayOnStart",
                     [this](bool playOnStart) { SetPlayOnStart(playOnStart); },
                     [this]() -> bool { return GetPlayOnStart(); },
                     BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_RESOURCE(
        AudioSource,
        "AudioClip",
        SetAudioClip,
        GetAudioClip,
        AudioClip,
        BANG_REFLECT_HINT_EXTENSIONS(Extensions::GetAudioClipExtensions()));
}
