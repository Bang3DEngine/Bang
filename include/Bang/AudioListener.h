#ifndef AUDIOLISTENER_H
#define AUDIOLISTENER_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"

namespace Bang
{
class AudioListener : public Component
{
    COMPONENT(AudioListener)

public:
    AudioListener();
    virtual ~AudioListener() override;

    // Component
    virtual void OnUpdate() override;

private:
    void UpdateALProperties() const;
};
}

#endif  // AUDIOLISTENER_H
