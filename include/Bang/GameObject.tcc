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
T *GameObject::GetObject() const
{
    if (T *obj = DCAST<T *>(const_cast<GameObject *>(this)))
    {
        return obj;
    }
    return GetComponent<T>();
}
template <class T>
Array<T *> GameObject::GetObjects() const
{
    Array<T *> objs;
    if (T *obj = DCAST<T *>(const_cast<GameObject *>(this)))
    {
        objs.PushBack(obj);
    }
    objs.PushBack(GetComponents<T>());
    return objs;
}

template <class T>
T *GameObject::GetObjectInChildren() const
{
    const Array<GameObject *> &children = GetChildren();
    for (GameObject *child : children)
    {
        if (T *obj = child->GetObject<T>())
        {
            return obj;
        }
    }
    return nullptr;
}

template <class T>
T *GameObject::GetObjectInChildrenAndThis() const
{
    if (T *obj = GetObject<T>())
    {
        return obj;
    }
    return GetObjectInChildren<T>();
}

template <class T>
Array<T *> GameObject::GetObjectsInChildren() const
{
    Array<T *> objs;
    const Array<GameObject *> &children = GetChildren();
    for (GameObject *child : children)
    {
        if (child)
        {
            objs.PushBack(child->GetObjects<T>());
        }
    }
    return objs;
}
template <class T>
Array<T *> GameObject::GetObjectsInChildrenAndThis() const
{
    Array<T *> objs;
    objs.PushBack(GetObjects<T>());
    objs.PushBack(GetObjectsInChildren<T>());
    return objs;
}
template <class T>
T *GameObject::GetObjectInParent() const
{
    if (GameObject *parent = GetParent())
    {
        return GetParent()->GetObject<T>();
    }
    return nullptr;
}

template <class T>
T *GameObject::GetObjectInParentAndThis() const
{
    if (T *obj = GetObject<T>())
    {
        return obj;
    }
    return GetObjectInParent<T>();
}

template <class T>
Array<T *> GameObject::GetObjectsInParent() const
{
    if (GameObject *parent = GetParent())
    {
        return GetParent()->GetObjects<T>();
    }
    return Array<T *>::Empty();
}

template <class T>
Array<T *> GameObject::GetObjectsInParentAndThis() const
{
    Array<T *> objs;
    objs.PushBack(GetObjects<T>());
    objs.PushBack(GetObjectsInParent<T>());
    return objs;
}

template <class T>
T *GameObject::GetObjectInDescendants() const
{
    const Array<GameObject *> descendants = GetDescendants();
    for (GameObject *descendant : descendants)
    {
        if (T *obj = descendant->GetObject<T>())
        {
            return obj;
        }
    }
    return nullptr;
}

template <class T>
T *GameObject::GetObjectInDescendantsAndThis() const
{
    if (T *obj = GetObject<T>())
    {
        return obj;
    }
    return GetObjectInDescendants<T>();
}

template <class T>
Array<T *> GameObject::GetObjectsInDescendants() const
{
    Array<T *> objs;
    const Array<GameObject *> descendants = GetDescendants();
    for (GameObject *descendant : descendants)
    {
        if (descendant)
        {
            objs.PushBack(descendant->GetObjects<T>());
        }
    }
    return objs;
}
template <class T>
Array<T *> GameObject::GetObjectsInDescendantsAndThis() const
{
    Array<T *> objs;
    objs.PushBack(GetObjects<T>());
    objs.PushBack(GetObjectsInDescendants<T>());
    return objs;
}

template <class T>
T *GameObject::GetObjectInAscendants() const
{
    const Array<GameObject *> ascendants = GetAscendants();
    for (GameObject *ascendant : ascendants)
    {
        if (ascendant)
        {
            if (T *obj = ascendant->GetObject<T>())
            {
                return obj;
            }
        }
    }
    return nullptr;
}

template <class T>
T *GameObject::GetObjectInAscendantsAndThis() const
{
    if (T *obj = GetObject<T>())
    {
        return obj;
    }
    return GetObjectInAscendants<T>();
}

template <class T>
Array<T *> GameObject::GetObjectsInAscendants() const
{
    Array<T *> objs;
    const Array<GameObject *> ascendants = GetAscendants();
    for (GameObject *ascendant : ascendants)
    {
        if (ascendant)
        {
            objs.PushBack(ascendant->GetObjects<T>());
        }
    }
    return objs;
}
template <class T>
Array<T *> GameObject::GetObjectsInAscendantsAndThis() const
{
    Array<T *> objs;
    objs.PushBack(GetObjects<T>());
    objs.PushBack(GetObjectsInAscendants<T>());
    return objs;
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
