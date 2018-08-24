#ifndef GAMEOBJECT_TCC
#define GAMEOBJECT_TCC

#include "Bang/GameObject.h"
#include "Bang/ComponentFactory.h"

NAMESPACE_BANG_BEGIN

template <class T, class... Args>
T* GameObject::Create(Args... args)
{
    T* gameObject = new T(args...);
    return gameObject;
}

template <class T>
T* GameObject::AddComponent(int index)
{
    T *c = Component::Create<T>();
    this->AddComponent(c, index);
    return c;
}

template <class T>
T* GameObject::GetComponent() const
{
    for (Component *comp : m_components)
    {
        if (T *tcomp = DCAST<T*>(comp))
        {
            return tcomp;
        }
    }
    return nullptr;
}

template <class T>
T* GameObject::GetComponentInParent(bool recursive) const
{
    if (GetParent())
    {
        if (T *comp = GetParent()->GetComponent<T>())
        {
            return comp;
        }
        if (recursive)
        {
            return GetParent()->GetComponentInParent<T>(true);
        }
    }
    return nullptr;
}

template <class T>
T* GameObject::GetComponentInParentAndThis(bool recursive) const
{
    if (T* comp = GetComponent<T>())
    {
        return comp;
    }
    return GetComponentInParent<T>(recursive);
}

template <class T>
T* GameObject::GetComponentInChildren(bool recursive) const
{
    for (GameObject *child : GetChildren())
    {
        if (T *comp = child->GetComponent<T>())
        {
            return comp;
        }

        if (recursive)
        {
            if (T *comp = child->GetComponentInChildren<T>(true))
            {
                return comp;
            }
        }
    }
    return nullptr;
}
template <class T>
T* GameObject::GetComponentInChildrenAndThis(bool recursive) const
{
    if (T* comp = GetComponent<T>())
    {
        return comp;
    }
    return GetComponentInChildren<T>(recursive);
}

template <class T>
Array<T*> GameObject::GetComponents() const
{
    Array<T*> comps;
    GetComponents<T>(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponents(Array<T*> *components) const
{
    for (Component *comp : GetComponents())
    {
        if (T *tcomp = DCAST<T*>(comp))
        {
            components->PushBack(tcomp);
        }
    }
}

template <class T>
Array<T*> GameObject::GetComponentsInParent() const
{
    Array<T*> comps;
    GetComponentsInParent(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInParent(Array<T*> *componentsOut) const
{
    if (GetParent())
    {
        GetParent()->GetComponents<T>(componentsOut);
    }
}
template <class T>
Array<T*> GameObject::GetComponentsInParentAndThis() const
{
    Array<T*> comps;
    GetComponentsInParentAndThis(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInParentAndThis(Array<T*> *componentsOut) const
{
    GetComponents<T>(componentsOut);
    GetComponentsInParent<T>(componentsOut);
}

template <class T>
Array<T*> GameObject::GetComponentsInAncestors() const
{
    Array<T*> comps;
    GetComponentsInAncestors(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInAncestors(Array<T*> *componentsOut) const
{
    if (GetParent())
    {
        GetParent()->GetComponents<T>(componentsOut);
        GetParent()->GetComponentsInAncestors<T>(componentsOut);
    }
}
template <class T>
Array<T*> GameObject::GetComponentsInAncestorsAndThis() const
{
    Array<T*> comps;
    GetComponentsInAncestorsAndThis(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInAncestorsAndThis(Array<T*> *componentsOut) const
{
    GetComponents<T>(componentsOut);
    GetComponentsInAncestors<T>(componentsOut);
}

template <class T>
Array<T*> GameObject::GetComponentsInChildren() const
{
    Array<T*> comps;
    GetComponentsInChildren(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInChildren(Array<T*> *componentsOut) const
{
    for (GameObject *child : GetChildren())
    {
        child->GetComponents<T>(componentsOut);
    }
}
template <class T>
Array<T*> GameObject::GetComponentsInChildrenAndThis() const
{
    Array<T*> comps;
    GetComponentsInChildrenAndThis(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInChildrenAndThis(Array<T*> *componentsOut) const
{
    GetComponents<T>(componentsOut);
    GetComponentsInChildren<T>(componentsOut);
}
template <class T>
Array<T*> GameObject::GetComponentsInDescendants() const
{
    Array<T*> comps;
    GetComponentsInDescendants(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInDescendants(Array<T*> *componentsOut) const
{
    for (GameObject *child : GetChildren())
    {
        child->GetComponents<T>(componentsOut);
        child->GetComponentsInDescendants<T>(componentsOut);
    }
}
template <class T>
Array<T*> GameObject::GetComponentsInDescendantsAndThis() const
{
    Array<T*> comps;
    GetComponentsInDescendantsAndThis(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInDescendantsAndThis(Array<T*> *componentsOut) const
{
    GetComponents<T>(componentsOut);
    GetComponentsInDescendants<T>(componentsOut);
}

template <class T>
bool GameObject::HasComponent() const
{
    return GetComponent<T>() ;
}

template<class T>
bool CanEventBePropagated(const T& x)
{
    if (!x)
    {
        return false;
    }

    const Object *object = DCAST<const Object*>(x);
    return !object ||
           (object->IsEnabled() &&
           !object->IsWaitingToBeDestroyed());
}


template<class TListener, class TListenerInnerT, class TReturn, class... Args>
void GameObject::PropagateSingle(TReturn TListenerInnerT::*func,
                                 TListener *receiver,
                                 const Args&... args)
{
    if (CanEventBePropagated(receiver))
    {
        (receiver->*func)(args...);
    }
}

template<class TListener, class TListenerInnerT, class TReturn, class... Args>
void GameObject::PropagateToArray(TReturn TListenerInnerT::*func,
                                 const Array<TListener*> &list,
                                 const Args&... args)
{
    for (TListener *listener : list)
    {
        if (CanEventBePropagated(listener) && listener->IsReceivingEvents())
        {
            (listener->*func)(args...);
        }
    }
}

template<class TListener, class TReturn, class... Args>
void GameObject::PropagateToChildren(TReturn TListener::*func,
                                     const Args&... args)
{
    for (GameObject *child : m_children)
    {
        if (child->IsEnabled())
        {
            (child->*func)(args...);
        }
    }
}

template<class TListener, class TReturn, class... Args>
void GameObject::PropagateToComponents(TReturn TListener::*func,
                                       const Args&... args)
{
    for (Component *comp : m_components)
    {
        if (comp->IsEnabled())
        {
            (comp->*func)(args...);
        }
    }
}


NAMESPACE_BANG_END

#endif // GAMEOBJECT_TCC
