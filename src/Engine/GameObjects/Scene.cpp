#include "Bang/Scene.h"

#include "Bang/Debug.h"
#include "Bang/Camera.h"
#include "Bang/MetaNode.h"
#include "Bang/Physics.h"
#include "Bang/GEngine.h"
#include "Bang/UICanvas.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/SceneManager.h"
#include "Bang/DebugRenderer.h"
#include "Bang/RectTransform.h"
#include "Bang/RenderFactory.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

Scene::Scene() : GameObject("Scene")
{
    p_debugRenderer = GameObject::Create<DebugRenderer>();

    Physics::GetInstance()->RegisterScene(this);
}

Scene::~Scene()
{
    GameObject::DestroyImmediate( GetDebugRenderer() );
}

void Scene::Render(RenderPass rp, bool renderChildren)
{
    GameObject::Render(rp, renderChildren);

    if (rp == RenderPass::SCENE)
    {
        GetDebugRenderer()->RenderPrimitives(true);
    }
    else if (rp == RenderPass::OVERLAY)
    {
        GetDebugRenderer()->RenderPrimitives(false);
    }
}

void Scene::OnResize(int newWidth, int newHeight)
{
    BANG_UNUSED_2(newWidth, newHeight);
    InvalidateCanvas();
}

void Scene::DestroyDelayedElements()
{
    while (!m_componentsToDestroyDelayed.IsEmpty())
    {
        Component *comp = m_componentsToDestroyDelayed.Back();
        Component::DestroyImmediate(comp);
    }

    while (!m_gameObjectsToDestroyDelayed.IsEmpty())
    {
        GameObject *go = m_gameObjectsToDestroyDelayed.Back();
        GameObject::DestroyImmediate(go);
    }
}

DebugRenderer *Scene::GetDebugRenderer() const
{
    return p_debugRenderer;
}

void Scene::SetCamera(Camera *cam)
{
    p_camera = cam;
    if (p_camera)
    {
        p_camera->EventEmitter<IEventsDestroy>::RegisterListener(this);
    }
}

void Scene::SetFirstFoundCamera()
{
    Camera *sceneCamera = GetComponentInDescendantsAndThis<Camera>();
    SetCamera(sceneCamera);
}

void Scene::AddGameObjectToDestroyDelayed(GameObject *go)
{
    if (!go->IsWaitingToBeDestroyed() &&
        !m_gameObjectsToDestroyDelayed.Contains(go))
    {
        m_gameObjectsToDestroyDelayed.PushBack(go);
        go->EventEmitter<IEventsDestroy>::RegisterListener(this);
    }
}

void Scene::AddComponentToDestroyDelayed(Component *comp)
{
    if (!comp->IsWaitingToBeDestroyed() &&
        !m_componentsToDestroyDelayed.Contains(comp))
    {
        m_componentsToDestroyDelayed.PushBack(comp);
        comp->EventEmitter<IEventsDestroy>::RegisterListener(this);
    }
}

void Scene::InvalidateCanvas()
{
    Array<UICanvas*> canvases = GetComponentsInDescendantsAndThis<UICanvas>();
    for (UICanvas *canvas : canvases)
    {
        canvas->InvalidateCanvas();
    }
}

void Scene::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (object == GetCamera())
    {
        SetCamera(nullptr);
    }

    if (Component *comp = DCAST<Component*>(object))
    {
        m_componentsToDestroyDelayed.Remove(comp);
    }

    if (GameObject *go = DCAST<GameObject*>(object))
    {
        m_gameObjectsToDestroyDelayed.Remove(go);
    }
}

Camera *Scene::GetCamera() const
{
    return p_camera;
}

void Scene::ImportMeta(const MetaNode &metaNode)
{
    GameObject::ImportMeta(metaNode);
    SetFirstFoundCamera();
}

void Scene::ExportMeta(MetaNode *metaNode) const
{
    GameObject::ExportMeta(metaNode);
    metaNode->SetName("Scene");
}
