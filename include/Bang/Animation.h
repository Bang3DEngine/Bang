#ifndef ANIMATION_H
#define ANIMATION_H

#include <utility>

#include "Bang/Map.h"
#include "Bang/Array.h"
#include "Bang/Asset.h"
#include "Bang/String.h"
#include "Bang/Matrix4.h"

NAMESPACE_BANG_BEGIN

enum class AnimationWrapMode
{
    CLAMP,
    REPEAT,
    PING_PONG
};

class Animation : public Asset
{
    ASSET(Animation);

public:
    template <class T>
    struct KeyFrame
    {
        float timeInFrames;
        T value;
    };

    void AddPositionKeyFrame(const String &boneName,
                             const Animation::KeyFrame<Vector3> &keyFrame);
    void AddRotationKeyFrame(const String &boneName,
                             const Animation::KeyFrame<Quaternion> &keyFrame);
    void AddScaleKeyFrame(const String &boneName,
                          const Animation::KeyFrame<Vector3> &keyFrame);
    void SetDurationInFrames(float durationInSeconds);
    void SetFramesPerSecond(float framesPerSecond);
    void SetWrapMode(AnimationWrapMode wrapMode);
    void SetSpeed(float speed);

    float GetSpeed() const;
    float GetFramesPerSecond() const;
    float GetDurationInFrames() const;
    AnimationWrapMode GetWrapMode() const;
    Map<String, Matrix4> GetBoneAnimationMatricesForSecond(float second) const;

    const Array< Animation::KeyFrame<Vector3> > &
    GetPositionKeyFrames(const String &boneName) const;

    const Array< Animation::KeyFrame<Quaternion> > &
    GetRotationKeyFrames(const String &boneName) const;

    const Array< Animation::KeyFrame<Vector3> > &
    GetScaleKeyFrames(const String &boneName) const;

    const Map< String, Array< Animation::KeyFrame<Vector3> > > &
    GetBoneNameToPositionKeyFrames() const;

    const Map< String, Array< Animation::KeyFrame<Quaternion> > > &
    GetBoneNameToRotationKeyFrames() const;

    const Map< String, Array< Animation::KeyFrame<Vector3> > > &
    GetBoneNameToScaleKeyFrames() const;

    // Resource
    void Import(const Path &animationFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Animation();
    virtual ~Animation();

    float m_speed = 1.0f;
    float m_durationInFrames = 0.0f;
    float m_framesPerSecond  = 0.0f;
    AnimationWrapMode m_wrapMode = AnimationWrapMode::CLAMP;

    Map<String, Array<KeyFrame<Vector3>>>    m_boneNameToPositionKeyFrames;
    Map<String, Array<KeyFrame<Quaternion>>> m_boneNameToRotationKeyFrames;
    Map<String, Array<KeyFrame<Vector3>>>    m_boneNameToScaleKeyFrames;
};

NAMESPACE_BANG_END

#endif // ANIMATION_H

