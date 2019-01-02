#include "Bang/GameObject.h"

#include <iostream>

#include "Bang/AABox.h"
#include "Bang/Assert.h"
#include "Bang/Camera.h"
#include "Bang/ClassDB.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Debug.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/HideFlags.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsComponent.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsGameObjectVisibilityChanged.h"
#include "Bang/IEventsName.h"
#include "Bang/IEventsObject.h"
#include "Bang/Material.h"
#include "Bang/Math.h"
#include "Bang/Matrix4.tcc"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/RectTransform.h"
#include "Bang/Renderer.h"
#include "Bang/Scene.h"
#include "Bang/SceneManager.h"
#include "Bang/Sphere.h"
#include "Bang/StreamOperators.h"
#include "Bang/Transform.h"
#include "Bang/USet.h"
#include "Bang/USet.tcc"

namespace Bang
{
class ICloneable;
}

using namespace Bang;

GameObject::GameObject(const String &name)
{
    SET_INSTANCE_CLASS_ID(GameObject);
    SetName(name);
}

GameObject::~GameObject()
{
    ASSERT(GetChildren().IsEmpty());
    ASSERT(GetComponents().IsEmpty());
    ASSERT(IsWaitingToBeDestroyed());
    ASSERT(IsBeingDestroyed());
    ASSERT(GetParent() == nullptr);
    ASSERT(GetDestroyEventHasBeenPropagated());
}

void GameObject::PreStart()
{
    PropagateToChildren(&GameObject::PreStart);
    PropagateToComponents(&Component::PreStart);
    Object::PreStart();
}

void GameObject::Start()
{
    PropagateToChildren(&GameObject::Start);
    PropagateToComponents(&Component::Start);
    Object::Start();
}

void GameObject::Update()
{
    if (IsActiveRecursively())
    {
        PropagateToComponents(&Component::Update);
        PropagateToChildren(&GameObject::Update);
        AfterChildrenUpdate();
    }
}

void GameObject::AfterChildrenUpdate()
{
    if (IsActiveRecursively())
    {
        PropagateToComponents(&Component::OnAfterChildrenUpdate);
    }
}

void GameObject::PostUpdate()
{
    if (IsActiveRecursively())
    {
        PropagateToComponents(&Component::OnPostUpdate);
        PropagateToChildren(&GameObject::PostUpdate);
    }
}

void GameObject::BeforeRender()
{
    if (IsActiveRecursively())
    {
        PropagateToComponents(&Component::OnBeforeRender);
        PropagateToChildren(&GameObject::BeforeRender);
    }
}

void GameObject::Render(RenderPass renderPass, bool renderChildren)
{
    if (IsActiveRecursively() && IsVisibleRecursively())
    {
        PropagateToComponents(&Component::OnRender, renderPass);
        if (renderChildren)
        {
            BeforeChildrenRender(renderPass);
            PropagateToChildren(&GameObject::Render, renderPass, true);
            AfterChildrenRender(renderPass);
        }
    }
}

void GameObject::BeforeChildrenRender(RenderPass renderPass)
{
    if (IsActiveRecursively())
    {
        PropagateToComponents(&Component::OnBeforeChildrenRender, renderPass);
    }
}

void GameObject::AfterChildrenRender(RenderPass renderPass)
{
    if (IsActiveRecursively())
    {
        PropagateToComponents(&Component::OnAfterChildrenRender, renderPass);
    }
}

void GameObject::ChildAdded(GameObject *addedChild, GameObject *parent)
{
    EventEmitter<IEventsChildren>::PropagateToListeners(
        &IEventsChildren::OnChildAdded, addedChild, parent);
    PropagateToArray(&IEventsChildren::OnChildAdded,
                     GetComponents<EventListener<IEventsChildren>>(),
                     addedChild,
                     parent);
}

