#ifndef AUDIOLISTENER_H
#define AUDIOLISTENER_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

class AudioListener : public Component
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(AudioListener)

public:
    AudioListener();
    virtual ~AudioListener() override;

    // Component
    virtual void OnUpdate() override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:

   void UpdateALProperties() const;
};

NAMESPACE_BANG_END

#endif // AUDIOLISTENER_H
