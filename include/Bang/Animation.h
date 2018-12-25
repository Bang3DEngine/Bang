#ifndef ANIMATION_H
#define ANIMATION_H

#include <functional>

#include "Bang/Array.h"
#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/Map.h"
#include "Bang/MetaNode.h"
#include "Bang/Quaternion.h"
#include "Bang/String.h"
#include "Bang/Transformation.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Path;
class Time;

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
    void SetDurationInFrames(float durationInFrames);
    void SetFramesPerSecond(float framesPerSecond);
    void SetWrapMode(AnimationWrapMode wrapMode);
    void SetSpeed(float speed);

    float GetSpeed() const;
    float GetFramesPerSecond() const;
    float GetDurationInFrames() const;
    float GetDurationInSeconds() const;
    AnimationWrapMode GetWrapMode() const;

    const Array<Animation::KeyFrame<Vector3>> &GetPositionKeyFrames(
        const String &boneName) const;

    const Array<Animation::KeyFrame<Quaternion>> &GetRotationKeyFrames(
        const String &boneName) const;

    const Array<Animation::KeyFrame<Vector3>> &GetScaleKeyFrames(
        const String &boneName) const;

    const Map<String, Array<Animation::KeyFrame<Vector3>>>
        &GetBoneNameToPositionKeyFrames() const;

    const Map<String, Array<Animation::KeyFrame<Quaternion>>>
        &GetBoneNameToRotationKeyFrames() const;

    const Map<String, Array<Animation::KeyFrame<Vector3>>>
        &GetBoneNameToScaleKeyFrames() const;

    static float WrapTime(float time,
                          float totalDuration,
                          AnimationWrapMode animationWrapMode);

    // Asset
    void Import(const Path &animationFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    static Map<String, Matrix4> GetBoneMatrices(
        const Map<String, Transformation> &bonesTransformations);

    static Map<String, Transformation> GetBoneAnimationTransformations(
        const Animation *animation,
        Time animationTime);

    static Map<String, Transformation> GetInterpolatedBoneTransformations(
        const Map<String, Transformation> &prevTransformations,
        const Map<String, Transformation> &nextTransformations,
        float weight);

private:
    Animation();
    virtual ~Animation() override;

    float m_speed = 1.0f;
    float m_durationInFrames = 0.0f;
    float m_framesPerSecond = 0.0f;
    AnimationWrapMode m_wrapMode = AnimationWrapMode::CLAMP;

    Map<String, Array<KeyFrame<Vector3>>> m_boneNameToPositionKeyFrames;
    Map<String, Array<KeyFrame<Quaternion>>> m_boneNameToRotationKeyFrames;
    Map<String, Array<KeyFrame<Vector3>>> m_boneNameToScaleKeyFrames;
};
}

#endif  // ANIMATION_H
