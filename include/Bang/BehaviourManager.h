#ifndef BEHAVIOURMANAGER_H
#define BEHAVIOURMANAGER_H

#include "Bang/Bang.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"

NAMESPACE_BANG_BEGIN

FORWARD class Library;
FORWARD class Behaviour;

class BehaviourManager : public EventListener<IEventsDestroy>
{
public:
	BehaviourManager();
	virtual ~BehaviourManager();

    static Behaviour* CreateBehaviourInstance(const String &behaviourName,
                                              Library *behavioursLibrary);
    static bool DeleteBehaviourInstance(const String &behaviourName,
                                        Behaviour *behaviour,
                                        Library *behavioursLibrary);

    virtual void Update();

    void SetBehavioursLibrary(const Path &libPath);
    void SetBehavioursLibrary(Library *behavioursLibrary);
    void RegisterBehaviour(Behaviour *behaviour);

    virtual bool IsInstanceCreationAllowed() const;
    Library* GetBehavioursLibrary() const;
    void DestroyBehavioursUsingCurrentLibrary();

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    static BehaviourManager* GetActive();

private:
    Library *m_behavioursLibrary = nullptr;
    Array<Behaviour*> p_behavioursUsingCurrentLibrary;

};

NAMESPACE_BANG_END

#endif // BEHAVIOURMANAGER_H

