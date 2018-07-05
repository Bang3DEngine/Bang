#include "Bang/Animation.h"

USING_NAMESPACE_BANG

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::AddPositionKeyFrame(const Animation::KeyFrame<Vector3> &keyFrame)
{
    m_positionKeyFrames.PushBack(keyFrame);
    PropagateResourceChanged();
}

void Animation::AddRotationKeyFrame(const Animation::KeyFrame<Quaternion> &keyFrame)
{
    m_rotationKeyFrames.PushBack(keyFrame);
    PropagateResourceChanged();
}

void Animation::AddScaleKeyFrame(const Animation::KeyFrame<Vector3> &keyFrame)
{
    m_scaleKeyFrames.PushBack(keyFrame);
    PropagateResourceChanged();
}

void Animation::SetFramesPerSecond(float framesPerSecond)
{
    if (framesPerSecond != GetFramesPerSecond())
    {
        m_framesPerSecond = framesPerSecond;
        PropagateResourceChanged();
    }
}

void Animation::SetDuration(float durationInSeconds)
{
    if (durationInSeconds != GetDuration())
    {
        m_durationSeconds = durationInSeconds;
        PropagateResourceChanged();
    }
}

float Animation::GetDuration() const
{
    return m_durationSeconds;
}

float Animation::GetFramesPerSecond() const
{
    return m_framesPerSecond;
}

const Array<Animation::KeyFrame<Vector3>>&
Animation::GetPositionKeyFrames() const
{
    return m_positionKeyFrames;
}

const Array<Animation::KeyFrame<Quaternion> >&
Animation::GetRotationKeyFrames() const
{
    return m_rotationKeyFrames;
}

const Array<Animation::KeyFrame<Vector3> >&
Animation::GetScaleKeyFrames() const
{
    return  m_scaleKeyFrames;
}

void Animation::Import(const Path &animationFilepath)
{
}

void Animation::ImportXML(const XMLNode &xmlInfo)
{
    Asset::ImportXML(xmlInfo);
}

void Animation::ExportXML(XMLNode *xmlInfo) const
{
    Asset::ExportXML(xmlInfo);
}

