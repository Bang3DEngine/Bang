#include "Bang/GameObject.h"

#include <iostream>

#include "Bang/GL.h"
#include "Bang/AABox.h"
#include "Bang/Debug.h"
#include "Bang/Scene.h"
#include "Bang/Sphere.h"
#include "Bang/Camera.h"
#include "Bang/MetaNode.h"
#include "Bang/Renderer.h"
#include "Bang/Material.h"
#include "Bang/Component.h"
#include "Bang/Transform.h"
#include "Bang/SceneManager.h"
#include "Bang/RectTransform.h"
#include "Bang/IEventsObject.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

GameObject::GameObject(const String &name)
{
    SetName(name);
}

GameObject::~GameObject()
{
    ASSERT(GetChildren().IsEmpty());
    ASSERT(GetComponents().IsEmpty());
    ASSERT(IsWaitingToBeDestroyed());
    SetParent(nullptr);
    ASSERT(GetParent() == nullptr);
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

void GameObject::PreUpdate()
{
    if (IsActive())
    {
        PropagateToComponents(&Component::PreUpdate);
        PropagateToChildren(&GameObject::PreUpdate);
    }
}

void GameObject::BeforeChildrenUpdate()
{
    if (IsActive())
    {
        PropagateToComponents(&Component::BeforeChildrenUpdate);
    }
}

void GameObject::Update()
{
    if (IsActive())
    {
        PropagateToComponents(&Component::Update);
        BeforeChildrenUpdate();
        PropagateToChildren(&GameObject::Update);
        AfterChildrenUpdate();
    }
}

void GameObject::AfterChildrenUpdate()
{
    if (IsActive())
    {
        PropagateToComponents(&Component::OnAfterChildrenUpdate);
    }
}

void GameObject::PostUpdate()
{
    if (IsActive())
    {
        PropagateToComponents(&Component::OnPostUpdate);
        PropagateToChildren(&GameObject::PostUpdate);
    }
}

void GameObject::BeforeRender()
{
    if (IsActive())
    {
        PropagateToComponents(&Component::OnBeforeRender);
        PropagateToChildren(&GameObject::BeforeRender);
    }
}

void GameObject::Render(RenderPass renderPass, bool renderChildren)
{
    if (IsActive() && IsVisible())
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
    if (IsActive())
    {
        PropagateToComponents(&Component::OnBeforeChildrenRender, renderPass);
    }
}

void GameObject::AfterChildrenRender(RenderPass renderPass)
{
    if (IsActive())
    {
        PropagateToComponents(&Component::OnAfterChildrenRender, renderPass);
    }
}

void GameObject::ChildAdded(GameObject *addedChild, GameObject *parent)
{
    EventEmitter<IEventsChildren>::
          PropagateToListeners(&IEventsChildren::OnChildAdded,
                               addedChild, parent);
    PropagateToArray(&EventListener<IEventsChildren>::OnChildAdded,
                    GetComponents< EventListener<IEventsChildren> >(),
                    addedChild, parent);
}

void GameObject::ChildRemoved(GameObject *removedChild, GameObject *parent)
{
    EventEmitter<IEventsChildren>::
          PropagateToListeners(&EventListener<IEventsChildren>::OnChildRemoved,
                               removedChild, parent);
    PropagateToArray(&EventListener<IEventsChildren>::OnChildRemoved,
                    GetComponents< EventListener<IEventsChildren> >(),
                    removedChild, parent);
}

Component *GameObject::AddComponent(const String &componentClassName,
                                    int _index)
{
    Component *component = ComponentFactory::Create(componentClassName);
    return AddComponent(component, _index);
}

Component* GameObject::AddComponent(Component *component, int _index)
{
    if (component && !GetComponents().Contains(component))
    {
        Transform *transformComp = DCAST<Transform*>(component);
        if (transformComp)
        {
            ASSERT_SOFT_MSG(!HasComponent<Transform>(),
                            "A GameObject can not have more than one transform");
        }

        const int index = (_index != -1 ? _index : GetComponents().Size());
        m_components.Insert(component, index);
        UpdateComponentIndicesForAdd(index);

        if (transformComp)
        {
            p_transform = transformComp;
        }

        component->SetGameObject(this);

        EventEmitter<IEventsComponent>::PropagateToListeners(
                    &IEventsComponent::OnComponentAdded, component, index);
    }
    return component;
}

void GameObject::AddChild(GameObject *child, int index)
{
    ASSERT(!GetChildren().Contains(child));

    m_children.Insert(child, index);
    UpdateChildrenIndicesForAdd(index);
    ChildAdded(child, this);
}

void GameObject::RemoveChild(GameObject *child)
{
    int i = m_children.IndexOf(child);
    if (i >= 0)
    {
        m_children.RemoveByIndex(i);
        UpdateChildrenIndicesForRemove(i);
        ChildRemoved(child, this);
    }
}

void GameObject::RemoveComponent(Component *component)
{
    int i = m_components.IndexOf(component);
    if (i >= 0)
    {
        m_components.RemoveByIndex(i);
        UpdateComponentIndicesForRemove(i);

        EventEmitter<IEventsComponent>::PropagateToListeners(
                    &IEventsComponent::OnComponentRemoved, component, this);

        if (component == p_transform)
        {
            p_transform = nullptr;
        }
    }
}

void GameObject::OnEnabled(Object *object)
{
    Object::OnEnabled(object);
    PropagateToArray(&EventListener<IObjectEvents>::OnEnabled,
                    GetComponents<EventListener<IObjectEvents>>(), object);
    PropagateToChildren(&EventListener<IObjectEvents>::OnEnabled, object);
}
void GameObject::OnDisabled(Object *object)
{
    Object::OnDisabled(object);
    PropagateToArray(&EventListener<IObjectEvents>::OnDisabled,
                    GetComponents<EventListener<IObjectEvents>>(), object);
    PropagateToChildren(&EventListener<IObjectEvents>::OnDisabled, object);
}

void GameObject::UpdateChildrenIndicesForAdd(int addIndex)
{
    UpdateIndicesForAdd(m_childrenIterationIndices, addIndex);
}

void GameObject::UpdateChildrenIndicesForRemove(int removeIndex)
{
    UpdateIndicesForRemove(m_childrenIterationIndices, removeIndex);
}

void GameObject::UpdateComponentIndicesForAdd(int addIndex)
{
    UpdateIndicesForAdd(m_componentIterationIndices, addIndex);
}

void GameObject::UpdateComponentIndicesForRemove(int removeIndex)
{
    UpdateIndicesForRemove(m_componentIterationIndices, removeIndex);
}

void GameObject::UpdateIndicesForAdd(Array<int> &iterationIndices, int addIndex)
{
    for (int &idx : iterationIndices)
    {
        if (addIndex <= idx)
        {
            ++idx;
        }
    }
}

void GameObject::UpdateIndicesForRemove(Array<int> &iterationIndices, int removeIndex)
{
    for (int &idx : iterationIndices)
    {
        if (removeIndex < idx)
        {
            --idx;
        }
    }
}

void GameObject::DestroyImmediate(GameObject *gameObject)
{
    ASSERT(gameObject);

    if (!gameObject->IsWaitingToBeDestroyed())
    {
        Object::PropagateObjectDestruction(gameObject);

        while (!gameObject->GetChildren().IsEmpty())
        {
            GameObject *go = gameObject->GetChildren().Back();
            ASSERT(!go->IsWaitingToBeDestroyed());
            GameObject::DestroyImmediate(go);
        }

        while (!gameObject->GetComponents().IsEmpty())
        {
            Component *comp = gameObject->GetComponents().Back();
            ASSERT(!comp->IsWaitingToBeDestroyed());
            Component::DestroyImmediate(comp);
        }

        delete gameObject;
    }
}

void GameObject::Destroy(GameObject *gameObject)
{
    if (Scene *scene = SceneManager::GetActiveScene())
    {
        scene->AddGameObjectToDestroyDelayed(gameObject);
        gameObject->SetParent(nullptr);
    }
    else
    {
        GameObject::DestroyImmediate(gameObject);
    }
}

bool GameObject::IsEnabled(bool recursive) const
{
    if (!recursive)
    {
        return Object::IsEnabled();
    }
    else
    {
        return IsEnabled(false) &&
               (!GetParent() || GetParent()->IsEnabled(true));
    }
}

const Array<Component *> &GameObject::GetComponents() const
{
    return m_components;
}

bool GameObject::HasComponent(const String &className) const
{
    for (Component *comp : GetComponents())
    {
        if (comp->GetClassName() == className)
        {
            return true;
        }
    }
    return false;
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

Scene* GameObject::GetScene() const
{
    if (Scene *scene = DCAST<Scene*>( const_cast<GameObject*>(this) ))
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
    return GetTransform() ? DCAST<RectTransform*>(GetTransform()) : nullptr;
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
const String& GameObject::GetName() const { return m_name; }

GameObject *GameObject::Find(const String &name)
{
    Scene *scene = SceneManager::GetActiveScene();
    return scene ? scene->FindInChildren(name) : nullptr;
}

GameObject *GameObject::FindInChildren(const GUID &guid, bool recursive)
{
    for (GameObject *child : GetChildren())
    {
        if (GameObject *found = child->FindInChildrenAndThis(guid, recursive))
        {
            return found;
        }
    }
    return nullptr;
}

GameObject *GameObject::FindInChildren(const String &name, bool recursive)
{
    for (GameObject *child : GetChildren())
    {
        if (GameObject *found = child->FindInChildrenAndThis(name, recursive))
        {
            return found;
        }
    }
    return nullptr;
}

GameObject *GameObject::FindInChildrenAndThis(const GUID &guid, bool recursive)
{
    if (GetGUID() == guid)
    {
        return this;
    }

    for (GameObject *child : GetChildren())
    {
        if (child->GetGUID() == guid)
        {
            return child;
        }
        else if (recursive)
        {
            GameObject *found = child->FindInChildren(guid, true);
            if (found)
            {
                return found;
            }
        }
    }
    return nullptr;
}

GameObject *GameObject::FindInChildrenAndThis(const String &name, bool recursive)
{
    if (GetName() == name)
    {
        return this;
    }

    for (GameObject *child : GetChildren())
    {
        if (child->GetName() == name)
        {
            return child;
        }
        else if (recursive)
        {
            GameObject *found = child->FindInChildren(name, true);
            if (found)
            {
                return found;
            }
        }
    }
    return nullptr;
}

GameObject *GameObject::FindInAncestors(const String &name, bool broadSearch)
{
    return GetParent() ? GetParent()->FindInAncestorsAndThis(name, broadSearch) :
                         nullptr;
}

GameObject *GameObject::FindInAncestorsAndThis(const String &name,
                                               bool broadSearch)
{
    if (GetName() == name)
    {
        return this;
    }

    if (broadSearch)
    {
        if (GetParent())
        {
            for (GameObject *siblingOrThis : GetParent()->GetChildren())
            {
                if (siblingOrThis->GetName() == name)
                {
                    return this;
                }

                if (GameObject *found = siblingOrThis->FindInAncestors(name, true))
                {
                    return found;
                }
            }
        }
    }

    return GetParent() ? GetParent()->FindInAncestorsAndThis(name, false) :
                         nullptr;
}

void GameObject::SetVisible(bool visible)
{
    if (visible != IsVisible())
    {
        m_visible = visible;
        EventEmitter<IEventsGameObjectVisibilityChanged>::PropagateToListeners(
            &IEventsGameObjectVisibilityChanged::OnVisibilityChanged, this);
    }
}

GameObject *GameObject::GetChild(const GUID &guid) const
{
    for (GameObject *go : GetChildren())
    {
        if (go->GetGUID() == guid)
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
        if (child->GetName() == name)
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

void GameObject::GetChildrenRecursively(Array<GameObject*> *children) const
{
    for (GameObject *child : GetChildren())
    {
        children->PushBack(child);
        child->GetChildrenRecursively(children);
    }
}

Array<GameObject*> GameObject::GetChildrenRecursively() const
{
    Array<GameObject*> children;
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
        return IsChildOf(parent, false) ||
               GetParent()->IsChildOf(parent, true);
    }
    return GetParent() == parent;
}

bool GameObject::IsVisible() const
{
    return m_visible;
}

void GameObject::SetParent(GameObject *newParent,
                           int index_,
                           bool keepWorldTransform)
{
    ASSERT( newParent != this );
    ASSERT( !newParent || !newParent->IsChildOf(this, true) );

    int index = newParent ? (index_ != -1 ? index_ :
                              newParent->GetChildren().Size()) : -1;
    if (newParent != GetParent())
    {
        if (newParent && newParent->IsWaitingToBeDestroyed())
        {
            Debug_Warn("Trying to set as parent a destroyed object. "
                       "Not setting parent...");
            return;
        }

        Matrix4 prevWorldTransform = Matrix4::Identity;
        if (keepWorldTransform && GetTransform())
        {
            prevWorldTransform = GetTransform()->GetLocalToWorldMatrix();
        }

        GameObject *oldParent = GetParent();
        if (oldParent)
        {
            oldParent->RemoveChild(this);
        }

        p_parent = newParent;
        if (newParent)
        {
            if (keepWorldTransform && newParent->GetTransform())
            {
                GetTransform()->FillFromMatrix(newParent->GetTransform()->
                                               GetLocalToWorldMatrixInv() *
                                               prevWorldTransform);
            }
            newParent->AddChild(this, index);
        }

        EventEmitter<IEventsChildren>::
               PropagateToListeners(&IEventsChildren::OnParentChanged,
                                    oldParent, newParent);
        PropagateToArray(&EventListener<IEventsChildren>::OnParentChanged,
                        GetComponents<EventListener<IEventsChildren>>(),
                        oldParent,
                        newParent);
    }
    else if (GetParent())
    {
        // Is it a movement inside the same parent ?
        int oldIndex = GetParent()->GetChildren().IndexOf(this);
        if (oldIndex != index)
        {
            GetParent()->RemoveChild(this);
            GetParent()->AddChild(this, (oldIndex < index) ? (index-1) : index);
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
        return parent->GetChildren().IndexOf(const_cast<GameObject*>(this));
    }
    return -1;
}

bool GameObject::IsDontDestroyOnLoad() const
{
    return m_dontDestroyOnLoad;
}

AARect GameObject::GetBoundingViewportRect(Camera *cam, bool includeChildren) const
{
    AABox bbox = GetAABBoxWorld(includeChildren);
    return cam->GetViewportBoundingAARectNDC(bbox);
}

AABox GameObject::GetLocalAABBox(bool includeChildren) const
{
    Array<Renderer*> rends = GetComponents<Renderer>();
    AABox aabBox = AABox::Empty;
    for (Renderer *rend : rends)
    {
        if (rend && rend->IsEnabled() && rend->GetActiveMaterial())
        {
            RenderPass rp = rend->GetActiveMaterial()->GetRenderPass();
            if (rp == RenderPass::SCENE || rp == RenderPass::SCENE_TRANSPARENT)
            {
                const AABox rendAABox = rend->GetAABBox();
                if (rendAABox != AABox::Empty)
                {
                    aabBox = AABox::Union(aabBox, rendAABox);
                }
            }
        }
    }

    if (includeChildren)
    {
        for (GameObject *child : GetChildren())
        {
            AABox aabBoxChild = child->GetLocalAABBox(true);
            if (aabBoxChild != AABox::Empty)
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

    return aabBox;
}

AABox GameObject::GetAABBoxWorld(bool includeChildren) const
{
    AABox b = GetLocalAABBox(includeChildren);
    if (b == AABox::Empty) { return AABox::Empty; }

    Matrix4 mat = Matrix4::Identity;
    if (GetTransform()) { mat = GetTransform()->GetLocalToWorldMatrix(); }
    b = mat * b;
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

void GameObject::PropagateToChildren(std::function<void(GameObject*)> func)
{
    m_childrenIterationIndices.PushBack(0);
    for (int &i = m_childrenIterationIndices.Back();
         i < GetChildren().Size();
         ++i)
    {
        GameObject *child = GetChildren()[i];
        if (child->IsEnabled())
        {
            func(child);
        }
    }
    m_childrenIterationIndices.PopBack();
}

void GameObject::PropagateToComponents(std::function<void(Component*)> func)
{
    m_componentIterationIndices.PushBack(0);
    for (int &i = m_componentIterationIndices.Back();
         i < GetComponents().Size();
         ++i)
    {
        Component *comp = GetComponents()[i];
        if (comp->IsEnabled())
        {
            func(comp);
        }
    }
    m_componentIterationIndices.PopBack();
}

template<class T>
bool CanEventBePropagatedToGameObject(const GameObject *go)
{
    return go->IsEnabled() && go->T::IsReceivingEvents();
}

template<class T>
bool CanEventBePropagatedToComponent(const Component *comp)
{
    return comp->IsEnabled() && comp->T::IsReceivingEvents();
}

void GameObject::CloneInto(ICloneable *clone) const
{
    Object::CloneInto(clone);

    GameObject *go = DCAST<GameObject*>(clone);
    go->SetName(m_name);
    go->SetParent(nullptr);

    for (GameObject *child : GetChildren())
    {
        if (!child->GetHideFlags().IsOn(HideFlag::DONT_CLONE))
        {
            GameObject *childClone = child->Clone();
            childClone->SetParent(go);
        }
    }

    for (Component *comp : GetComponents())
    {
        if (!comp->GetHideFlags().IsOn(HideFlag::DONT_CLONE))
        {
            go->AddComponent(comp->Clone());
        }
    }
}

void GameObject::Print(const String &indent) const
{
    String indent2 = indent; indent2 += "   ";
    Debug_Log(indent << this);
    for (GameObject *child : GetChildren())
    {
        child->Print(indent2);
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
            oss << child->ToStringStructure(true, indent + "  ");
        }
    }
    return String(oss.str());
}

String GameObject::ToString() const
{
    std::ostringstream oss;
    oss << "GameObject: " << GetName() << "(" << ((void*)this) << ")";
    return oss.str();
}

void GameObject::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if (metaNode.Contains("Enabled"))
    {
        SetEnabled( metaNode.Get<bool>("Enabled") );
    }

    if (metaNode.Contains("Visible"))
    {
        SetVisible( metaNode.Get<bool>("Visible") );
    }

    if (metaNode.Contains("Name"))
    {
        SetName( metaNode.Get<String>("Name") );
    }

    if (metaNode.Contains("DontDestroyOnLoad"))
    {
        SetDontDestroyOnLoad( metaNode.Get<bool>("DontDestroyOnLoad") );
    }

    USet<GameObject*> childrenToRemove;
    for (GameObject *child : GetChildren())
    {
        childrenToRemove.Add(child);
    }

    USet<Component*> componentsToRemove;
    for (Component *comp : GetComponents())
    {
        componentsToRemove.Add(comp);
    }

    for (const MetaNode& childMeta : metaNode.GetChildren() )
    {
        const GUID guid = childMeta.Get<GUID>("GUID");
        const String& tagName = childMeta.GetName();

        if (tagName == GameObject::GetClassNameStatic())
        {
            GameObject *child = nullptr;
            for (GameObject *containedChild : GetChildren())
            {
                // See if it is already contained
                if (containedChild->GetGUID() == guid)
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
                child->SetParent(nullptr); // To reorder
            }
            child->SetParent(this);
            child->ImportMeta(childMeta);
        }
        else
        {
            Component *comp = nullptr;
            for (Component *containedComp : GetComponents())
            {
                // See if it is already contained
                if (containedComp->GetGUID() == guid)
                {
                    comp = containedComp;
                    componentsToRemove.Remove(comp);
                    break;
                }
            }

            if (!comp)
            {
                comp = AddComponent(tagName);
            }
            else
            {
                RemoveComponent(comp); // To reorder
                AddComponent(comp);
            }
            comp->ImportMeta(childMeta);
        }
    }

    // Remove non existing gameObjects and components in meta
    for (GameObject *childToRemove : childrenToRemove)
    {
        GameObject::DestroyImmediate(childToRemove);
    }
    for (Component *compToRemove : componentsToRemove)
    {
        Component::DestroyImmediate(compToRemove);
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
        if (c->GetHideFlags().IsOff(HideFlag::DONT_SERIALIZE))
        {
            MetaNode compMeta;
            c->ExportMeta(&compMeta);
            metaNode->AddChild(compMeta);
        }
    }

    for (GameObject *child : GetChildren())
    {
        if (child->GetHideFlags().IsOff(HideFlag::DONT_SERIALIZE))
        {
            MetaNode childMeta;
            child->ExportMeta(&childMeta);
            metaNode->AddChild(childMeta);
        }
    }
}
