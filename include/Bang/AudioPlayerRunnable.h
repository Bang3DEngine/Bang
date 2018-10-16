#ifndef AUDIOPLAYERRUNNABLE_H
#define AUDIOPLAYERRUNNABLE_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/Thread.h"

NAMESPACE_BANG_BEGIN

FORWARD class ALAudioSource;
FORWARD class AudioClip;
FORWARD class IEventsDestroy;

class AudioPlayerRunnable : public ThreadRunnable,
                            public EventEmitter<IEventsDestroy>
{
public:
    AudioPlayerRunnable(AudioClip *clip,
                        ALAudioSource *alAudioSource,
                        float delayInSeconds = 0.0f);
    virtual ~AudioPlayerRunnable();

    void Resume();
    void Pause();
    void Stop();

    AudioClip* GetAudioClip() const;
    ALAudioSource* GetALAudioSource() const;

    void Run() override;

private:
    bool m_forceExit = false;
    AudioClip *p_audioClip = nullptr;
    ALAudioSource *p_alAudioSource = 0;
    float m_delayInSeconds = 0.0f;
};

NAMESPACE_BANG_END

#endif // AUDIOPLAYERRUNNABLE_H
