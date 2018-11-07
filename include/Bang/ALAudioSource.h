#ifndef ALAUDIOSOURCE_H
#define ALAUDIOSOURCE_H

#include <AL/al.h>
#include <vector>

#include "Bang/Array.tcc"
#include "Bang/AudioParams.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"

namespace Bang
{
class IEventsDestroy;

class ALAudioSource : public virtual EventEmitter<IEventsDestroy>
{
public:
    enum State
    {
        PLAYING = AL_PLAYING,
        PAUSED = AL_PAUSED,
        STOPPED = AL_STOPPED
    };

    ALAudioSource();
    virtual ~ALAudioSource() override;

    void Play();
    void Pause();
    void Stop();

    void SetVolume(float volume);
    void SetPitch(float pitch);
    void SetRange(float range);
    void SetLooping(bool looping);
    void SetPosition(const Vector3 &position);
    void SetParams(const AudioParams &audioParams);
    void SetALBufferId(ALuint bufferId);
    void UpdateALProperties() const;

    bool IsPlaying() const;
    bool IsPaused() const;
    bool IsStopped() const;
    State GetState() const;
    float GetVolume() const;
    float GetPitch() const;
    float GetRange() const;
    ALuint GetALSourceId() const;
    const Vector3 &GetPosition() const;
    const AudioParams &GetParams();
    bool GetLooping() const;

private:
    uint m_bufferId = 0;
    ALuint m_alSourceId = 0;
    AudioParams m_audioParams;
    bool m_autoDelete = false;

    friend class AudioManager;
    friend class AudioPlayerRunnable;
};
}

#endif  // ALAUDIOSOURCE_H
