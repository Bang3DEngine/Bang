#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include "Bang/ALAudioSource.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"

namespace Bang
{
class AudioClip;
class ICloneable;

class AudioSource : public Component, public ALAudioSource
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(AudioSource)

public:
    virtual ~AudioSource() override;

    void SetAudioClip(AudioClip *audioClip);
    void SetPlayOnStart(bool playOnStart);

    void Play();  // Hides ALAudioSource::Play()
    void Play(float delay);

    bool GetPlayOnStart() const;
    float GetPlayProgress() const;
    AudioClip *GetAudioClip() const;

    // Component
    void OnStart() override;

    // Serializable
    void Reflect() override;

protected:
    AudioSource();

    // Component
    virtual void OnUpdate() override;

private:
    RH<AudioClip> p_audioClip;
    bool m_playOnStart = true;
    uint m_currentAudioClipALBufferId = 0;

    friend class AudioClip;
    friend class AudioManager;
};
}

#endif  // AUDIOSOURCE_H
