#include "Bang/Scene.h"

#include "Bang/Debug.h"
#include "Bang/Camera.h"
#include "Bang/XMLNode.h"
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
    m_gizmos = new RenderFactory();
    p_debugRenderer = GameObject::Create<DebugRenderer>();

    Physics::GetInstance()->RegisterScene(this);
}

Scene::~Scene()
{
    delete m_gizmos;
    GameObject::Destroy(p_debugRenderer);
}

void Scene::OnPreStart()
{
    GameObject::OnPreStart();
}

void Scene::Update()
{
    GameObject::Update();
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
    InvalidateCanvas();
}

RenderFactory *Scene::GetGizmos() const
{
    return m_gizmos;
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
    Camera *sceneCamera = GetComponentInChildrenAndThis<Camera>();
    SetCamera(sceneCamera);
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
}

Camera *Scene::GetCamera() const
{
    return p_camera;
}

void Scene::ImportXML(const XMLNode &xmlInfo)
{
    GameObject::ImportXML(xmlInfo);
    SetFirstFoundCamera();
}

void Scene::ExportXML(XMLNode *xmlInfo) const
{
    GameObject::ExportXML(xmlInfo);
    xmlInfo->SetTagName("Scene");
}
