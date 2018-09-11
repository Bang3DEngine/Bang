#include "Bang/Animation.h"

#include "Bang/MetaNode.h"

USING_NAMESPACE_BANG

Animation::Animation()
{
    SetWrapMode(AnimationWrapMode::REPEAT);
}

Animation::~Animation()
{
}

void Animation::AddPositionKeyFrame(const String &boneName,
                                    const Animation::KeyFrame<Vector3> &keyFrame)
{
    if (!GetBoneNameToPositionKeyFrames().ContainsKey(boneName))
    {
        m_boneNameToPositionKeyFrames.Add(boneName, {{}});
    }
    m_boneNameToPositionKeyFrames.Get(boneName).PushBack(keyFrame);
    PropagateResourceChanged();
}

void Animation::AddRotationKeyFrame(const String &boneName,
                                    const Animation::KeyFrame<Quaternion> &keyFrame)
{
    if (!GetBoneNameToRotationKeyFrames().ContainsKey(boneName))
    {
        m_boneNameToRotationKeyFrames.Add(boneName, {{}});
    }
    m_boneNameToRotationKeyFrames.Get(boneName).PushBack(keyFrame);
    PropagateResourceChanged();
}

void Animation::AddScaleKeyFrame(const String &boneName,
                                 const Animation::KeyFrame<Vector3> &keyFrame)
{
    if (!GetBoneNameToScaleKeyFrames().ContainsKey(boneName))
    {
        m_boneNameToScaleKeyFrames.Add(boneName, {{}});
    }
    m_boneNameToScaleKeyFrames.Get(boneName).PushBack(keyFrame);
    PropagateResourceChanged();
}

void Animation::SetDurationInFrames(float durationInSeconds)
{
    if (durationInSeconds != GetDurationInFrames())
    {
        m_durationInFrames = durationInSeconds;
        PropagateResourceChanged();
    }
}

void Animation::SetFramesPerSecond(float framesPerSecond)
{
    if (framesPerSecond != GetFramesPerSecond())
    {
        m_framesPerSecond = framesPerSecond;
        PropagateResourceChanged();
    }
}

void Animation::SetWrapMode(AnimationWrapMode wrapMode)
{
    if (wrapMode != GetWrapMode())
    {
        m_wrapMode = wrapMode;
        PropagateResourceChanged();
    }
}

void Animation::SetSpeed(float speed)
{
    if (speed != GetSpeed())
    {
        m_speed = speed;
        PropagateResourceChanged();
    }
}

float Animation::GetSpeed() const
{
    return m_speed;
}

float Animation::GetDurationInFrames() const
{
    return m_durationInFrames;
}

float Animation::GetFramesPerSecond() const
{
    return m_framesPerSecond;
}

AnimationWrapMode Animation::GetWrapMode() const
{
    return m_wrapMode;
}

template<class T>
Array<Animation::KeyFrame<T>>
GetConsecutiveKeyFrames(const Array<Animation::KeyFrame<T>> &keyFrames,
                         float timeInFrames)
{
    const uint numKF = keyFrames.Size();
    for (uint i = 0; i < numKF - 1; ++i)
    {
        const Animation::KeyFrame<T> &prevPosKF = keyFrames[i];
        const Animation::KeyFrame<T> &nextPosKF = keyFrames[i + 1];
        if (timeInFrames >= prevPosKF.timeInFrames &&
            timeInFrames <= nextPosKF.timeInFrames)
        {
            return {{prevPosKF, nextPosKF}};
        }
    }
    return {{}};
}

float WrapTime(float time,
               float totalDuration,
               AnimationWrapMode animationWrapMode)
{
    float wrappedTime = -1.0f;
    switch (animationWrapMode)
    {
        case AnimationWrapMode::CLAMP:
            wrappedTime = Math::Clamp(time, 0.0f, totalDuration);
        break;

        case AnimationWrapMode::REPEAT:
            wrappedTime = Math::FModAbs(time, totalDuration);
        break;

        case AnimationWrapMode::PING_PONG:
        {
            wrappedTime = Math::FModAbs(time, totalDuration);
            int parity = SCAST<int>(wrappedTime / totalDuration);
            if ((parity % 2) == 1)
            {
                wrappedTime = totalDuration - wrappedTime;
            }
        }
        break;
    }
    return wrappedTime;
}