void GameObject::ChildRemoved(GameObject *removedChild, GameObject *parent)
{
    EventEmitter<IEventsChildren>::PropagateToListeners(
        &IEventsChildren::OnChildRemoved, removedChild, parent);
    PropagateToArray(&IEventsChildren::OnChildRemoved,
                     GetComponents<EventListener<IEventsChildren>>(),
                     removedChild,
                     parent);
}

Component *GameObject::AddComponent(Component *component, int index)
{
    m_componentsToAdd.PushBack(std::make_pair(component, index));
    TryToAddQueuedComponents();
    return component;
}

void GameObject::AddChild(GameObject *child, int index, bool keepWorldTransform)
{
    ChildToAdd childToAdd;
    childToAdd.child = child;
    childToAdd.index = index;
    childToAdd.keepWorldTransform = keepWorldTransform;
    m_childrenToAdd.PushBack(childToAdd);
    TryToAddQueuedChildren();
}

void GameObject::AddChild_(GameObject *child,
                           int index_,
                           bool keepWorldTransform)
{
    ASSERT(child);
    ASSERT(child != this);

    int index = (index_ != -1 ? index_ : GetChildren().Size());
    ASSERT(index >= 0 && index <= GetChildren().Size());
    if (child->GetParent() != this)  // Parent change
    {
        Matrix4 prevWorldTransform = Matrix4::Identity();
        if (keepWorldTransform && child->GetTransform())
        {
            prevWorldTransform = child->GetTransform()->GetLocalToWorldMatrix();
        }

        GameObject *oldParent = child->GetParent();

        child->p_parent = this;
        child->InvalidateEnabledRecursively();
        child->InvalidateVisibleRecursively();

        if (keepWorldTransform && GetTransform() && child->GetTransform())
        {
            child->GetTransform()->FillFromMatrix(
                GetTransform()->GetWorldToLocalMatrix() * prevWorldTransform);
        }

        index = Math::Clamp(index, 0, m_children.Size());
        m_children.Insert(child, index);
        ChildAdded(child, this);

        EventEmitter<IEventsChildren>::PropagateToListeners(
            &IEventsChildren::OnParentChanged, oldParent, this);
        PropagateToArray(&EventListener<IEventsChildren>::OnParentChanged,
                         child->GetComponents<EventListener<IEventsChildren>>(),
                         oldParent,
                         this);
    }
    else  // Its a movement
    {
        int oldIndex = GetChildren().IndexOf(this);
        if (oldIndex != index)
        {
            if (oldIndex >= 0)
            {
                index = (oldIndex < index) ? (index - 1) : index;
            }

            m_children.Remove(child);

            index = Math::Clamp(index, 0, m_children.Size());
            m_children.Insert(child, index);
        }
    }
}

void GameObject::RemoveChild(GameObject *child)
{
    int i = m_children.IndexOf(child);
    if (i >= 0)
    {
        m_children[i] = nullptr;
        TryToClearDeletedChildren();
        ChildRemoved(child, this);
    }
}

void GameObject::InvalidateVisibleRecursively()
{
    if (m_visibleRecursivelyValid)
    {
        m_visibleRecursivelyValid = false;
        OnVisibleRecursivelyInvalidated();
    }
}

Component *GameObject::AddComponent_(Component *component, int index_)
{
    ASSERT(m_componentsIterationDepth == 0);
    if (component && !GetComponents().Contains(component))
    {
        Transform *transformComp = DCAST<Transform *>(component);
        if (transformComp)
        {
            ASSERT_SOFT_MSG(
                !HasComponent<Transform>(),
                "A GameObject can not have more than one transform");
        }

        const int index = (index_ != -1 ? index_ : GetComponents().Size());
        m_components.Insert(component, index);

        if (transformComp)
        {
            p_transform = transformComp;
            if (RectTransform *rt = DCAST<RectTransform *>(component))
            {
                p_rectTransform = rt;
            }
        }

        component->SetGameObject(this);

        EventEmitter<IEventsComponent>::PropagateToListeners(
            &IEventsComponent::OnComponentAdded, component, index);
    }
    return component;
}

