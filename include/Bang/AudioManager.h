#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/List.h"
#include "Bang/Mutex.h"
#include "Bang/String.h"
#include "Bang/ThreadPool.h"
#include "Bang/UMap.h"


NAMESPACE_BANG_BEGIN

FORWARD_T class  EventEmitter;
FORWARD   class  ALAudioSource;
FORWARD   class  AudioClip;
FORWARD   class  AudioPlayerRunnable;
FORWARD   class  IEventsDestroy;
FORWARD   class  Path;
FORWARD   struct AudioParams;

class AudioManager : public EventListener<IEventsDestroy>
{
public:
    void Init();

    static ALAudioSource* Play(AudioClip* audioClip,
                               ALAudioSource *alAudioSource,
                               float delay = 0.0f);
    static ALAudioSource* Play(AudioClip* audioClip,
                               const AudioParams &params,
                               float delay = 0.0f);
    static ALAudioSource* Play(const Path& audioClipFilepath,
                               const AudioParams &params,
                               float delay = 0.0f);

    static void PauseAllSounds();
    static void ResumeAllSounds();
    static void StopAllSounds();
    static void SetPlayOnStartBlocked(bool blocked);

    static bool GetPlayOnStartBlocked();
    static void ClearALErrors();
    static bool CheckALError();

    static AudioManager *GetInstance();

private:
    ALCdevice *m_alDevice = nullptr;
    ALCcontext *m_alContext = nullptr;

    ThreadPool m_threadPool;
    Mutex m_mutexCurrentAudios;
    bool m_playOnStartBlocked = false;
    UMap<ALAudioSource*, AudioPlayerRunnable*> m_sourcesToPlayers;

    AudioManager();
    virtual ~AudioManager() override;

    bool InitAL();
    static List<String> GetAudioDevicesList();
    static String GetALErrorEnumString(ALenum errorEnum);
    static String GetALCErrorEnumString(ALCenum errorEnum);

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;
    void OnAudioPlayerDestroyed(AudioPlayerRunnable *audioPlayer);
    void OnALAudioSourceDestroyed(ALAudioSource *alAudioSource);

    // Handling of real-time buffer change
    static void DettachSourcesFromAudioClip(AudioClip *ac);

    void OnAudioFinishedPlaying(AudioPlayerRunnable *audioPlayer);

    friend class AudioClip;
    friend class Application;
    friend class AudioPlayerRunnable;
};

NAMESPACE_BANG_END

#endif // AUDIOMANAGER_H
