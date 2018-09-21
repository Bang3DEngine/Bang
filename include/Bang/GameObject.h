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

    template <class T = GameObject, class... Args>
    static T* Create(Args... args);
    static void Destroy(GameObject *gameObject);
    static void DestroyImmediate(GameObject *gameObject);

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
    void SetParent(GameObject *newParent,
                   int _index = -1,
                   bool keepWorldTransform = false);
    void SetDontDestroyOnLoad(bool dontDestroyOnLoad);

    int GetIndexInsideParent() const;
    GameObject* GetChild(uint index) const;
    GameObject* GetChild(const GUID &guid) const;
    GameObject* GetChild(const String &name) const;
    const Array<GameObject*>& GetChildren() const;
    Array<GameObject*> GetChildrenRecursively() const;
    void GetChildrenRecursively(Array<GameObject*> *children) const;

    Component* GetComponentByGUID(const GUID &guid) const;

    const Array<Component*>& GetComponents() const;

    template <class T>
    T* GetComponent() const;
    template <class T>
    T* GetComponentInParent() const;
    template <class T>
    T* GetComponentInParentAndThis() const;
    template <class T>
    T* GetComponentInAncestors() const;
    template <class T>
    T* GetComponentInAncestorsAndThis() const;

    template <class T>
    T* GetComponentInChildren() const;
    template <class T>
    T* GetComponentInChildrenAndThis() const;
    template <class T>
    T* GetComponentInDescendants() const;
    template <class T>
    T* GetComponentInDescendantsAndThis() const;

    template <class T>
    Array<T*> GetComponents() const;
    template <class T>
    void GetComponents(Array<T*> *componentsOut) const;

    template <class T>
    Array<T*> GetComponentsInParent() const;
    template <class T>
    void GetComponentsInParent(Array<T*> *componentsOut) const;
    template <class T>
    Array<T*> GetComponentsInParentAndThis() const;
    template <class T>
    void GetComponentsInParentAndThis(Array<T*> *componentsOut) const;
    template <class T>
    Array<T*> GetComponentsInAncestors() const;
    template <class T>
    void GetComponentsInAncestors(Array<T*> *componentsOut) const;
    template <class T>
    Array<T*> GetComponentsInAncestorsAndThis() const;
    template <class T>
    void GetComponentsInAncestorsAndThis(Array<T*> *componentsOut) const;

    template <class T>
    Array<T*> GetComponentsInChildren() const;
    template <class T>
    void GetComponentsInChildren(Array<T*> *componentsOut) const;
    template <class T>
    Array<T*> GetComponentsInChildrenAndThis() const;
    template <class T>
    void GetComponentsInChildrenAndThis(Array<T*> *componentsOut) const;
    template <class T>
    Array<T*> GetComponentsInDescendants() const;
    template <class T>
    void GetComponentsInDescendants(Array<T*> *componentsOut) const;
    template <class T>
    Array<T*> GetComponentsInDescendantsAndThis() const;
    template <class T>
    void GetComponentsInDescendantsAndThis(Array<T*> *componentsOut) const;

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
    bool IsVisible(bool recursive) const;
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
    void PropagateToArray(TReturn TListenerInnerT::*func,
                          const Array<TListener*> &list,
                          const Args&... args);

    void PropagateToChildren(std::function<void(GameObject*)> func);
    void PropagateToComponents(std::function<void(Component*)> func);

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
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

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
    Array<GameObject*> m_children;
    Array<Component*> m_components;

    String m_name = "";
    bool m_visible = true;
    bool m_dontDestroyOnLoad = false;

    GameObject* p_parent = nullptr;

    // Convencience cached components
    Transform *p_transform = nullptr;
    RectTransform *p_rectTransform = nullptr;

    // Concurrent iteration
    int m_childrenIterationDepth;
    int m_componentsIterationDepth;
    void TryToClearDeletedChildren();
    void TryToClearDeletedComponents();

    void AddChild(GameObject *child, int index);
    void RemoveChild(GameObject *child);

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
