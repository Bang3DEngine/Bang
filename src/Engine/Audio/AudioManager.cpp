#include "Bang/AudioManager.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <cstring>
#include <ostream>
#include <unordered_map>
#include <utility>

#include "Bang/ALAudioSource.h"
#include "Bang/Application.h"
#include "Bang/Array.h"
#include "Bang/Assert.h"
#include "Bang/AssetHandle.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/AudioClip.h"
#include "Bang/AudioPlayerRunnable.h"
#include "Bang/Debug.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/List.tcc"
#include "Bang/MutexLocker.h"
#include "Bang/UMap.tcc"

using namespace Bang;

AudioManager::AudioManager()
{
}

void AudioManager::Init()
{
    InitAL();
    m_threadPool.SetMaxThreadCount(256);
}

AudioManager::~AudioManager()
{
    for (const auto &pair : m_sourcesToPlayers)
    {
        AudioPlayerRunnable *audioPlayer = pair.second;
        ALuint sourceId = audioPlayer->GetALAudioSource()->GetALSourceId();
        alDeleteSources(1, &sourceId);
    }
    StopAllSounds();

    alcDestroyContext(m_alContext);
    alcCloseDevice(m_alDevice);
}

bool AudioManager::InitAL()
{
    bool extIsPresent = alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT");
    if (!extIsPresent)
    {
        Debug_Error("Enumeration extension not available.");
        return false;
    }

    String defaultDeviceName =
        alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);

    m_alDevice = alcOpenDevice(defaultDeviceName.ToCString());
    if (!m_alDevice)
    {
        Debug_Error("Could not start OpenAL Device '" << defaultDeviceName
                                                      << "'");
        return false;
    }

    m_alContext = alcCreateContext(m_alDevice, nullptr);
    if (!m_alContext)
    {
        Debug_Error("Could not start OpenAL Context");
        return false;
    }

    bool currentContextResult = alcMakeContextCurrent(m_alContext);
    if (!currentContextResult)
    {
        Debug_Error("Could not set OpenAL Context");
        return false;
    }

    return true;
}

String AudioManager::GetALErrorEnumString(ALenum errorEnum)
{
    switch (errorEnum)
    {
        case AL_NO_ERROR: return "AL_NO_ERROR";
        case AL_INVALID_NAME: return "AL_INVALID_NAME";
        case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
        case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
        case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
        case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
    }
    ASSERT(false);
    return "";
}

String AudioManager::GetALCErrorEnumString(ALCenum errorEnum)
{
    switch (errorEnum)
    {
        case ALC_NO_ERROR: return "ALC_NO_ERROR";
        case ALC_INVALID_DEVICE: return "ALC_INVALID_DEVICE";
        case ALC_INVALID_CONTEXT: return "ALC_INVALID_CONTEXT";
        case ALC_INVALID_ENUM: return "ALC_INVALID_ENUM";
        case ALC_INVALID_VALUE: return "ALC_INVALID_VALUE";
        case ALC_OUT_OF_MEMORY: return "ALC_OUT_OF_MEMORY";
    }
    ASSERT(false);
    return "";
}

void AudioManager::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (ALAudioSource *alAudioSource = DCAST<ALAudioSource *>(object))
    {
        OnALAudioSourceDestroyed(alAudioSource);
    }
    else if (AudioPlayerRunnable *audioPlayer =
                 DCAST<AudioPlayerRunnable *>(object))
    {
        OnAudioPlayerDestroyed(audioPlayer);
    }
}

void AudioManager::OnALAudioSourceDestroyed(ALAudioSource *alAudioSource)
{
    alAudioSource->Stop();
    if (m_sourcesToPlayers.ContainsKey(alAudioSource))
    {
        AudioPlayerRunnable *audioPlayer =
            m_sourcesToPlayers.Get(alAudioSource);
        audioPlayer->Stop();
    }
}

void AudioManager::OnAudioPlayerDestroyed(AudioPlayerRunnable *audioPlayer)
{
    if (m_sourcesToPlayers.ContainsValue(audioPlayer))
    {
        ALAudioSource *audioSource =
            m_sourcesToPlayers.GetKeysWithValue(audioPlayer).Front();
        audioSource->Stop();
    }
}

ALAudioSource *AudioManager::Play(AudioClip *audioClip,
                                  ALAudioSource *aas,
                                  float delay)
{
    if (audioClip)
    {
        AudioPlayerRunnable *player =
            new AudioPlayerRunnable(audioClip, aas, delay);
        AudioManager *am = AudioManager::GetInstance();
        bool started = am->m_threadPool.TryStart(player);
        if (started)
        {
            MutexLocker ml(&am->m_mutexCurrentAudios);
            BANG_UNUSED(ml);
            aas->EventEmitter<IEventsDestroy>::RegisterListener(am);
            am->m_sourcesToPlayers.Add(aas, player);
        }
    }
    return aas;
}