bool GameObject::CalculateEnabledRecursively() const
{
    return IsEnabled() &&
           (GetParent() ? GetParent()->IsEnabledRecursively() : true);
}

bool GameObject::CalculateVisibleRecursively() const
{
    return IsVisible() &&
           (GetParent() ? GetParent()->IsVisibleRecursively() : true);
}

void GameObject::OnEnabledRecursivelyInvalidated()
{
    for (GameObject *child : GetChildren())
    {
        if (child)
        {
            child->InvalidateEnabledRecursively();
        }
    }

    for (Component *comp : GetComponents())
    {
        if (comp)
        {
            comp->InvalidateEnabledRecursively();
        }
    }
}

void GameObject::OnVisibleRecursivelyInvalidated()
{
    for (GameObject *child : GetChildren())
    {
        child->InvalidateVisibleRecursively();
    }
}

void GameObject::RemoveComponent(Component *component)
{
    int i = m_components.IndexOf(component);
    if (i >= 0)
    {
        m_components[i] = nullptr;

        EventEmitter<IEventsComponent>::PropagateToListeners(
            &IEventsComponent::OnComponentRemoved, component, this);

        if (component == p_transform)
        {
            p_transform = nullptr;
        }

        if (component == p_rectTransform)
        {
            p_rectTransform = nullptr;
        }
    }
}

void GameObject::OnEnabled(Object *object)
{
    Object::OnEnabled(object);
    PropagateToArray(&EventListener<IEventsObject>::OnEnabled,
                     GetComponents<EventListener<IEventsObject>>(),
                     object);
    PropagateToChildren(&EventListener<IEventsObject>::OnEnabled, object);
}
void GameObject::OnDisabled(Object *object)
{
    Object::OnDisabled(object);
    PropagateToArray(&EventListener<IEventsObject>::OnDisabled,
                     GetComponents<EventListener<IEventsObject>>(),
                     object);
    PropagateToChildren(&EventListener<IEventsObject>::OnDisabled, object);
}

void GameObject::TryToAddQueuedChildren()
{
    if (m_childrenIterationDepth == 0)
    {
        for (const auto &childToAddStruct : m_childrenToAdd)
        {
            GameObject *childToAdd = childToAddStruct.child;
            int idx = childToAddStruct.index;
            bool keepWorldTransform = childToAddStruct.keepWorldTransform;
            AddChild_(childToAdd, idx, keepWorldTransform);
        }
        m_childrenToAdd.Clear();
    }
}

void GameObject::TryToAddQueuedComponents()
{
    if (m_componentsIterationDepth == 0)
    {
        for (const auto &pair : m_componentsToAdd)
        {
            Component *compToAdd = pair.first;
            uint idx = pair.second;
            AddComponent_(compToAdd, idx);
        }
        m_componentsToAdd.Clear();
    }
}

void GameObject::TryToClearDeletedChildren()
{
    Array<GameObject *> newChildren;
    for (GameObject *child : m_children)
    {
        if (child)
        {
            newChildren.PushBack(child);
        }
    }
    m_children = newChildren;
}

void GameObject::TryToClearDeletedComponents()
{
    Array<Component *> newComponents;
    for (Component *comp : m_components)
    {
        if (comp)
        {
            newComponents.PushBack(comp);
        }
    }
    m_components = newComponents;
}

void GameObject::DestroyImmediate(GameObject *gameObject)
{
    ASSERT(gameObject);

    if (!gameObject->IsBeingDestroyed())
    {
        gameObject->SetWaitingToBeDestroyed();
        gameObject->SetBeingDestroyed();
        Object::PropagateObjectDestruction(gameObject);

        gameObject->DestroyDelayedGameObjects();
        gameObject->DestroyDelayedComponents();

        while (!gameObject->GetChildren().IsEmpty())
        {
            if (GameObject *child = gameObject->GetChildren().Back())
            {
                GameObject::DestroyImmediate(child);
            }
            else
            {
                gameObject->m_children.PopBack();
            }
        }

        while (!gameObject->GetComponents().IsEmpty())
        {
            if (Component *comp = gameObject->GetComponents().Back())
            {
                Component::DestroyImmediate(comp);
            }
            else
            {
                gameObject->m_components.PopBack();
            }
        }

        gameObject->SetParent(nullptr);
        delete gameObject;
    }
}