Map<String, Matrix4> Animation::GetBoneAnimationMatricesForSecond(float timeSecs) const
{
    Map<String, Matrix4> bonesMatrices;
    if (GetDurationInFrames() <= 0.0f)
    {
        return bonesMatrices;
    }

    float timeInFrames = (timeSecs * GetFramesPerSecond());
    timeInFrames = WrapTime(timeInFrames,
                            GetDurationInFrames(),
                            GetWrapMode());

    Map<String, Matrix4> bonesPositionMatrices;
    for (const auto &it : GetBoneNameToPositionKeyFrames())
    {
        const String &boneName = it.first;
        const auto &posKeyFrames = it.second;

        bonesMatrices.Add(boneName, Matrix4::Identity);

        Array< KeyFrame<Vector3> > positionInterpKeyFrames =
                        GetConsecutiveKeyFrames(posKeyFrames, timeInFrames);

        if (positionInterpKeyFrames.Size() == 2)
        {
            Matrix4 boneMatrix;
            const KeyFrame<Vector3> &prevPosKF = positionInterpKeyFrames[0];
            const KeyFrame<Vector3> &nextPosKF = positionInterpKeyFrames[1];
            float timeBetweenPrevNext = (nextPosKF.timeInFrames -
                                         prevPosKF.timeInFrames);
            timeBetweenPrevNext = Math::Max(timeBetweenPrevNext, 0.0001f);

            float timePassedSincePrev = (timeInFrames - prevPosKF.timeInFrames);
            float interpFactor = (timePassedSincePrev / timeBetweenPrevNext);
            interpFactor = Math::Clamp(interpFactor, 0.0f, 1.0f);

            Vector3 position = Vector3::Lerp(prevPosKF.value,
                                             nextPosKF.value,
                                             interpFactor);
            boneMatrix = Matrix4::TranslateMatrix(position);
            bonesPositionMatrices.Add(boneName, boneMatrix);
        }
    }

    Map<String, Matrix4> bonesRotationMatrices;
    for (const auto &it : GetBoneNameToRotationKeyFrames())
    {
        const String &boneName = it.first;
        const auto &rotKeyFrames = it.second;

        bonesMatrices.Add(boneName, Matrix4::Identity);

        Array< KeyFrame<Quaternion> > rotationInterpKeyFrames =
                        GetConsecutiveKeyFrames(rotKeyFrames, timeInFrames);

        if (rotationInterpKeyFrames.Size() == 2)
        {
            Matrix4 boneMatrix;
            const KeyFrame<Quaternion> &prevRotKF = rotationInterpKeyFrames[0];
            const KeyFrame<Quaternion> &nextRotKF = rotationInterpKeyFrames[1];
            float timeBetweenPrevNext = (nextRotKF.timeInFrames -
                                         prevRotKF.timeInFrames);
            timeBetweenPrevNext = Math::Max(timeBetweenPrevNext, 0.0001f);

            float timePassedSincePrev = (timeInFrames - prevRotKF.timeInFrames);
            float interpFactor = (timePassedSincePrev / timeBetweenPrevNext);
            interpFactor = Math::Clamp(interpFactor, 0.0f, 1.0f);

            Quaternion rotation = Quaternion::SLerp(prevRotKF.value,
                                                    nextRotKF.value,
                                                    interpFactor);
            boneMatrix = Matrix4::RotateMatrix(rotation);
            bonesRotationMatrices.Add(boneName, boneMatrix);
        }
    }

    for (auto &it : bonesMatrices)
    {
        const String &boneName = it.first;

        Matrix4 positionMatrix;
        if (bonesPositionMatrices.ContainsKey(boneName))
        {
            positionMatrix = bonesPositionMatrices.Get(boneName);
        }

        Matrix4 rotationMatrix;
        if (bonesRotationMatrices.ContainsKey(boneName))
        {
            rotationMatrix = bonesRotationMatrices.Get(boneName);
        }

        Matrix4 scaleMatrix = Matrix4::Identity;

        Matrix4 &boneMatrix = it.second;
        boneMatrix = positionMatrix * rotationMatrix * scaleMatrix;
    }

    return bonesMatrices;
}

const Array<Animation::KeyFrame<Vector3>> &
Animation::GetPositionKeyFrames(const String &boneName) const
{
    if (!GetBoneNameToPositionKeyFrames().ContainsKey(boneName))
    {
        return Array<Animation::KeyFrame<Vector3>>::Empty();
    }
    return GetBoneNameToPositionKeyFrames().Get(boneName);
}

const Array<Animation::KeyFrame<Quaternion>> &
Animation::GetRotationKeyFrames(const String &boneName) const
{
    if (!GetBoneNameToRotationKeyFrames().ContainsKey(boneName))
    {
        return Array<Animation::KeyFrame<Quaternion>>::Empty();
    }
    return GetBoneNameToRotationKeyFrames().Get(boneName);
}

const Array<Animation::KeyFrame<Vector3>> &
Animation::GetScaleKeyFrames(const String &boneName) const
{
    if (!GetBoneNameToScaleKeyFrames().ContainsKey(boneName))
    {
        return Array<Animation::KeyFrame<Vector3>>::Empty();
    }
    return GetBoneNameToScaleKeyFrames().Get(boneName);
}

const Map< String, Array<Animation::KeyFrame<Vector3>> >&
Animation::GetBoneNameToPositionKeyFrames() const
{
    return m_boneNameToPositionKeyFrames;
}

const Map< String, Array<Animation::KeyFrame<Quaternion> > >&
Animation::GetBoneNameToRotationKeyFrames() const
{
    return m_boneNameToRotationKeyFrames;
}

const Map< String, Array<Animation::KeyFrame<Vector3> > >&
Animation::GetBoneNameToScaleKeyFrames() const
{
    return  m_boneNameToScaleKeyFrames;
}

void Animation::Import(const Path &animationFilepath)
{
    BANG_UNUSED(animationFilepath);
}

void Animation::ImportMeta(const MetaNode &metaNode)
{
    Asset::ImportMeta(metaNode);

    if (metaNode.Contains("WrapMode"))
    {
        SetWrapMode( SCAST<AnimationWrapMode>(metaNode.Get<int>("WrapMode")) );
    }

    if (metaNode.Contains("Speed"))
    {
        SetSpeed( metaNode.Get<float>("Speed") );
    }
}

void Animation::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);

    metaNode->Set("Speed", SCAST<float>(GetSpeed()));
    metaNode->Set("WrapMode", SCAST<int>(GetWrapMode()));
}

