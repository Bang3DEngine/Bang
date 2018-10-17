#ifndef BEHAVIOURMANAGER_H
#define BEHAVIOURMANAGER_H

#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/String.h"

namespace Bang
{
template <class>
class EventEmitter;
class Behaviour;
class IEventsDestroy;
class Library;
class Path;

class BehaviourManager : public EventListener<IEventsDestroy>
{
public:
    BehaviourManager();
    virtual ~BehaviourManager() override;

    static Behaviour *CreateBehaviourInstance(const String &behaviourName,
                                              Library *behavioursLibrary);
    static bool DeleteBehaviourInstance(const String &behaviourName,
                                        Behaviour *behaviour,
                                        Library *behavioursLibrary);

    virtual void Update();

    void SetBehavioursLibrary(const Path &libPath);
    void SetBehavioursLibrary(Library *behavioursLibrary);
    void RegisterBehaviour(Behaviour *behaviour);

    virtual bool IsInstanceCreationAllowed() const;
    Library *GetBehavioursLibrary() const;
    void DestroyBehavioursUsingCurrentLibrary();

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    static BehaviourManager *GetActive();

private:
    Library *m_behavioursLibrary = nullptr;
    Array<Behaviour *> p_behavioursUsingCurrentLibrary;
};
}

#endif  // BEHAVIOURMANAGER_H
