#include "Bang/Component.h"

#include "Bang/Debug.h"
#include "Bang/Scene.h"
#include "Bang/String.h"
#include "Bang/XMLNode.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/SceneManager.h"

USING_NAMESPACE_BANG

Component::Component()
{
}

Component::~Component()
{
    ASSERT(IsWaitingToBeDestroyed());
    ASSERT(GetGameObject() == nullptr);
}

void Component::DestroyImmediate(Component *component)
{
    if (!component->IsWaitingToBeDestroyed())
    {
        Object::PropagateObjectDestruction(component);
        component->SetGameObjectForced(nullptr);
        delete component;
    }
}

void Component::Destroy(Component *component)
{
    if (Scene *scene = SceneManager::GetActiveScene())
    {
        scene->AddComponentToDestroyDelayed(component);
        component->SetGameObject(nullptr);
    }
    else
    {
        Component::DestroyImmediate(component);
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
                if(comp != this && comp->GetClassName() == GetClassName())
                {
                    Debug_Error(comp->GetClassName() << " should not be repeated "
                                                        "the same a GameObject.");
                }
            }
        }

        p_gameObject = newGameObject;

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
void Component::OnPreUpdate()
{
}
void Component::OnBeforeChildrenUpdate()
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

void Component::PreUpdate()
{
    if (IsActive())
    {
        OnPreUpdate();
    }
}
void Component::BeforeChildrenUpdate()
{
    if (IsActive())
    {
        OnBeforeChildrenUpdate();
    }
}
void Component::Update()
{
    if (IsActive())
    {
        OnUpdate();
    }
}
void Component::AfterChildrenUpdate()
{
    if (IsActive())
    {
        OnAfterChildrenUpdate();
    }
}
void Component::BeforeRender()
{
    if (IsActive())
    {
        OnBeforeRender();
    }
}
void Component::BeforeChildrenRender(RenderPass rp)
{
    if (IsActive())
    {
        OnBeforeChildrenRender(rp);
    }
}
void Component::Render(RenderPass rp)
{
    if (IsActive())
    {
        OnRender(rp);
    }
}
void Component::AfterChildrenRender(RenderPass rp)
{
    if (IsActive())
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

bool Component::IsEnabled(bool recursive) const
{
    return recursive ?
             (Object::IsEnabled() && GetGameObject()->IsEnabled(true)) :
              Object::IsEnabled();
}

void Component::CloneInto(ICloneable *clone) const
{
    Object::CloneInto(clone);
    Component *c = Cast<Component*>(clone);
    c->SetEnabled( IsEnabled() );
}

String Component::ToString() const
{
    std::ostringstream msg;
    msg << "";
    return GetClassName() + "(" + String::ToString((void*)this) + ")";
}

void Component::ImportXML(const XMLNode &xmlInfo)
{
    Serializable::ImportXML(xmlInfo);
    if (xmlInfo.Contains("Enabled"))
    {
        SetEnabled(xmlInfo.Get<bool>("Enabled", true));
    }
}

void Component::ExportXML(XMLNode *xmlInfo) const
{
    Serializable::ExportXML(xmlInfo);

    xmlInfo->SetTagName( GetClassName() );
    xmlInfo->Set("Enabled", IsEnabled());
}
