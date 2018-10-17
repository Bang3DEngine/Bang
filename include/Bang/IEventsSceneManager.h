#ifndef IEVENTSSCENEMANAGER_H
#define IEVENTSSCENEMANAGER_H

#include "Bang/IEvents.h"

namespace Bang
{
class Path;
class Scene;

class IEventsSceneManager
{
    IEVENTS(IEventsSceneManager);

public:
    virtual void OnSceneLoaded(Scene *scene, const Path &sceneFilepath)
    {
        BANG_UNUSED_2(scene, sceneFilepath);
    }
};
}

#endif  // IEVENTSSCENEMANAGER_H
