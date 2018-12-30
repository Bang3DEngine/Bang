#include "Bang/Component.h"

#include <ostream>

#include "Bang/Assert.h"
#include "Bang/Debug.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/GameObject.h"
#include "Bang/ICloneable.h"
#include "Bang/IEventsComponentChangeGameObject.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/String.h"

using namespace Bang;

Component::Component()
{
}

Component::~Component()
{
    ASSERT(IsBeingDestroyed());
    ASSERT(IsWaitingToBeDestroyed());
    ASSERT(GetGameObject() == nullptr);
    ASSERT(GetDestroyEventHasBeenPropagated());
}

void Component::DestroyImmediate(Component *component)
{
    if (!component->IsBeingDestroyed())
    {
        component->SetWaitingToBeDestroyed();
        component->SetBeingDestroyed();
        Object::PropagateObjectDestruction(component);
        component->SetGameObjectForced(nullptr);
        delete component;
    }
}

void Component::Destroy(Component *component)
{
    if (!component->IsWaitingToBeDestroyed())
    {
        component->SetWaitingToBeDestroyed();
        Object::PropagateObjectDestruction(component);

        if (GameObject *go = component->GetGameObject())
        {
            go->AddComponentToDestroyDelayed(component);
            component->SetGameObjectForced(nullptr);
        }
        else
        {
            Component::DestroyImmediate(component);
        }
    }
}

void Component::SetGameObject(GameObject *newGameObject)
{
    if (!IsWaitingToBeDestroyed())
    {
        SetGameObjectForced(newGameObject);
    }
}

void Component::SetGameObjectForced(GameObject *newGameObject)
{
    if (newGameObject != GetGameObject())
    {
        GameObject *previousGameObject = GetGameObject();

        if (newGameObject && newGameObject->IsWaitingToBeDestroyed())
        {
            Debug_Warn("Trying to set as gameObject a destroyed gameObject. "
                       "Not setting gameObject");
            return;
        }

        if (GetGameObject())
        {
            GetGameObject()->RemoveComponent(this);
        }

        if (newGameObject && !CanBeRepeatedInGameObject())
        {
            for (Component *comp : newGameObject->GetComponents())
            {
                if (comp != this && comp->GetClassName() == GetClassName())
                {
                    Debug_Error(comp->GetClassName()
                                << " should not be repeated in"
                                   "the same GameObject.");
                }
            }
        }

        p_gameObject = newGameObject;
        InvalidateEnabledRecursively();
        OnGameObjectChanged(previousGameObject, newGameObject);
    }
}

GameObject *Component::GetGameObject() const
{
    return p_gameObject;
}

void Component::OnPreStart()
{
}
void Component::OnStart()
{
}
void Component::OnUpdate()
{
}
void Component::OnAfterChildrenUpdate()
{
}
void Component::OnPostUpdate()
{
}
void Component::OnBeforeRender()
{
}
void Component::OnBeforeChildrenRender(RenderPass)
{
}
void Component::OnRender(RenderPass)
{
}
void Component::OnAfterChildrenRender(RenderPass)
{
}
void Component::OnDestroy()
{
}

void Component::Update()
{
    if (IsActiveRecursively())
    {
        OnUpdate();
    }
}
void Component::AfterChildrenUpdate()
{
    if (IsActiveRecursively())
    {
        OnAfterChildrenUpdate();
    }
}
void Component::BeforeRender()
{
    if (IsActiveRecursively())
    {
        OnBeforeRender();
    }
}
void Component::BeforeChildrenRender(RenderPass rp)
{
    if (IsActiveRecursively())
    {
        OnBeforeChildrenRender(rp);
    }
}
void Component::Render(RenderPass rp)
{
    if (IsActiveRecursively())
    {
        OnRender(rp);
    }
}
void Component::AfterChildrenRender(RenderPass rp)
{
    if (IsActiveRecursively())
    {
        OnAfterChildrenRender(rp);
    }
}

void Component::OnGameObjectChanged(GameObject *previousGameObject,
                                    GameObject *newGameObject)
{
    EventEmitter<IEventsComponentChangeGameObject>::PropagateToListeners(
        &IEventsComponentChangeGameObject::OnComponentChangedGameObject,
        previousGameObject,
        newGameObject,
        this);
}

bool Component::CanBeRepeatedInGameObject() const
{
    return true;
}

bool Component::CalculateEnabledRecursively() const
{
    return IsEnabled() &&
           (GetGameObject() ? GetGameObject()->IsEnabledRecursively() : true);
}

void Component::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Object::CloneInto(clone, cloneGUID);
    Component *c = SCAST<Component *>(clone);
    c->SetEnabled(IsEnabled());
}

String Component::ToString() const
{
    std::ostringstream msg;
    msg << "";
    return GetClassName() + "(" + String::ToString((void *)this) + ")";
}

void Component::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);
    if (metaNode.Contains("Enabled"))
    {
        SetEnabled(metaNode.Get<bool>("Enabled", true));
    }
}

void Component::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->SetName(GetClassName());
    metaNode->Set("Enabled", IsEnabled());
}
