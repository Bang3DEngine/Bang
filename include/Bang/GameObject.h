#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <functional>
#include <utility>
#include <vector>

#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsComponent.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsGameObjectVisibilityChanged.h"
#include "Bang/IEventsName.h"
#include "Bang/IToString.h"
#include "Bang/MetaNode.h"
#include "Bang/Object.h"
#include "Bang/RenderPass.h"
#include "Bang/Serializable.h"
#include "Bang/Sphere.h"
#include "Bang/String.h"

namespace Bang
{
class Camera;
class Component;
class GUID;
class Serializable;
class RectTransform;
class Scene;
class Transform;

#define GAMEOBJECT_(ClassName) OBJECT(ClassName)

#define GAMEOBJECT_ABSTRACT_WITHOUT_CLASS_ID(ClassName) \
    OBJECT_ABSTRACT_WITHOUT_CLASS_ID(ClassName)         \
    friend class GameObject

#define GAMEOBJECT_WITHOUT_CLASS_ID(ClassName) \
    OBJECT_WITHOUT_CLASS_ID(ClassName)         \
    friend class GameObject

#define GAMEOBJECT_ABSTRACT(ClassName) \
    OBJECT_ABSTRACT(ClassName)         \
    friend class GameObject

#define GAMEOBJECT(ClassName) \
    OBJECT(ClassName)         \
    friend class GameObject

class GameObject : public Object,
                   public IToString,
                   public EventListener<IEventsChildren>,
                   public EventEmitter<IEventsName>,
                   public EventEmitter<IEventsChildren>,
                   public EventEmitter<IEventsComponent>,
                   public EventEmitter<IEventsGameObjectVisibilityChanged>
{
    GAMEOBJECT_(GameObject)

public:
    GameObject(const String &name = "GameObject");

    virtual void PreStart() override;
    virtual void Start() override;
    virtual void Update();
    virtual void Render(RenderPass renderPass, bool renderChildren = true);

    void SetName(const String &m_name);

    template <class T>
    T *AddComponent(int index = -1);
    Component *AddComponent(Component *c, int index = -1);

    template <class T>
    bool HasComponent() const;
    void RemoveComponent(Component *component);

    static void Destroy(GameObject *gameObject);
    static void DestroyImmediate(GameObject *gameObject);

    static GameObject *Find(const String &name);
    Object *GetObjectInDescendantsAndThis(const GUID &guid) const;
    GameObject *FindInChildren(const GUID &guid, bool recursive = true) const;
    GameObject *FindInChildren(const String &name, bool recursive = true) const;
    GameObject *FindInChildrenAndThis(const GUID &guid,
                                      bool recursive = true) const;
    GameObject *FindInChildrenAndThis(const String &name,
                                      bool recursive = true) const;
    GameObject *FindInAncestors(const String &name, bool broadSearch) const;
    GameObject *FindInAncestorsAndThis(const String &name,
                                       bool broadSearch) const;

    void SetVisible(bool visible);
    void SetParent(GameObject *newParent,
                   int index = -1,
                   bool keepWorldTransform = false);
    void SetDontDestroyOnLoad(bool dontDestroyOnLoad);

    // GetGameObject functions
    int GetIndexInsideParent() const;
    GameObject *GetChild(uint index) const;
    GameObject *GetChild(const GUID &guid) const;
    GameObject *GetChild(const String &name) const;
    const Array<GameObject *> &GetChildren() const;
    Array<GameObject *> GetAscendants() const;
    Array<GameObject *> GetDescendants() const;
    void GetDescendants(Array<GameObject *> *descendants) const;

    // GetComponent functions
    Component *GetComponentByGUID(const GUID &guid) const;
    const Array<Component *> &GetComponents() const;
    template <class T>
    T *GetComponent() const;
    template <class T>
    T *GetComponentInParent() const;
    template <class T>
    T *GetComponentInParentAndThis() const;
    template <class T>
    T *GetComponentInAncestors() const;
    template <class T>
    T *GetComponentInAncestorsAndThis() const;
    template <class T>
    T *GetComponentInChildren() const;
    template <class T>
    T *GetComponentInChildrenAndThis() const;
    template <class T>
    T *GetComponentInDescendants() const;
    template <class T>
    T *GetComponentInDescendantsAndThis() const;
    template <class T>
    Array<T *> GetComponents() const;
    template <class T>
    void GetComponents(Array<T *> *componentsOut) const;
    template <class T>
    Array<T *> GetComponentsInParent() const;
    template <class T>
    void GetComponentsInParent(Array<T *> *componentsOut) const;
    template <class T>
    Array<T *> GetComponentsInParentAndThis() const;
    template <class T>
    void GetComponentsInParentAndThis(Array<T *> *componentsOut) const;
    template <class T>
    Array<T *> GetComponentsInAncestors() const;
    template <class T>
    void GetComponentsInAncestors(Array<T *> *componentsOut) const;
    template <class T>
    Array<T *> GetComponentsInAncestorsAndThis() const;
    template <class T>
    void GetComponentsInAncestorsAndThis(Array<T *> *componentsOut) const;
    template <class T>
    Array<T *> GetComponentsInChildren() const;
    template <class T>
    void GetComponentsInChildren(Array<T *> *componentsOut) const;
    template <class T>
    Array<T *> GetComponentsInChildrenAndThis() const;
    template <class T>
    void GetComponentsInChildrenAndThis(Array<T *> *componentsOut) const;
    template <class T>
    Array<T *> GetComponentsInDescendants() const;
    template <class T>
    void GetComponentsInDescendants(Array<T *> *componentsOut) const;
    template <class T>
    Array<T *> GetComponentsInDescendantsAndThis() const;
    template <class T>
    void GetComponentsInDescendantsAndThis(Array<T *> *componentsOut) const;

    // GetObject functions
    template <class T>
    T *GetObject() const;
    template <class T>
    Array<T *> GetObjects() const;
    template <class T>
    T *GetObjectInChildren() const;
    template <class T>
    T *GetObjectInChildrenAndThis() const;
    template <class T>
    Array<T *> GetObjectsInChildren() const;
    template <class T>
    Array<T *> GetObjectsInChildrenAndThis() const;
    template <class T>
    T *GetObjectInDescendants() const;
    template <class T>
    T *GetObjectInDescendantsAndThis() const;
    template <class T>
    Array<T *> GetObjectsInDescendants() const;
    template <class T>
    Array<T *> GetObjectsInDescendantsAndThis() const;
    template <class T>
    T *GetObjectInAscendants() const;
    template <class T>
    T *GetObjectInAscendantsAndThis() const;
    template <class T>
    Array<T *> GetObjectsInAscendants() const;
    template <class T>
    Array<T *> GetObjectsInAscendantsAndThis() const;
    Object *GetObjectInDescendantsAndThis(ClassIdType classIdBegin,
                                          ClassIdType classIdEnd) const;

    Scene *GetScene() const;
    const String &GetName() const;
    Transform *GetTransform() const;
    RectTransform *GetRectTransform() const;
    GameObject *GetParent() const;
    bool IsChildOf(const GameObject *_parent, bool recursive = true) const;

    bool IsVisible() const;
    bool IsVisibleRecursively() const;
    bool IsDontDestroyOnLoad() const;
    AARect GetBoundingViewportRect(Camera *cam,
                                   bool includeChildren = true) const;
    AABox GetAABBoxWorld(bool includeChildren = true) const;
    Sphere GetBoundingSphereWorld(bool includeChildren = true) const;

    // Helper propagate functions
    void PropagateToChildren(std::function<void(GameObject *)> func);
    void PropagateToComponents(std::function<void(Component *)> func);

    template <class T, class TReturn, class... Args>
    void PropagateToChildren(TReturn T::*func, const Args &... args);

    template <class T, class TReturn, class... Args>
    void PropagateToComponents(TReturn T::*func, const Args &... args);

    static GameObject *Instantiate();

    // Serializable
    virtual void CloneInto(Serializable *clone, bool cloneGUID) const override;

    // IToString
    void Print(const String &indent = "") const;
    String ToStringStructure(bool recursive = false, const String &indent = "");
    String ToString() const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    virtual ~GameObject() override;

    virtual void AfterChildrenUpdate();
    virtual void PostUpdate();
    virtual void BeforeRender();
    virtual void BeforeChildrenRender(RenderPass renderPass);

    // IEventsObject
    virtual void OnEnabled(Object *object) override;
    virtual void OnDisabled(Object *object) override;

private:
    Array<GameObject *> m_children;
    Array<Component *> m_components;

    String m_name = "";
    bool m_dontDestroyOnLoad = false;

    bool m_visible = true;
    mutable bool m_visibleRecursively = false;
    mutable bool m_visibleRecursivelyValid = false;

    GameObject *p_parent = nullptr;

    // Convencience cached components
    Transform *p_transform = nullptr;
    RectTransform *p_rectTransform = nullptr;

    // Concurrent iteration
    struct ChildToAdd
    {
        GameObject *child;
        int index;
        bool keepWorldTransform;
    };
    int m_childrenIterationDepth = 0;
    int m_componentsIterationDepth = 0;
    Array<ChildToAdd> m_childrenToAdd;
    Array<std::pair<Component *, int>> m_componentsToAdd;

    Array<GameObject *> m_gameObjectsToDestroyDelayed;
    Array<Component *> m_componentsToDestroyDelayed;

    void TryToAddQueuedChildren();
    void TryToAddQueuedComponents();
    void TryToClearDeletedChildren();
    void TryToClearDeletedComponents();

    void AddComponentToDestroyDelayed(Component *comp);
    void DestroyDelayedGameObjects();
    void DestroyDelayedComponents();

    void AddChild(GameObject *child, int index, bool keepWorldTransform);
    void AddChild_(GameObject *child, int index, bool keepWorldTransform);
    void RemoveChild(GameObject *child);

    Component *AddComponent_(Component *c, int index);

    // Object
    bool CalculateEnabledRecursively() const override;
    bool CalculateVisibleRecursively() const;
    void OnEnabledRecursivelyInvalidated() override;
    void OnVisibleRecursivelyInvalidated();
    void InvalidateVisibleRecursively();

    friend class Scene;
    friend class Prefab;
    friend class GEngine;
    friend class Component;
    friend class SceneManager;
    friend class RectTransform;
};
}  // namespace Bang

#include "Bang/GameObject.tcc"

#endif  // GAMEOBJECT_H
