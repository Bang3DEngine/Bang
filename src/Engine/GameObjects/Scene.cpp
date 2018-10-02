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

void Scene::Start()
{
    if (!IsStarted())
    {
        m_lastUpdateTime = Time::GetNow();
    }
    GameObject::Start();
}

void Scene::Update()
{
    GameObject::Update();
}

void Scene::PostUpdate()
{
    GameObject::PostUpdate();
    m_lastUpdateTime = Time::GetNow();
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
    if (GetCamera())
    {
        GetCamera()->EventEmitter<IEventsDestroy>::UnRegisterListener(this);
    }

    p_camera = cam;
    if (GetCamera())
    {
        GetCamera()->EventEmitter<IEventsDestroy>::RegisterListener(this);
    }
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

Time Scene::GetDeltaTime() const
{
    return Time::GetNow() - m_lastUpdateTime;
}

void Scene::InvalidateCanvas()
{
    Array<UICanvas*> canvases = GetComponentsInDescendantsAndThis<UICanvas>();
    for (UICanvas *canvas : canvases)
    {
        canvas->InvalidateCanvas();
    }
}

void Scene::CloneInto(ICloneable *clone) const
{
    GameObject::CloneInto(clone);

    Scene *cloneScene = SCAST<Scene*>(clone);

    // Find cloned camera by GUID.
    if (GetCamera())
    {
        Array<Camera*> cams = GetComponentsInDescendantsAndThis<Camera>();
        Array<Camera*> cloneCams = cloneScene->GetComponentsInDescendantsAndThis<Camera>();
        ASSERT(cams.Size() == cloneCams.Size());

        uint camIdx = cams.IndexOf(GetCamera());
        if (camIdx != -1u)
        {
            Camera *cloneCam = cloneCams[camIdx];
            cloneScene->SetCamera( cloneCam );
        }
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

    if (metaNode.Contains("CameraGameObjectGUID"))
    {
        GUID camGoGUID = metaNode.Get<GUID>("CameraGameObjectGUID");
        if (GameObject *camGo = FindInChildrenAndThis(camGoGUID, true))
        {
            SetCamera( camGo->GetComponent<Camera>() );
        }
    }
}

void Scene::ExportMeta(MetaNode *metaNode) const
{
    GameObject::ExportMeta(metaNode);
    metaNode->SetName("Scene");

    GUID camGoGUID = GetCamera() ? GetCamera()->GetGameObject()->GetGUID() :
                                   GUID::Empty();
    metaNode->Set("CameraGameObjectGUID", camGoGUID);
}
