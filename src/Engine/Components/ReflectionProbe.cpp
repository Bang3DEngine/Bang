#include "Bang/ReflectionProbe.h"

#include "Bang/Scene.h"
#include "Bang/Camera.h"
#include "Bang/GEngine.h"
#include "Bang/GameObject.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

ReflectionProbe::ReflectionProbe()
{
    m_cameraGo = GameObjectFactory::CreateGameObject();
    m_cameraGo->AddComponent<Camera>();
    m_cameraGo->GetComponent<Camera>()->SetRenderSize( Vector2i(256) );
}

ReflectionProbe::~ReflectionProbe()
{
    GameObject::Destroy(m_cameraGo);
}

Camera *ReflectionProbe::GetCamera() const
{
    return m_cameraGo->GetComponent<Camera>();
}

void ReflectionProbe::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);

    ReflectionProbe *rpClone = SCAST<ReflectionProbe*>(clone);
    (void) rpClone;
}

void ReflectionProbe::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);
}

void ReflectionProbe::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);
}