void GameObject::Destroy(GameObject *gameObject)
{
    if (!gameObject->IsWaitingToBeDestroyed())
    {
        gameObject->SetWaitingToBeDestroyed();
        Object::PropagateObjectDestruction(gameObject);
        if (GameObject *parent = gameObject->GetParent())
        {
            // Add this gameObject to parent for destroying delayed
            ASSERT(!parent->m_gameObjectsToDestroyDelayed.Contains(gameObject))
            parent->m_gameObjectsToDestroyDelayed.PushBack(gameObject);

            // Remove from parent
            gameObject->SetParent(nullptr);
        }
        else
        {
            GameObject::DestroyImmediate(gameObject);
        }
    }
}

const Array<Component *> &GameObject::GetComponents() const
{
    return m_components;
}

Component *GameObject::GetComponentByGUID(const GUID &guid) const
{
    for (Component *comp : GetComponents())
    {
        if (comp->GetGUID() == guid)
        {
            return comp;
        }
    }
    return nullptr;
}

Scene *GameObject::GetScene() const
{
    if (Scene *scene = DCAST<Scene *>(const_cast<GameObject *>(this)))
    {
        return scene;
    }

    GameObject *parent = GetParent();
    return parent ? parent->GetScene() : nullptr;
}

Transform *GameObject::GetTransform() const
{
    return p_transform;
}
RectTransform *GameObject::GetRectTransform() const
{
    return p_rectTransform;
}

void GameObject::SetName(const String &name)
{
    if (name != GetName())
    {
        String oldName = GetName();
        m_name = name;
        EventEmitter<IEventsName>::PropagateToListeners(
            &IEventsName::OnNameChanged, this, oldName, GetName());
    }
}

const String &GameObject::GetName() const
{
    return m_name;
}

GameObject *GameObject::Find(const String &name)
{
    Scene *scene = SceneManager::GetActiveScene();
    return scene ? scene->FindInChildren(name) : nullptr;
}

Object *GameObject::FindObjectInDescendants(const GUID &guid) const
{
    if (guid == GUID::Empty())
    {
        return nullptr;
    }

    if (GetGUID() == guid)
    {
        return const_cast<GameObject *>(this);
    }

    const Array<Component *> &comps = GetComponents();
    for (Component *comp : comps)
    {
        if (comp && comp->GetGUID() == guid)
        {
            return comp;
        }
    }

    const Array<GameObject *> &children = GetChildren();
    for (GameObject *child : children)
    {
        if (child)
        {
            if (Object *obj = child->FindObjectInDescendants(guid))
            {
                return obj;
            }
        }
    }
    return nullptr;
}

GameObject *GameObject::FindInChildren(const GUID &guid, bool recursive) const
{
    for (GameObject *child : GetChildren())
    {
        if (child)
        {
            if (GameObject *found =
                    child->FindInChildrenAndThis(guid, recursive))
            {
                return found;
            }
        }
    }
    return nullptr;
}

GameObject *GameObject::FindInChildren(const String &name, bool recursive) const
{
    for (GameObject *child : GetChildren())
    {
        if (child)
        {
            if (GameObject *found =
                    child->FindInChildrenAndThis(name, recursive))
            {
                return found;
            }
        }
    }
    return nullptr;
}

GameObject *GameObject::FindInChildrenAndThis(const GUID &guid,
                                              bool recursive) const
{
    if (GetGUID() == guid)
    {
        return const_cast<GameObject *>(this);
    }

    for (GameObject *child : GetChildren())
    {
        if (child && child->GetGUID() == guid)
        {
            return child;
        }
        else if (recursive)
        {
            if (GameObject *found = child->FindInChildren(guid, true))
            {
                return found;
            }
        }
    }
    return nullptr;
}

