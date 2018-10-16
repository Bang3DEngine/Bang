#include "Bang/AudioListener.h"

#include <AL/al.h>

#include "Bang/FastDynamicCast.h"
#include "Bang/GameObject.h"
#include "Bang/MetaNode.h"
#include "Bang/Transform.h"
#include "Bang/Vector.tcc"
#include "Bang/Vector3.h"

USING_NAMESPACE_BANG

AudioListener::AudioListener()
{
    CONSTRUCT_CLASS_ID(AudioListener)
}

AudioListener::~AudioListener()
{

}

void AudioListener::OnUpdate()
{
    Component::OnUpdate();
    UpdateALProperties();
}

void AudioListener::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);
}

void AudioListener::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);
}

void AudioListener::UpdateALProperties() const
{
    alDistanceModel(AL_LINEAR_DISTANCE);
    //alDistanceModel(AL_EXPONENT_DISTANCE);

    Transform *tr = GetGameObject()->GetTransform();
    Vector3 at = -tr->GetForward();
    Vector3 up = tr->GetUp();
    ALfloat listenerOri[] = { at.x, at.y, at.z, up.x, up.y, up.z };
    alListenerfv(AL_ORIENTATION, listenerOri);
    //alListenerfv(AL_DIRECTION, tr->GetEuler().Data());
    alListenerfv(AL_POSITION, tr->GetPosition().Data());
    alListenerfv(AL_VELOCITY, Vector3::Zero.Data());
}
