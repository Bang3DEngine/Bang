#include "Bang/AudioSource.h"

#include <AL/al.h>
#include <vector>

#include "Bang/Array.tcc"
#include "Bang/AudioClip.h"
#include "Bang/AudioManager.h"
#include "Bang/EventListener.tcc"
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
    if(GetPlayOnStart() && !AudioManager::GetPlayOnStartBlocked())
    {
        Play();
    }
}

void AudioSource::OnUpdate()
{
    Component::OnUpdate();

    if(GetAudioClip())
    {
        if(m_currentAudioClipALBufferId != GetAudioClip()->GetALBufferId())
        {
            SetAudioClip(GetAudioClip());
        }
    }

    Transform *tr = GetGameObject()->GetTransform();
    if(tr)
    {
        ALAudioSource::SetPosition(tr->GetPosition());
    }
}

void AudioSource::SetAudioClip(AudioClip *audioClip)
{
    p_audioClip.Set(audioClip);
    if(GetAudioClip())
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

void AudioSource::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    AudioSource *as = Cast<AudioSource *>(clone);
    as->SetAudioClip(GetAudioClip());
    as->SetVolume(GetVolume());
    as->SetPitch(GetPitch());
    as->SetRange(GetRange());
    as->SetLooping(GetLooping());
    as->SetPlayOnStart(GetPlayOnStart());
}

void AudioSource::ImportMeta(const MetaNode &meta)
{
    Component::ImportMeta(meta);

    if(meta.Contains("AudioClip"))
    {
        RH<AudioClip> audioClip =
            Resources::Load<AudioClip>(meta.Get<GUID>("AudioClip"));
        SetAudioClip(audioClip.Get());
    }

    if(meta.Contains("Volume"))
    {
        SetVolume(meta.Get<float>("Volume"));
    }

    if(meta.Contains("Pitch"))
    {
        SetPitch(meta.Get<float>("Pitch"));
    }

    if(meta.Contains("Range"))
    {
        SetRange(meta.Get<float>("Range"));
    }

    if(meta.Contains("Looping"))
    {
        SetLooping(meta.Get<bool>("Looping"));
    }

    if(meta.Contains("PlayOnStart"))
    {
        SetPlayOnStart(meta.Get<bool>("PlayOnStart"));
    }
}

void AudioSource::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    AudioClip *audioClip = GetAudioClip();
    GUID audioClipGUID = audioClip ? audioClip->GetGUID() : GUID::Empty();
    metaNode->Set("AudioClip", audioClipGUID);
    metaNode->Set("Volume", GetVolume());
    metaNode->Set("Pitch", GetPitch());
    metaNode->Set("Range", GetRange());
    metaNode->Set("Looping", GetLooping());
    metaNode->Set("PlayOnStart", GetPlayOnStart());
}