GameObject *GameObject::FindInChildrenAndThis(const String &name,
                                              bool recursive) const
{
    if (GetName() == name)
    {
        return const_cast<GameObject *>(this);
    }

    for (GameObject *child : GetChildren())
    {
        if (child && child->GetName() == name)
        {
            return child;
        }
        else if (recursive)
        {
            if (GameObject *found = child->FindInChildren(name, true))
            {
                return found;
            }
        }
    }
    return nullptr;
}

GameObject *GameObject::FindInAncestors(const String &name,
                                        bool broadSearch) const
{
    return GetParent() ? GetParent()->FindInAncestorsAndThis(name, broadSearch)
                       : nullptr;
}

GameObject *GameObject::FindInAncestorsAndThis(const String &name,
                                               bool broadSearch) const
{
    if (GetName() == name)
    {
        return const_cast<GameObject *>(this);
    }

    if (broadSearch)
    {
        if (GetParent())
        {
            for (GameObject *siblingOrThis : GetParent()->GetChildren())
            {
                if (siblingOrThis->GetName() == name)
                {
                    return const_cast<GameObject *>(this);
                }

                if (GameObject *found =
                        siblingOrThis->FindInAncestors(name, true))
                {
                    return found;
                }
            }
        }
    }

    return GetParent() ? GetParent()->FindInAncestorsAndThis(name, false)
                       : nullptr;
}

Object *GameObject::FindObjectInDescendants(ClassIdType classIdBegin,
                                            ClassIdType classIdEnd) const
{
    if (ClassDB::IsSubClass(classIdBegin, classIdEnd, this))
    {
        return const_cast<GameObject *>(this);
    }

    const Array<Component *> &components = GetComponents();
    for (Component *comp : components)
    {
        if (comp && ClassDB::IsSubClass(classIdBegin, classIdEnd, comp))
        {
            return comp;
        }
    }

    const Array<GameObject *> &children = GetChildren();
    for (GameObject *child : children)
    {
        if (child)
        {
            if (Object *obj =
                    child->FindObjectInDescendants(classIdBegin, classIdEnd))
            {
                return obj;
            }
        }
    }

    return nullptr;
}

void GameObject::SetVisible(bool visible)
{
    if (visible != IsVisible())
    {
        m_visible = visible;
        InvalidateVisibleRecursively();

        EventEmitter<IEventsGameObjectVisibilityChanged>::PropagateToListeners(
            &IEventsGameObjectVisibilityChanged::OnVisibilityChanged, this);
    }
}

GameObject *GameObject::GetChild(const GUID &guid) const
{
    for (GameObject *go : GetChildren())
    {
        if (go && go->GetGUID() == guid)
        {
            return go;
        }
    }
    return nullptr;
}

GameObject *GameObject::GetChild(const String &name) const
{
    for (GameObject *child : GetChildren())
    {
        if (child && child->GetName() == name)
        {
            return child;
        }
    }
    return nullptr;
}

const Array<GameObject *> &GameObject::GetChildren() const
{
    return m_children;
}

GameObject *GameObject::GetChild(uint index) const
{
    if (index >= GetChildren().Size())
    {
        return nullptr;
    }
    return GetChildren()[index];
}

void GameObject::GetChildrenRecursively(Array<GameObject *> *children) const
{
    for (GameObject *child : GetChildren())
    {
        if (child)
        {
            children->PushBack(child);
            child->GetChildrenRecursively(children);
        }
    }
}

Array<GameObject *> GameObject::GetChildrenRecursively() const
{
    Array<GameObject *> children;
    GetChildrenRecursively(&children);
    return children;
}

bool GameObject::IsChildOf(const GameObject *parent, bool recursive) const
{
    if (!GetParent())
    {
        return false;
    }

    if (recursive)
    {
        return IsChildOf(parent, false) || GetParent()->IsChildOf(parent, true);
    }
    return GetParent() == parent;
}

