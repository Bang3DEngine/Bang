#include "Bang/ALAudioSource.h"

#include "Bang/AudioManager.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/Math.h"

using namespace Bang;

ALAudioSource::ALAudioSource()
{
    BANG_AL_CALL(alGenSources(1, &m_alSourceId));
    UpdateALProperties();  // Initialize AL source
}

ALAudioSource::~ALAudioSource()
{
    Stop();
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);
    BANG_AL_CALL(alDeleteSources(1, &m_alSourceId));
}

void ALAudioSource::Play()
{
    UpdateALProperties();
    if (GetALSourceId() > 0)
    {
        BANG_AL_CALL(alSourcePlay(GetALSourceId()));
    }
}

void ALAudioSource::Pause()
{
    if (GetALSourceId() > 0)
    {
        BANG_AL_CALL(alSourcePause(GetALSourceId()));
    }
}

void ALAudioSource::Stop()
{
    if (GetALSourceId() > 0)
    {
        BANG_AL_CALL(alSourceStop(GetALSourceId()));
    }
}

void ALAudioSource::SetVolume(float volume)
{
    if (volume != GetVolume())
    {
        m_audioParams.volume = volume;
        UpdateALProperties();
    }
}
void ALAudioSource::SetPitch(float pitch)
{
    float clampedPitch = Math::Max(pitch, 0.01f);
    if (clampedPitch != GetPitch())
    {
        m_audioParams.pitch = clampedPitch;
        UpdateALProperties();
    }
}
void ALAudioSource::SetRange(float range)
{
    if (range != GetRange())
    {
        m_audioParams.range = range;
        UpdateALProperties();
    }
}
void ALAudioSource::SetLooping(bool looping)
{
    if (looping != GetLooping())
    {
        m_audioParams.looping = looping;
        UpdateALProperties();
    }
}

void ALAudioSource::SetPosition(const Vector3 &position)
{
    if (position != GetPosition())
    {
        m_audioParams.position = position;
        UpdateALProperties();

        // Vector3 at = -transform->GetForward(), up = transform->GetUp();
        // ALfloat listenerOri[] = { at.x, at.y, at.z, up.x, up.y, up.z };
        // alSourcefv(m_alSourceId, AL_ORIENTATION, listenerOri);
    }
}

void ALAudioSource::SetParams(const AudioParams &audioParams)
{
    SetVolume(audioParams.volume);
    SetPitch(audioParams.pitch);
    SetRange(audioParams.range);
    SetLooping(audioParams.looping);
    SetPosition(audioParams.position);
}

void ALAudioSource::SetALBufferId(ALuint bufferId)
{
    if (bufferId != m_bufferId && (GetALSourceId() > 0))
    {
        m_bufferId = bufferId;
        BANG_AL_CALL(alSourcei(GetALSourceId(), AL_BUFFER, bufferId));
    }
}

void ALAudioSource::UpdateALProperties() const
{
    if (GetALSourceId() > 0)
    {
        BANG_AL_CALL(alSourcef(GetALSourceId(), AL_GAIN, GetVolume()));
        BANG_AL_CALL(alSourcef(GetALSourceId(), AL_PITCH, GetPitch()));
        BANG_AL_CALL(alSourcef(GetALSourceId(), AL_ROLLOFF_FACTOR, 1.0f));
        BANG_AL_CALL(alSourcef(
            GetALSourceId(), AL_MAX_DISTANCE, Math::Max(GetRange(), 0.01f)));
        BANG_AL_CALL(alSourcef(GetALSourceId(),
                               AL_REFERENCE_DISTANCE,
                               Math::Max(GetRange() * 0.5f, 0.01f)));
        BANG_AL_CALL(alSourcei(GetALSourceId(), AL_LOOPING, GetLooping()));
        BANG_AL_CALL(
            alSourcefv(GetALSourceId(), AL_POSITION, GetPosition().Data()));
    }
}

bool ALAudioSource::IsPlaying() const
{
    return GetState() == State::PLAYING;
}
bool ALAudioSource::IsPaused() const
{
    return GetState() == State::PAUSED;
}
bool ALAudioSource::IsStopped() const
{
    return GetState() == State::STOPPED;
}
float ALAudioSource::GetVolume() const
{
    return m_audioParams.volume;
}
float ALAudioSource::GetPitch() const
{
    return m_audioParams.pitch;
}
float ALAudioSource::GetRange() const
{
    return m_audioParams.range;
}
ALuint ALAudioSource::GetALSourceId() const
{
    return m_alSourceId;
}
const Vector3 &ALAudioSource::GetPosition() const
{
    return m_audioParams.position;
}
const AudioParams &ALAudioSource::GetParams()
{
    return m_audioParams;
}
bool ALAudioSource::GetLooping() const
{
    return m_audioParams.looping;
}
ALAudioSource::State ALAudioSource::GetState() const
{
    ALint state;
    if (GetALSourceId() > 0)
    {
        alGetSourcei(GetALSourceId(), AL_SOURCE_STATE, &state);
    }
    return static_cast<State>(state);
}
