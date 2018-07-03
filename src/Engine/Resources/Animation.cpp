#include "Bang/Animation.h"

USING_NAMESPACE_BANG

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::AddKeyFrame(const Animation::KeyFrame &keyFrame)
{
    m_keyFrames.PushBack(keyFrame);
}

const Array<Animation::KeyFrame> &Animation::GetKeyFrames() const
{
    return m_keyFrames;
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

