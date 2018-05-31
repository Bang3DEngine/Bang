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
        T *tp = DCAST<T*>(comp);
        if (tp) { return tp; }
    }
    return nullptr;
}

template <class T>
List<T*> GameObject::GetComponents() const
{
    List<T*> comps_l;
    for (Component *comp : m_components)
    {
        T *tp = DCAST<T*>(comp);
        if (tp) { comps_l.PushBack(tp); }
    }
    return comps_l;
}

template <class T>
T* GameObject::GetComponentInParent(bool recursive) const
{
    if (!GetParent()) { return nullptr; }
    T* comp = GetParent()->GetComponent<T>();
    if (comp) { return comp; }
    return recursive ? GetParent()->GetComponentInParent<T>(recursive) : nullptr;
}

template <class T>
List<T*> GameObject::GetComponentsInParent(bool recursive) const
{
    List<T*> result;
    if (!GetParent()) { return result; }
    result = GetParent()->GetComponents<T>();
    return recursive ? result.Concat(GetParent()->GetComponentsInParent<T>(recursive)) :
                       result;
}

template <class T>
T* GameObject::GetComponentInChildren(bool recursive) const
{
    T *compThis = GetComponent<T>();
    if (compThis) { return compThis; }
    return GetComponentInChildrenOnly<T>(recursive);
}

template <class T>
List<T*> GameObject::GetComponentsInChildren(bool recursive) const
{
    List<T*> comps_l;
    comps_l.PushBack( GetComponents<T>() );
    comps_l.PushBack( GetComponentsInChildrenOnly<T>(recursive) );
    return comps_l;
}

template<class T>
T *GameObject::GetComponentInChildrenOnly(bool recursive) const
{
    for (auto c = GetChildren().Begin(); c != GetChildren().End(); ++c)
    {
        T *comp = (*c)->GetComponent<T>();
        if (comp) return comp;
        if (recursive)
        {
            comp = (*c)->GetComponentInChildren<T>(true);
            if (comp) return comp;
        }
    }
    return nullptr;
}

template<class T>
List<T*> GameObject::GetComponentsInChildrenOnly(bool recursive) const
{
    List<T*> comps;
    for (auto c = GetChildren().Begin(); c != GetChildren().End(); ++c)
    {
        List<T*> childChildrenComps =
                recursive ? (*c)->GetComponentsInChildren<T>(true) :
                            (*c)->GetComponents<T>();
        comps.Splice(comps.End(), childChildrenComps); //concat
    }
    return comps;
}

template <class T>
bool GameObject::HasComponent() const { return GetComponent<T>() ; }

template<class T>
bool CanEventBePropagated(const T& x)
{
    if (!x) { return false; }
    const Object *object = DCAST<const Object*>(x);
    return !object ||
           (object->IsEnabled() &&
           !object->IsWaitingToBeDestroyed());
}

template<class T, class TReturn, class... Args>
void GameObject::PropagateSingle(TReturn T::*func, T *receiver,
                                 const Args&... args)
{
    if (CanEventBePropagated(receiver))
    {
        (receiver->*func)(args...);
    }
}

template<class TListener, class TListenerInnerT, class TReturn, class... Args>
void GameObject::PropagateToList(TReturn TListenerInnerT::*func,
                                 const List<TListener*> &list,
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
            // func(comp, args...);
            // GameObject::Propagate(func, comp, args...);
        }
    }
    /*
    m_currentChildrenIterators.push(m_children.Begin());
    while (m_currentChildrenIterators.top() != m_children.End())
    {
        m_increaseChildrenIterator = true;
        GameObject *child = *(m_currentChildrenIterators.top());
        GameObject::Propagate(func, child, args...);
        if (m_increaseChildrenIterator) { ++m_currentChildrenIterators.top(); }
    }
    m_currentChildrenIterators.pop();
    */
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
            // func(comp, args...);
            // GameObject::Propagate(func, comp, args...);
        }
    }
    /*
    m_currentComponentsIterators.push(m_components.Begin());
    while (m_currentComponentsIterators.top() != m_components.End())
    {
        m_increaseComponentsIterator = true;
        Component *comp = *(m_currentComponentsIterators.top());
        GameObject::Propagate(func, comp, args...);
        if (m_increaseComponentsIterator) { ++m_currentComponentsIterators.top(); }
    }
    m_currentComponentsIterators.pop();
    */
}

template<class T>
List<T*> GameObject::GetComponentsInParentAndThis(bool recursive) const
{
    List<T*> comps_l;
    comps_l.PushBack( GetComponents<T>() );
    comps_l.PushBack( GetComponentsInParent<T>(recursive) );
    return comps_l;
}


NAMESPACE_BANG_END

#endif // GAMEOBJECT_TCC