bool GameObject::IsVisible() const
{
    return m_visible;
}

bool GameObject::IsVisibleRecursively() const
{
    if (!m_visibleRecursivelyValid)
    {
        m_visibleRecursively = CalculateVisibleRecursively();
        m_visibleRecursivelyValid = true;
    }
    return m_visibleRecursively;
}

void GameObject::SetParent(GameObject *newParent,
                           int index,
                           bool keepWorldTransform)
{
    if (newParent != GetParent())  // Parent change
    {
        if (GetParent())
        {
            GetParent()->RemoveChild(this);
            p_parent = nullptr;

            InvalidateEnabledRecursively();
            InvalidateVisibleRecursively();
        }

        if (newParent)
        {
            newParent->AddChild(this, index, keepWorldTransform);
        }
    }
    else if (GetParent())  // Movement in same parent
    {
        ASSERT(newParent == GetParent());
        if (GetParent()->GetChildren().IndexOf(this) != index)
        {
            GetParent()->AddChild(this, index, keepWorldTransform);
        }
    }
}

GameObject *GameObject::GetParent() const
{
    return p_parent;
}

void GameObject::SetDontDestroyOnLoad(bool dontDestroyOnLoad)
{
    m_dontDestroyOnLoad = dontDestroyOnLoad;
}

int GameObject::GetIndexInsideParent() const
{
    if (GameObject *parent = GetParent())
    {
        return parent->GetChildren().IndexOf(const_cast<GameObject *>(this));
    }
    return -1;
}

bool GameObject::IsDontDestroyOnLoad() const
{
    return m_dontDestroyOnLoad;
}

AARect GameObject::GetBoundingViewportRect(Camera *cam,
                                           bool includeChildren) const
{
    AABox bbox = GetAABBoxWorld(includeChildren);
    return cam->GetViewportBoundingAARectNDC(bbox);
}

AABox GameObject::GetLocalAABBox(bool includeChildren) const
{
    AABox aabBox = AABox::Empty();
    if (IsEnabledRecursively() && IsVisibleRecursively())
    {
        Array<Renderer *> rends = GetComponents<Renderer>();
        for (Renderer *rend : rends)
        {
            if (rend && rend->IsEnabledRecursively() &&
                rend->GetActiveMaterial())
            {
                RenderPass rp = rend->GetActiveMaterial()
                                    ->GetShaderProgramProperties()
                                    .GetRenderPass();
                if (rp == RenderPass::SCENE_OPAQUE ||
                    rp == RenderPass::SCENE_TRANSPARENT)
                {
                    const AABox rendAABox = rend->GetAABBox();
                    aabBox = AABox::Union(aabBox, rendAABox);
                }
            }
        }

        if (includeChildren)
        {
            for (GameObject *child : GetChildren())
            {
                AABox aabBoxChild = child->GetLocalAABBox(true);
                if (aabBoxChild != AABox::Empty())
                {
                    Matrix4 mat;
                    const Transform *childT = child->GetTransform();
                    if (childT)
                    {
                        mat = childT->GetLocalToParentMatrix();
                    }
                    aabBoxChild = mat * aabBoxChild;
                    aabBox = AABox::Union(aabBox, aabBoxChild);
                }
            }
        }
    }
    return aabBox;
}

AABox GameObject::GetAABBoxWorld(bool includeChildren) const
{
    AABox b = GetLocalAABBox(includeChildren);
    if (b != AABox::Empty())
    {
        Matrix4 mat = Matrix4::Identity();
        if (GetTransform())
        {
            mat = GetTransform()->GetLocalToWorldMatrix();
        }
        b = mat * b;
    }
    return b;
}

Sphere GameObject::GetLocalBoundingSphere(bool includeChildren) const
{
    return Sphere::FromBox(GetLocalAABBox(includeChildren));
}

Sphere GameObject::GetBoundingSphere(bool includeChildren) const
{
    return Sphere::FromBox(GetAABBoxWorld(includeChildren));
}

