#include "Bang/AudioListener.h"

#include <AL/al.h>

#include "Bang/AudioManager.h"
#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/MetaNode.h"
#include "Bang/Transform.h"
#include "Bang/Vector3.h"

using namespace Bang;

AudioListener::AudioListener()
{
    SET_INSTANCE_CLASS_ID(AudioListener)
}

AudioListener::~AudioListener()
{
}

void AudioListener::OnUpdate()
{
    Component::OnUpdate();
    UpdateALProperties();
}

void AudioListener::UpdateALProperties() const
{
    BANG_AL_CALL(alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED));

    if (Transform *tr = GetGameObject()->GetTransform())
    {
        Vector3 at = -tr->GetForward();
        Vector3 up = tr->GetUp();
        Vector3 listenerPos = tr->GetPosition();
        ALfloat listenerOri[] = {at.x, at.y, at.z, up.x, up.y, up.z};
        BANG_AL_CALL(alListenerfv(AL_ORIENTATION, listenerOri));
        // BANG_AL_CALL(alListenerfv(AL_DIRECTION, tr->GetEuler().Data()));
        BANG_AL_CALL(alListenerfv(AL_POSITION, listenerPos.Data()));
        BANG_AL_CALL(alListenerfv(AL_VELOCITY, Vector3::Zero().Data()));
    }
}
