#include "Bang/AudioPlayerRunnable.h"

#include "Bang/ALAudioSource.h"
#include "Bang/AudioClip.h"
#include "Bang/AudioManager.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/IEventsDestroy.h"

using namespace Bang;

AudioPlayerRunnable::AudioPlayerRunnable(AudioClip *clip,
                                         ALAudioSource *alAudioSource,
                                         float delayInSeconds)
{
    p_audioClip = clip;
    p_alAudioSource = alAudioSource;
    m_delayInSeconds = delayInSeconds;

    GetALAudioSource()->EventEmitter<IEventsDestroy>::RegisterListener(this);
    SetAutoDelete(true);
}

AudioPlayerRunnable::~AudioPlayerRunnable()
{
    AudioManager::GetInstance()->OnAudioFinishedPlaying(this);
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);

    if (GetALAudioSource() && GetALAudioSource()->m_autoDelete)
    {
        delete p_alAudioSource;
    }
}

void AudioPlayerRunnable::Resume()
{
    if (GetALAudioSource())
    {
        GetALAudioSource()->Play();
    }
}

void AudioPlayerRunnable::Pause()
{
    if (GetALAudioSource())
    {
        GetALAudioSource()->Pause();
    }
}

void AudioPlayerRunnable::Stop()
{
    m_forceExit = true;
    if (GetALAudioSource())
    {
        GetALAudioSource()->Stop();
    }
}

AudioClip *AudioPlayerRunnable::GetAudioClip() const
{
    return p_audioClip;
}

ALAudioSource *AudioPlayerRunnable::GetALAudioSource() const
{
    return p_alAudioSource;
}

void AudioPlayerRunnable::Run()
{
    if (!p_audioClip->IsLoaded())
    {
        return;
    }

    if (m_delayInSeconds > 0.0f)  // Wait delay
    {
        Thread::SleepCurrentThread(m_delayInSeconds);
    }

    if (GetALAudioSource())
    {
        GetALAudioSource()->Play();  // Play and wait until source is stopped
        do
        {
            Thread::SleepCurrentThread(0.3f);
        } while (!m_forceExit && !GetALAudioSource()->IsStopped());
    }
}

void AudioPlayerRunnable::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    ASSERT(object == GetALAudioSource());
    p_alAudioSource = nullptr;
    m_forceExit = true;
}