void GameObject::PropagateToChildren(std::function<void(GameObject *)> func)
{
    ++m_childrenIterationDepth;

    const Array<GameObject *> &children = GetChildren();
    for (GameObject *child : children)
    {
        if (child && child->IsEnabledRecursively())
        {
            func(child);
        }
    }

    if (--m_childrenIterationDepth == 0)
    {
        TryToAddQueuedChildren();
        TryToClearDeletedChildren();
        DestroyDelayedGameObjects();
    }
}

void GameObject::PropagateToComponents(std::function<void(Component *)> func)
{
    ++m_componentsIterationDepth;

    const Array<Component *> &components = GetComponents();
    for (Component *comp : components)
    {
        if (comp && comp->IsEnabledRecursively())
        {
            func(comp);
        }
    }

    if (--m_componentsIterationDepth == 0)
    {
        TryToAddQueuedComponents();
        TryToClearDeletedComponents();
        DestroyDelayedComponents();
    }
}

GameObject *GameObject::Instantiate()
{
    GameObject *go = GameObjectFactory::CreateGameObject(true);
    go->SetParent(SceneManager::GetActiveScene());
    return go;
}

void GameObject::AddComponentToDestroyDelayed(Component *comp)
{
    if (!comp->IsWaitingToBeDestroyed())
    {
        ASSERT(!m_componentsToDestroyDelayed.Contains(comp));
        m_componentsToDestroyDelayed.PushBack(comp);
    }
}

void GameObject::DestroyDelayedGameObjects()
{
    while (!m_gameObjectsToDestroyDelayed.IsEmpty())
    {
        GameObject *go = m_gameObjectsToDestroyDelayed.Back();
        GameObject::DestroyImmediate(go);
        m_gameObjectsToDestroyDelayed.PopBack();
    }
}

void GameObject::DestroyDelayedComponents()
{
    while (!m_componentsToDestroyDelayed.IsEmpty())
    {
        Component *comp = m_componentsToDestroyDelayed.Back();
        Component::DestroyImmediate(comp);
        m_componentsToDestroyDelayed.PopBack();
    }
}

template <class T>
bool CanEventBePropagatedToGameObject(const GameObject *go)
{
    return go->IsEnabledRecursively() && go->T::IsReceivingEvents();
}

template <class T>
bool CanEventBePropagatedToComponent(const Component *comp)
{
    return comp->IsEnabledRecursively() && comp->T::IsReceivingEvents();
}

void GameObject::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Object::CloneInto(clone, cloneGUID);

    GameObject *go = SCAST<GameObject *>(clone);
    go->SetName(m_name);
    go->SetParent(nullptr);

    for (GameObject *child : GetChildren())
    {
        if (child && !child->GetHideFlags().IsOn(HideFlag::DONT_CLONE))
        {
            GameObject *childClone = child->Clone(cloneGUID);
            childClone->SetParent(go);
        }
    }

    for (Component *comp : GetComponents())
    {
        if (comp && !comp->GetHideFlags().IsOn(HideFlag::DONT_CLONE))
        {
            go->AddComponent(comp->Clone(cloneGUID));
        }
    }
}

void GameObject::Print(const String &indent) const
{
    String indent2 = indent;
    indent2 += "   ";
    Debug_Log(indent << this);
    for (GameObject *child : GetChildren())
    {
        if (child)
        {
            child->Print(indent2);
        }
    }
}

String GameObject::ToStringStructure(bool recursive, const String &indent)
{
    std::ostringstream oss;
    oss << indent << " - " << GetName() << " " << GetComponents();
    if (recursive)
    {
        oss << "\n";
        for (GameObject *child : GetChildren())
        {
            if (child)
            {
                oss << child->ToStringStructure(true, indent + "  ");
            }
        }
    }
    return String(oss.str());
}

String GameObject::ToString() const
{
    std::ostringstream oss;
    oss << "GameObject: " << GetName() << "(" << RCAST<const void *>(this)
        << ")";
    return oss.str();
}

