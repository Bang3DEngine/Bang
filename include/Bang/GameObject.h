#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <queue>
#include <stack>
#include <tuple>

#include "Bang/List.h"
#include "Bang/Object.h"
#include "Bang/Component.h"
#include "Bang/IToString.h"
#include "Bang/RenderPass.h"
#include "Bang/IsContainer.h"
#include "Bang/Serializable.h"
#include "Bang/EventEmitter.h"
#include "Bang/IEventsName.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsComponent.h"
#include "Bang/IEventsGameObjectVisibilityChanged.h"

NAMESPACE_BANG_BEGIN

FORWARD class Scene;
FORWARD class Camera;
FORWARD class Component;
FORWARD class RectTransform;

#define GAMEOBJECT_NO_FRIEND(ClassName) \
    public: virtual ClassName* Clone() const override {\
        ClassName *clone = GameObject::Create<ClassName>();\
        CloneInto(clone);\
        return clone;\
    }\
    SERIALIZABLE(ClassName)

#define GAMEOBJECT(ClassName) \
    GAMEOBJECT_NO_FRIEND(ClassName) \
    friend class GameObject


class GameObject : public Object,
                   public IToString,
                   public EventListener<IEventsChildren>,
                   public EventEmitter<IEventsName>,
                   public EventEmitter<IEventsChildren>,
                   public EventEmitter<IEventsComponent>,
                   public EventEmitter<IEventsGameObjectVisibilityChanged>
{
    GAMEOBJECT_NO_FRIEND(GameObject)

public:
    virtual void PreStart() override;
    virtual void Start() override;
    virtual void Update();
    virtual void Render(RenderPass renderPass, bool renderChildren = true);
    void DestroyPending();

    template <class T = GameObject, class... Args>
    static T* Create(Args... args);
    static void Destroy(GameObject *gameObject);

    bool IsEnabled(bool recursive = false) const;

    void SetName(const String &m_name);
    const String& GetName() const;

    template <class T>
    T* AddComponent(int index = -1);
    Component* AddComponent(const String &componentClassName, int _index = -1);
    Component* AddComponent(Component *c, int index = -1);

    static GameObject *Find(const String &name);
    GameObject *FindInChildren(const GUID &guid, bool recursive = true);
    GameObject *FindInChildren(const String &name, bool recursive = true);
    GameObject *FindInChildrenAndThis(const GUID &guid, bool recursive = true);
    GameObject *FindInChildrenAndThis(const String &name, bool recursive = true);
    GameObject *FindInAncestors(const String &name, bool broadSearch);
    GameObject *FindInAncestorsAndThis(const String &name, bool broadSearch);

    void SetVisible(bool visible);
    void SetParent(GameObject *newParent, int _index = -1);
    void SetDontDestroyOnLoad(bool dontDestroyOnLoad);

    GameObject* GetChild(uint index) const;
    GameObject* GetChild(const GUID &guid) const;
    GameObject* GetChild(const String &name) const;
    const List<GameObject*>& GetChildren() const;
    List<GameObject*> GetChildrenRecursively() const;

    template <class T>
    T* GetComponent() const;
    Component* GetComponentByGUID(const GUID &guid) const;

    template <class T>
    List<T*> GetComponents() const;
    const List<Component*>& GetComponents() const;

    template <class T>
    T* GetComponentInParent(bool recursive = true) const;

    template <class T>
    List<T*> GetComponentsInParent(bool recursive = true) const;

    template <class T>
    List<T*> GetComponentsInParentAndThis(bool recursive = true) const;

    template <class T>
    T* GetComponentInChildren(bool recursive = true) const;

    template <class T>
    List<T*> GetComponentsInChildren(bool recursive = true) const;

    template <class T>
    T* GetComponentInChildrenOnly(bool recursive = true) const;
    template <class T>
    List<T*> GetComponentsInChildrenOnly(bool recursive = true) const;

    template <class T>
    bool HasComponent() const;
    bool HasComponent(const String &className) const;

    void RemoveComponent(Component *component);
    Scene* GetScene() const;
    Transform *GetTransform() const;
    RectTransform *GetRectTransform() const;
    GameObject* GetParent() const;
    bool IsChildOf(const GameObject *_parent, bool recursive = true) const;

    bool IsVisible() const;
    bool IsDontDestroyOnLoad() const;
    AARect GetBoundingViewportRect(Camera *cam, bool includeChildren = true) const;
    AABox GetLocalAABBox(bool includeChildren = true) const;
    AABox GetAABBoxWorld(bool includeChildren = true) const;
    Sphere GetLocalBoundingSphere(bool includeChildren = true) const;
    Sphere GetBoundingSphere(bool includeChildren = true) const;

    // Helper propagate functions
    template<class TListener, class TListenerInnerT, class TReturn, class... Args>
    void PropagateSingle(TReturn TListenerInnerT::*func,
                         TListener *receiver,
                         const Args&... args);

    template<class TListener, class TListenerInnerT, class TReturn, class... Args>
    void PropagateToList(TReturn TListenerInnerT::*func,
                         const List<TListener*> &list,
                         const Args&... args);

    template<class T, class TReturn, class... Args>
    void PropagateToChildren(TReturn T::*func, const Args&... args);

    template<class T, class TReturn, class... Args>
    void PropagateToComponents(TReturn T::*func, const Args&... args);

    template<class TListener, class TListenerInnerT, class TReturn, class... Args>
    void PropagateToChildrenListeners(TReturn TListener::*func,
                                      const Args&... args);

    template<class TListener, class TListenerInnerT, class TReturn, class... Args>
    void PropagateToComponentListeners(TReturn TListenerInnerT::*func,
                                       const Args&... args);


    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // IToString
    void Print(const String& indent = "") const;
    String ToStringStructure(bool recursive = false, const String &indent = "");
    String ToString() const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

protected:
    GameObject(const String &name = "GameObject");
    virtual ~GameObject();

    virtual void PreUpdate();
    virtual void BeforeChildrenUpdate();
    virtual void AfterChildrenUpdate();
    virtual void PostUpdate();
    virtual void BeforeRender();
    virtual void BeforeChildrenRender(RenderPass renderPass);
    virtual void AfterChildrenRender(RenderPass renderPass);
    virtual void ChildAdded(GameObject *addedChild, GameObject *parent);
    virtual void ChildRemoved(GameObject *removedChild, GameObject *parent);

    // IEventsObject
    virtual void OnEnabled(Object *object) override;
    virtual void OnDisabled(Object *object) override;

private:
    List<GameObject*> m_children;
    List<Component*> m_components;

    String m_name = "";
    bool m_visible = true;
    bool m_dontDestroyOnLoad = false;

    Transform *p_transform = nullptr;
    GameObject* p_parent = nullptr;

    std::queue<GameObject*> p_pendingGameObjectsToDestroy;
    std::queue<Component*> p_pendingComponentsToDestroy;

    // Concurrent modification when iterating stuff
    bool m_increaseChildrenIterator = true;
    bool m_increaseComponentsIterator = true;
    std::stack< List<GameObject*>::Iterator > m_currentChildrenIterators;
    std::stack< List<Component*>::Iterator  > m_currentComponentsIterators;

    void AddChild(GameObject *child, int index);
    void RemoveChild(GameObject *child);
    void MarkComponentForDestroyPending(Component *comp);

    friend class Scene;
    friend class Prefab;
    friend class GEngine;
    friend class Component;
    friend class SceneManager;
    friend class RectTransform;
};

NAMESPACE_BANG_END

#include "GameObject.tcc"

#endif // GAMEOBJECT_H
