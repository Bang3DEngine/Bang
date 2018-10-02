#include "Bang/Object.h"

#include "Bang/Debug.h"

USING_NAMESPACE_BANG

Object::~Object()
{
    ASSERT( IsWaitingToBeDestroyed() );
}

bool Object::IsActive() const
{
    return IsStarted() && IsEnabled() && !IsWaitingToBeDestroyed();
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
        InvalidateStartedRecursively();

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

void Object::InvalidateEnabledRecursively()
{
    if (m_enabledRecursivelyValid)
    {
        m_enabledRecursivelyValid = false;
        OnEnabledRecursivelyInvalidated();
    }
}

void Object::InvalidateStartedRecursively()
{
    if (m_startedRecursivelyValid)
    {
        m_startedRecursivelyValid = false;
        OnStartedRecursivelyInvalidated();
    }
}

void Object::PropagateObjectDestruction(Object *object)
{
    if (!object->IsWaitingToBeDestroyed())
    {
        object->m_waitingToBeDestroyed = true;

        object->OnDestroy();
        object->EventEmitter<IEventsDestroy>::
                PropagateToListeners(&IEventsDestroy::OnDestroyed, object);
    }
}

void Object::OnEnabledRecursivelyInvalidated()
{
    // Empty
}

void Object::OnStartedRecursivelyInvalidated()
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
    return IsActive() &&
           IsEnabledRecursively() &&
           IsStartedRecursively();
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

bool Object::IsStartedRecursively() const
{
    if (!m_startedRecursivelyValid)
    {
        m_startedRecursively = CalculateStartedRecursively();
        m_startedRecursivelyValid = true;
    }
    return m_startedRecursively;
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


