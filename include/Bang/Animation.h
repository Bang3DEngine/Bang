#ifndef ANIMATION_H
#define ANIMATION_H

#include <utility>

#include "Bang/Map.h"
#include "Bang/Array.h"
#include "Bang/Asset.h"
#include "Bang/String.h"
#include "Bang/Matrix4.h"

NAMESPACE_BANG_BEGIN

class Animation : public Asset
{
    ASSET(Animation);

public:
    struct KeyFrame
    {
        float time;
        Map<String, Matrix4> boneNameToTransformation;
    };

    void AddKeyFrame(const Animation::KeyFrame &keyFrame);

    const Array<KeyFrame>& GetKeyFrames() const;

    // Resource
    void Import(const Path &animationFilepath) override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    Animation();
    virtual ~Animation();

    Array<KeyFrame> m_keyFrames;
};

NAMESPACE_BANG_END

#endif // ANIMATION_H

