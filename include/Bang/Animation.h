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
    template <class T>
    struct KeyFrame
    {
        float time;
        T value;
    };

    void AddPositionKeyFrame(const Animation::KeyFrame<Vector3> &keyFrame);
    void AddRotationKeyFrame(const Animation::KeyFrame<Quaternion> &keyFrame);
    void AddScaleKeyFrame(const Animation::KeyFrame<Vector3> &keyFrame);
    void SetFramesPerSecond(float framesPerSecond);
    void SetDuration(float durationInSeconds);

    float GetDuration() const;
    float GetFramesPerSecond() const;
    const Array<KeyFrame<Vector3> > &GetPositionKeyFrames() const;
    const Array<KeyFrame<Quaternion>>& GetRotationKeyFrames() const;
    const Array<KeyFrame<Vector3>>& GetScaleKeyFrames() const;

    // Resource
    void Import(const Path &animationFilepath) override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    Animation();
    virtual ~Animation();

    float m_durationSeconds = 0.0f;
    float m_framesPerSecond = 0.0f;

    Array<KeyFrame<Vector3>> m_positionKeyFrames;
    Array<KeyFrame<Quaternion>> m_rotationKeyFrames;
    Array<KeyFrame<Vector3>> m_scaleKeyFrames;
};

NAMESPACE_BANG_END

#endif // ANIMATION_H