void GameObject::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if (metaNode.Contains("Enabled"))
    {
        SetEnabled(metaNode.Get<bool>("Enabled"));
    }

    if (metaNode.Contains("Visible"))
    {
        SetVisible(metaNode.Get<bool>("Visible"));
    }

    if (metaNode.Contains("Name"))
    {
        SetName(metaNode.Get<String>("Name"));
    }

    if (metaNode.Contains("DontDestroyOnLoad"))
    {
        SetDontDestroyOnLoad(metaNode.Get<bool>("DontDestroyOnLoad"));
    }

    // Read components
    {
        USet<Component *> componentsToRemove;
        for (Component *comp : GetComponents())
        {
            componentsToRemove.Add(comp);
        }

        const Array<MetaNode> &componentsMetaNodes =
            metaNode.GetChildren("Components");
        for (const MetaNode &childMeta : componentsMetaNodes)
        {
            const GUID guid = childMeta.Get<GUID>("GUID");
            const String &tagName = childMeta.GetName();

            Component *comp = nullptr;
            for (Component *containedComp : GetComponents())
            {
                // See if it is already contained
                if (containedComp && containedComp->GetGUID() == guid)
                {
                    comp = containedComp;
                    componentsToRemove.Remove(comp);
                    break;
                }
            }

            if (!comp)
            {
                comp = ClassDB::Create<Component>(tagName);
                AddComponent(comp);
            }
            else
            {
                RemoveComponent(comp);  // To reorder
                AddComponent(comp);
            }
            comp->ImportMeta(childMeta);
        }

        // Remove non existing components in meta
        for (Component *compToRemove : componentsToRemove)
        {
            if (compToRemove)
            {
                Component::DestroyImmediate(compToRemove);
            }
        }
    }

    // Read children
    {
        USet<GameObject *> childrenToRemove;
        for (GameObject *child : GetChildren())
        {
            childrenToRemove.Add(child);
        }

        const Array<MetaNode> &childreMetaNodes =
            metaNode.GetChildren("GameObjectChildren");
        for (const MetaNode &childMeta : childreMetaNodes)
        {
            const GUID guid = childMeta.Get<GUID>("GUID");
            const String &tagName = childMeta.GetName();
            ASSERT(tagName == GameObject::GetClassNameStatic());

            GameObject *child = nullptr;
            for (GameObject *containedChild : GetChildren())
            {
                // See if it is already contained
                if (containedChild && containedChild->GetGUID() == guid)
                {
                    child = containedChild;
                    childrenToRemove.Remove(child);
                    break;
                }
            }

            if (!child)
            {
                child = GameObjectFactory::CreateGameObject(false);
            }
            else
            {
                child->SetParent(nullptr);  // To reorder
            }
            child->SetParent(this);
            child->ImportMeta(childMeta);
        }

        // Remove non existing gameObjects in meta
        for (GameObject *childToRemove : childrenToRemove)
        {
            if (childToRemove)
            {
                GameObject::DestroyImmediate(childToRemove);
            }
        }
    }
}

void GameObject::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("Enabled", IsEnabled());
    metaNode->Set("Visible", IsVisible());
    metaNode->Set("Name", GetName());
    metaNode->Set("DontDestroyOnLoad", IsDontDestroyOnLoad());

    for (Component *c : GetComponents())
    {
        if (c && c->GetHideFlags().IsOff(HideFlag::DONT_SERIALIZE))
        {
            MetaNode compMeta;
            c->ExportMeta(&compMeta);
            metaNode->AddChild(compMeta, "Components");
        }
    }

    for (GameObject *child : GetChildren())
    {
        if (child && child->GetHideFlags().IsOff(HideFlag::DONT_SERIALIZE))
        {
            MetaNode childMeta;
            child->ExportMeta(&childMeta);
            metaNode->AddChild(childMeta, "GameObjectChildren");
        }
    }
}
