#pragma once

#include "Bang/Component.h"
#include "Bang/GameObject.h"

namespace Bang
{
template <class T>
T *GameObject::AddComponent(int index)
{
    T *c = new T();
    AddComponent(c, index);
    return c;
}

template <class T>
T *GameObject::GetComponent() const
{
    for (Component *comp : m_components)
    {
        if (comp)
        {
            if (T *tcomp = DCAST<T *>(comp))
            {
                return tcomp;
            }
        }
    }
    return nullptr;
}

template <class T>
T *GameObject::GetComponentInParent() const
{
    if (GameObject *parent = GetParent())
    {
        if (T *comp = parent->GetComponent<T>())
        {
            return comp;
        }
    }
    return nullptr;
}

template <class T>
T *GameObject::GetComponentInParentAndThis() const
{
    if (T *comp = GetComponent<T>())
    {
        return comp;
    }
    return GetComponentInParent<T>();
}

template <class T>
T *GameObject::GetComponentInAncestors() const
{
    if (T *comp = GetComponentInParent<T>())
    {
        return comp;
    }
    return GetParent() ? GetParent()->GetComponentInAncestors<T>() : nullptr;
}

template <class T>
T *GameObject::GetComponentInAncestorsAndThis() const
{
    if (T *comp = GetComponent<T>())
    {
        return comp;
    }
    return GetComponentInAncestors<T>();
}

template <class T>
T *GameObject::GetComponentInChildren() const
{
    for (GameObject *child : GetChildren())
    {
        if (child)
        {
            if (T *comp = child->GetComponent<T>())
            {
                return comp;
            }
        }
    }
    return nullptr;
}
template <class T>
T *GameObject::GetComponentInChildrenAndThis() const
{
    if (T *comp = GetComponent<T>())
    {
        return comp;
    }
    return GetComponentInChildren<T>();
}
template <class T>
T *GameObject::GetComponentInDescendants() const
{
    for (GameObject *child : GetChildren())
    {
        if (child)
        {
            if (T *comp = child->GetComponentInDescendantsAndThis<T>())
            {
                return comp;
            }
        }
    }
    return nullptr;
}

template <class T>
T *GameObject::GetComponentInDescendantsAndThis() const
{
    if (T *comp = GetComponent<T>())
    {
        return comp;
    }
    return GetComponentInDescendants<T>();
}

template <class T>
Array<T *> GameObject::GetComponents() const
{
    Array<T *> comps;
    GetComponents<T>(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponents(Array<T *> *components) const
{
    for (Component *comp : GetComponents())
    {
        if (comp)
        {
            if (T *tcomp = DCAST<T *>(comp))
            {
                components->PushBack(tcomp);
            }
        }
    }
}

template <class T>
Array<T *> GameObject::GetComponentsInParent() const
{
    Array<T *> comps;
    GetComponentsInParent(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInParent(Array<T *> *componentsOut) const
{
    if (GetParent())
    {
        GetParent()->GetComponents<T>(componentsOut);
    }
}
template <class T>
Array<T *> GameObject::GetComponentsInParentAndThis() const
{
    Array<T *> comps;
    GetComponentsInParentAndThis(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInParentAndThis(Array<T *> *componentsOut) const
{
    GetComponents<T>(componentsOut);
    GetComponentsInParent<T>(componentsOut);
}

template <class T>
Array<T *> GameObject::GetComponentsInAncestors() const
{
    Array<T *> comps;
    GetComponentsInAncestors(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInAncestors(Array<T *> *componentsOut) const
{
    if (GetParent())
    {
        GetParent()->GetComponents<T>(componentsOut);
        GetParent()->GetComponentsInAncestors<T>(componentsOut);
    }
}
template <class T>
Array<T *> GameObject::GetComponentsInAncestorsAndThis() const
{
    Array<T *> comps;
    GetComponentsInAncestorsAndThis(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInAncestorsAndThis(
    Array<T *> *componentsOut) const
{
    GetComponents<T>(componentsOut);
    GetComponentsInAncestors<T>(componentsOut);
}

template <class T>
Array<T *> GameObject::GetComponentsInChildren() const
{
    Array<T *> comps;
    GetComponentsInChildren(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInChildren(Array<T *> *componentsOut) const
{
    for (GameObject *child : GetChildren())
    {
        if (child)
        {
            child->GetComponents<T>(componentsOut);
        }
    }
}
template <class T>
Array<T *> GameObject::GetComponentsInChildrenAndThis() const
{
    Array<T *> comps;
    GetComponentsInChildrenAndThis(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInChildrenAndThis(Array<T *> *componentsOut) const
{
    GetComponents<T>(componentsOut);
    GetComponentsInChildren<T>(componentsOut);
}
template <class T>
Array<T *> GameObject::GetComponentsInDescendants() const
{
    Array<T *> comps;
    GetComponentsInDescendants(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInDescendants(Array<T *> *componentsOut) const
{
    for (GameObject *child : GetChildren())
    {
        if (child)
        {
            child->GetComponents<T>(componentsOut);
            child->GetComponentsInDescendants<T>(componentsOut);
        }
    }
}
template <class T>
Array<T *> GameObject::GetComponentsInDescendantsAndThis() const
{
    Array<T *> comps;
    GetComponentsInDescendantsAndThis(&comps);
    return comps;
}
template <class T>
void GameObject::GetComponentsInDescendantsAndThis(
    Array<T *> *componentsOut) const
{
    GetComponents<T>(componentsOut);
    GetComponentsInDescendants<T>(componentsOut);
}

template <class T>
bool GameObject::HasComponent() const
{
    return GetComponent<T>();
}

template <class T>
T *GameObject::FindObjectInDescendants() const
{
    if (T *obj = DCAST<T *>(const_cast<GameObject *>(this)))
    {
        return obj;
    }

    const Array<GameObject *> &children = GetChildren();
    for (GameObject *child : children)
    {
        if (child)
        {
            if (T *obj = child->FindObjectInDescendants<T>())
            {
                return obj;
            }
        }
    }

    if (T *obj = GetComponent<T>())
    {
        return obj;
    }

    return nullptr;
}

template <class T>
bool CanEventBePropagated(const T &x)
{
    if (!x)
    {
        return false;
    }

    const Object *object = DCAST<const Object *>(x);
    return !object || !object->IsWaitingToBeDestroyed();
}

template <class TListener, class TListenerInnerT, class TReturn, class... Args>
void GameObject::PropagateToArray(TReturn TListenerInnerT::*func,
                                  const Array<TListener *> &list,
                                  const Args &... args)
{
    for (TListener *listener : list)
    {
        if (CanEventBePropagated(listener) && listener->IsReceivingEvents())
        {
            (listener->*func)(args...);
        }
    }
}

template <class TListener, class TReturn, class... Args>
void GameObject::PropagateToChildren(TReturn TListener::*func,
                                     const Args &... args)
{
    PropagateToChildren([&](GameObject *child) { (child->*func)(args...); });
}

template <class TListener, class TReturn, class... Args>
void GameObject::PropagateToComponents(TReturn TListener::*func,
                                       const Args &... args)
{
    PropagateToComponents([&](Component *comp) { (comp->*func)(args...); });
}
}
