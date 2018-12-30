#include "Bang/Scene.h"

#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Camera.h"
#include "Bang/DebugRenderer.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.tcc"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/IEvents.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/UICanvas.h"

namespace Bang
{
class ICloneable;
class IEventsDestroy;
}  // namespace Bang

using namespace Bang;

Scene::Scene() : GameObject("Scene")
{
    p_debugRenderer = new DebugRenderer();
    Physics::GetInstance()->RegisterScene(this);
}

Scene::~Scene()
{
    Physics::GetInstance()->UnRegisterScene(this);
    GameObject::DestroyImmediate(GetDebugRenderer());
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
    m_deltaTime = Time::GetPassedTimeSince(m_lastUpdateTime);
    m_lastUpdateTime = Time::GetNow();
    GameObject::Update();
}

void Scene::Render(RenderPass rp, bool renderChildren)
{
    GameObject::Render(rp, renderChildren);

    if (rp == RenderPass::SCENE_OPAQUE)
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

Time Scene::GetDeltaTime() const
{
    return m_deltaTime;
}

void Scene::InvalidateCanvas()
{
    Array<UICanvas *> canvases = GetComponentsInDescendantsAndThis<UICanvas>();
    for (UICanvas *canvas : canvases)
    {
        canvas->InvalidateCanvas();
    }
}

void Scene::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    GameObject::CloneInto(clone, cloneGUID);

    Scene *cloneScene = SCAST<Scene *>(clone);

    // Find cloned camera
    if (GetCamera())
    {
        Array<Camera *> cams = GetComponentsInDescendantsAndThis<Camera>();
        Array<Camera *> cloneCams =
            cloneScene->GetComponentsInDescendantsAndThis<Camera>();
        ASSERT(cams.Size() == cloneCams.Size());

        uint camIdx = cams.IndexOf(GetCamera());
        if (camIdx != SCAST<uint>(-1))
        {
            Camera *cloneCam = cloneCams[camIdx];
            cloneScene->SetCamera(cloneCam);
        }
    }
}

void Scene::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (object == GetCamera())
    {
        SetCamera(nullptr);
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
            SetCamera(camGo->GetComponent<Camera>());
        }
    }
}

void Scene::ExportMeta(MetaNode *metaNode) const
{
    GameObject::ExportMeta(metaNode);
    metaNode->SetName("Scene");

    GUID camGoGUID =
        GetCamera() ? GetCamera()->GetGameObject()->GetGUID() : GUID::Empty();
    metaNode->Set("CameraGameObjectGUID", camGoGUID);
}
