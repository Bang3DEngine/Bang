#include "Bang/BehaviourManager.h"

#include "Bang/Application.h"
#include "Bang/Assert.h"
#include "Bang/Behaviour.h"
#include "Bang/Component.h"
#include "Bang/Debug.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/Library.h"
#include "Bang/SceneManager.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

namespace Bang
{
class Path;
}

using namespace Bang;

BehaviourManager::BehaviourManager()
{
}

BehaviourManager::~BehaviourManager()
{
    SetBehavioursLibrary(nullptr);
}

Behaviour *BehaviourManager::CreateBehaviourInstance(
    const String &behaviourName,
    Library *behavioursLib)
{
    if (!behavioursLib)
    {
        Debug_Error("No current behavioursLib...");
        return nullptr;
    }

    if (!behavioursLib->IsLoaded())
    {
        Debug_Error("No loaded behavioursLib...");
        if (!behavioursLib->Load())
        {
            return nullptr;
        }
    }

    String errorString = "";
    if (behavioursLib->IsLoaded())
    {
        // Get the pointer to the CreateDynamically function
        String funcName = "CreateDynamically_" + behaviourName;
        Behaviour *(*createFunction)(Application *) =
            behavioursLib->Get<Behaviour *(*)(Application *)>(
                funcName.ToCString());

        if (createFunction)
        {
            // Call it and get the pointer to the created Behaviour
            // Create the Behaviour, passing to it the Application
            // of this main binary, so it can link it.
            return createFunction(Application::GetInstance());
        }
        else
        {
            errorString = behavioursLib->GetErrorString();
        }
    }
    else
    {
        errorString = behavioursLib->GetErrorString();
    }

    Debug_Error(errorString);
    return nullptr;
}

bool BehaviourManager::DeleteBehaviourInstance(const String &behaviourName,
                                               Behaviour *behaviour,
                                               Library *behavioursLib)
{
    if (!behavioursLib)
    {
        delete behaviour;
        return false;
    }

    // Get the pointer to the DeleteDynamically function
    String funcName = "DeleteDinamically_" + behaviourName;
    void (*deleteFunction)(Behaviour *) =
        (behavioursLib->Get<void (*)(Behaviour *)>(funcName.ToCString()));

    if (deleteFunction)
    {
        deleteFunction(behaviour);
        return true;
    }

    Debug_Error(behavioursLib->GetErrorString());
    return false;
}

void BehaviourManager::Update()
{
}

bool BehaviourManager::IsInstanceCreationAllowed() const
{
    return true;
}

Library *BehaviourManager::GetBehavioursLibrary() const
{
    return m_behavioursLibrary;
}

void BehaviourManager::DestroyBehavioursUsingCurrentLibrary()
{
    while (!p_behavioursUsingCurrentLibrary.IsEmpty())
    {
        Behaviour *behaviour = p_behavioursUsingCurrentLibrary.Back();
        Component::DestroyImmediate(behaviour);
    }
}

void BehaviourManager::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    Behaviour *behaviour = SCAST<Behaviour *>(object);
    p_behavioursUsingCurrentLibrary.Remove(behaviour);
    ASSERT(!p_behavioursUsingCurrentLibrary.Contains(behaviour));
}

BehaviourManager *BehaviourManager::GetActive()
{
    return SceneManager::GetActive()->GetBehaviourManager();
}

void BehaviourManager::SetBehavioursLibrary(const Path &libPath)
{
    Library *behavioursLib = new Library(libPath);
    SetBehavioursLibrary(behavioursLib);
}

void BehaviourManager::SetBehavioursLibrary(Library *behavioursLibrary)
{
    if (behavioursLibrary != GetBehavioursLibrary())
    {
        if (GetBehavioursLibrary())
        {
            DestroyBehavioursUsingCurrentLibrary();
            delete GetBehavioursLibrary();
        }

        m_behavioursLibrary = behavioursLibrary;

        if (GetBehavioursLibrary())
        {
            Debug_DLog("Going to load BehavioursLibrary "
                       << GetBehavioursLibrary()->GetLibraryPath());

            if (!GetBehavioursLibrary()->Load())
            {
                Debug_Error(GetBehavioursLibrary()->GetErrorString());
            }
        }
    }
}

void BehaviourManager::RegisterBehaviour(Behaviour *behaviour)
{
    if (!p_behavioursUsingCurrentLibrary.Contains(behaviour))
    {
        p_behavioursUsingCurrentLibrary.PushBack(behaviour);
        behaviour->EventEmitter<IEventsDestroy>::RegisterListener(this);
    }
}
