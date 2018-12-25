#ifndef AUDIOCLIP_H
#define AUDIOCLIP_H

#include <AL/al.h>

#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/MetaNode.h"
#include "Bang/Path.h"
#include "Bang/String.h"

namespace Bang
{
class AudioClip : public Asset
{
    ASSET(AudioClip)

public:
    int GetChannels() const;
    int GetBufferSize() const;
    int GetBitDepth() const;
    int GetFrequency() const;
    float GetLength() const;
    bool IsLoaded() const;
    const Path &GetSoundFilepath() const;

    // Asset
    void Import(const Path &soundFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    ALuint m_alBufferId = 0;
    Path m_soundFilepath;

    AudioClip();
    virtual ~AudioClip() override;

    void FreeBuffer();
    ALuint GetALBufferId() const;

    friend class AudioSource;
    friend class AudioManager;
    friend class AudioPlayerRunnable;
};
}

#endif  // AUDIOCLIP_H