ALAudioSource *AudioManager::Play(AudioClip *audioClip,
                                  const AudioParams &params,
                                  float delay)
{
    ALAudioSource *aas = nullptr;
    if (audioClip)
    {
        aas = new ALAudioSource();
        aas->SetALBufferId(audioClip->GetALBufferId());
        aas->SetParams(params);
        aas->m_autoDelete = true;
        AudioManager::Play(audioClip, aas, delay);
    }
    return aas;
}

ALAudioSource *AudioManager::Play(const Path &audioClipFilepath,
                                  const AudioParams &params,
                                  float delay)
{
    AH<AudioClip> audioClip = Assets::Load<AudioClip>(audioClipFilepath);
    return AudioManager::Play(audioClip.Get(), params, delay);
}

void AudioManager::PauseAllSounds()
{
    AudioManager *am = AudioManager::GetInstance();
    for (const auto &pair : am->m_sourcesToPlayers)
    {
        AudioPlayerRunnable *audioPlayer = pair.second;
        audioPlayer->Pause();
    }
}

void AudioManager::ResumeAllSounds()
{
    AudioManager *am = AudioManager::GetInstance();
    for (const auto &pair : am->m_sourcesToPlayers)
    {
        AudioPlayerRunnable *audioPlayer = pair.second;
        audioPlayer->Resume();
    }
}

void AudioManager::StopAllSounds()
{
    AudioManager *am = AudioManager::GetInstance();
    for (const auto &pair : am->m_sourcesToPlayers)
    {
        AudioPlayerRunnable *audioPlayer = pair.second;
        audioPlayer->Stop();
    }

    MutexLocker m(&am->m_mutexCurrentAudios);
    am->m_sourcesToPlayers.Clear();
}

void AudioManager::SetPlayOnStartBlocked(bool blocked)
{
    AudioManager *am = AudioManager::GetInstance();
    am->m_playOnStartBlocked = blocked;
}

bool AudioManager::GetPlayOnStartBlocked()
{
    AudioManager *am = AudioManager::GetInstance();
    return am->m_playOnStartBlocked;
}

void AudioManager::OnAudioFinishedPlaying(AudioPlayerRunnable *audioPlayer)
{
    MutexLocker m(&m_mutexCurrentAudios);
    m_sourcesToPlayers.RemoveValues(audioPlayer);
}

void AudioManager::DettachSourcesFromAudioClip(AudioClip *ac)
{
    // Dettach all audioSources using this AudioClip.
    // Otherwise OpenAL throws error.
    AudioManager *am = AudioManager::GetInstance();
    for (const auto &pair : am->m_sourcesToPlayers)
    {
        AudioPlayerRunnable *audioPlayer = pair.second;
        if (audioPlayer->GetAudioClip() == ac)
        {
            audioPlayer->Stop();
            audioPlayer->GetALAudioSource()->SetALBufferId(0);
        }
    }
}

void AudioManager::ClearALErrors()
{
    alGetError();
    if (ALCdevice *device = AudioManager::GetInstance()->m_alDevice)
    {
        alcGetError(device);
    }
}

bool AudioManager::CheckALError()
{
    bool someError = false;
    {
        ALenum error = alGetError();
        bool hasError = (error != AL_NO_ERROR);
        if (hasError)
        {
            const char *errorStr = alGetString(error);
            Debug_Error("OpenAL al error("
                        << AudioManager::GetALErrorEnumString(error)
                        << "): "
                        << errorStr);
            someError = true;
        }
    }

    ALCdevice *device = AudioManager::GetInstance()->m_alDevice;
    if (device)
    {
        ALenum error = alcGetError(device);
        bool hasError = (error != AL_NO_ERROR);
        if (hasError)
        {
            const char *errorStr = alcGetString(device, error);
            Debug_Error("OpenAL alc error("
                        << AudioManager::GetALCErrorEnumString(error)
                        << "): "
                        << errorStr);
            someError = true;
        }
        alcGetError(device);
    }

    return someError;
}

List<String> AudioManager::GetAudioDevicesList()
{
    List<String> audioDevicesList;

    const ALCchar *devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
    const ALCchar *currentChar = devices;
    const ALCchar *nextChar = currentChar + 1;

    while (currentChar && *currentChar != '\0' && nextChar && *nextChar != '\0')
    {
        String currentDevice = String(currentChar);
        audioDevicesList.PushBack(currentDevice);

        size_t len = strlen(currentChar);
        currentChar += (len + 1);
        nextChar += (len + 2);
    }

    return audioDevicesList;
}

AudioManager *AudioManager::GetInstance()
{
    return Application::GetInstance()->GetAudioManager();
}
