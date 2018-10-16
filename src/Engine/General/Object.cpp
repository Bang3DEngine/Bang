#include "Bang/Object.h"

#include "Bang/Assert.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsObject.h"

USING_NAMESPACE_BANG

Object::~Object()
{
    ASSERT( IsWaitingToBeDestroyed() );
}

bool Object::IsActive() const
{
    return IsEnabled() && !IsWaitingToBeDestroyed();
}

void Object::PreStart()
{
    if (!IsStarted())
    {
        OnPreStart();
    }
}

void Object::Start()
{
    if (!IsStarted())
    {
        OnStart();
        m_started = true;

        EventEmitter<IEventsObject>::
                PropagateToListeners(&IEventsObject::OnStarted, this);
    }
}

void Object::OnPreStart()
{
}

void Object::OnStart()
{
}

void Object::OnEnabled(Object *object)
{
    EventEmitter<IEventsObject>::
            PropagateToListeners(&IEventsObject::OnEnabled, object);
}

void Object::OnDisabled(Object *object)
{
    EventEmitter<IEventsObject>::
            PropagateToListeners(&IEventsObject::OnDisabled, object);
}

void Object::OnDestroy()
{
}

void Object::SetWaitingToBeDestroyed()
{
    m_waitingToBeDestroyed = true;
}

void Object::InvalidateEnabledRecursively()
{
    if (m_enabledRecursivelyValid)
    {
        m_enabledRecursivelyValid = false;
        OnEnabledRecursivelyInvalidated();
    }
}

void Object::PropagateObjectDestruction(Object *object)
{
    if (!object->IsWaitingToBeDestroyed())
    {
        object->SetWaitingToBeDestroyed();

        object->OnDestroy();
        object->EventEmitter<IEventsDestroy>::
                PropagateToListeners(&IEventsDestroy::OnDestroyed, object);
    }
}

void Object::OnEnabledRecursivelyInvalidated()
{
    // Empty
}

const ObjectId& Object::GetObjectId() const
{
    return m_objectId;
}

void Object::SetEnabled(bool enabled)
{
    if (enabled != IsEnabled())
    {
        m_enabled = enabled;
        InvalidateEnabledRecursively();

        if (IsEnabled())
        {
            OnEnabled(this);
            EventEmitter<IEventsObject>::PropagateToListeners(
                        &IEventsObject::OnEnabled, this);
        }
        else
        {
            OnDisabled(this);
            EventEmitter<IEventsObject>::PropagateToListeners(
                        &IEventsObject::OnDisabled, this);
        }
    }
}

bool Object::IsEnabled() const
{
    return m_enabled;
}

bool Object::IsStarted() const
{
    return m_started;
}

bool Object::IsActiveRecursively() const
{
    return IsActive() && IsEnabledRecursively();
}

bool Object::IsEnabledRecursively() const
{
    if (!m_enabledRecursivelyValid)
    {
        m_enabledRecursively = CalculateEnabledRecursively();
        m_enabledRecursivelyValid = true;
    }
    return m_enabledRecursively;
}

bool Object::IsWaitingToBeDestroyed() const
{
    return m_waitingToBeDestroyed;
}

void Object::CloneInto(ICloneable *clone) const
{
    Object *obj = SCAST<Object*>(clone);
    obj->SetEnabled( IsEnabled() );
}


