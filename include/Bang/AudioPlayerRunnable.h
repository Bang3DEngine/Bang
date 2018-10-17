#ifndef AUDIOPLAYERRUNNABLE_H
#define AUDIOPLAYERRUNNABLE_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/Thread.h"

namespace Bang
{
class ALAudioSource;
class AudioClip;
class IEventsDestroy;

class AudioPlayerRunnable : public ThreadRunnable,
                            public EventEmitter<IEventsDestroy>
{
public:
    AudioPlayerRunnable(AudioClip *clip,
                        ALAudioSource *alAudioSource,
                        float delayInSeconds = 0.0f);
    virtual ~AudioPlayerRunnable() override;

    void Resume();
    void Pause();
    void Stop();

    AudioClip *GetAudioClip() const;
    ALAudioSource *GetALAudioSource() const;

    void Run() override;

private:
    bool m_forceExit = false;
    AudioClip *p_audioClip = nullptr;
    ALAudioSource *p_alAudioSource = 0;
    float m_delayInSeconds = 0.0f;
};
}

#endif  // AUDIOPLAYERRUNNABLE_H
