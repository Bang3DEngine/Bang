#include "Bang/ReflectionProbe.h"

#include "Bang/Scene.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

ReflectionProbe::ReflectionProbe()
{
    m_textureCubeMapFB = new Framebuffer();
    m_textureCubeMapFB->CreateAttachmentTexCubeMap(GL::Attachment::COLOR0,
                                                   GL::ColorFormat::RGBA8);
    m_textureCubeMapFB->Resize( Vector2i(256) );

    p_textureCubeMap.Set(m_textureCubeMapFB->
                         GetAttachmentTexCubeMap(GL::Attachment::COLOR0));

    for (int i = 0; i < GL::GetAllCubeMapDirs().size(); ++i)
    {
        GL::CubeMapDir cmDir = GL::GetAllCubeMapDirs()[i];
        GameObject *camGo = GameObjectFactory::CreateGameObject();
        Camera *cam = camGo->AddComponent<Camera>();
        cam->SetRenderSize(p_textureCubeMap.Get()->GetSize() );

        Vector3 lookDir = Vector3::Forward;
        Vector3 upDir   = Vector3::Up;
        switch(cmDir)
        {
            case GL::CubeMapDir::TOP:
                lookDir = Vector3::Up;
                upDir = Vector3::Right;
            break;

            case GL::CubeMapDir::BOT:
                lookDir = Vector3::Down;
                upDir = Vector3::Right;
            break;

            case GL::CubeMapDir::LEFT:  lookDir = Vector3::Left;    break;
            case GL::CubeMapDir::RIGHT: lookDir = Vector3::Right;   break;
            case GL::CubeMapDir::BACK:  lookDir = Vector3::Back;    break;
            case GL::CubeMapDir::FRONT: lookDir = Vector3::Forward; break;
        }
        cam->SetFovDegrees(90.0f);
        cam->GetGameObject()->GetTransform()->LookInDirection(lookDir, upDir);

        m_cameraGos[i] = camGo;
    }
}

ReflectionProbe::~ReflectionProbe()
{
    delete m_textureCubeMapFB;

    for (int i = 0; i < GL::GetAllCubeMapDirs().size(); ++i)
    {
        GameObject::Destroy(m_cameraGos[i]);
    }
}

void ReflectionProbe::RenderReflectionProbe()
{
    Camera *sceneCam = GetGameObject()->GetScene()->GetCamera();

    // Render from each of the 6 cameras...
    for (int i = 0; i < GL::GetAllCubeMapDirs().size(); ++i)
    {
        GameObject *camGo = m_cameraGos[i];
        camGo->GetTransform()->SetPosition( GetGameObject()->GetTransform()->
                                            GetPosition() );

        Camera *cam = camGo->GetComponent<Camera>();
        cam->SetSkyBoxTexture(sceneCam->GetSkyBoxTexture());
        cam->SetRenderSize( p_textureCubeMap.Get()->GetSize() );

        GEngine::GetInstance()->RenderToGBuffer(GetGameObject()->GetScene(), cam);
    }

    #define __GET_TEX(CubeMapDir) \
        m_cameraGos[ GL::GetCubeMapDirIndex(CubeMapDir) ]-> \
        GetComponent<Camera>()->GetGBuffer()->GetLastDrawnColorTexture()

    GEngine::GetInstance()->FillCubeMapFromTextures(
                                    p_textureCubeMap.Get(),
                                    __GET_TEX(GL::CubeMapDir::TOP),
                                    __GET_TEX(GL::CubeMapDir::BOT),
                                    __GET_TEX(GL::CubeMapDir::LEFT),
                                    __GET_TEX(GL::CubeMapDir::RIGHT),
                                    __GET_TEX(GL::CubeMapDir::FRONT),
                                    __GET_TEX(GL::CubeMapDir::BACK));

    #undef __GET_TEX
}

TextureCubeMap *ReflectionProbe::GetTextureCubeMap() const
{
    return p_textureCubeMap.Get();
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

Camera *ReflectionProbe::GetCamera(GL::CubeMapDir dir) const
{
    return m_cameraGos[ GL::GetCubeMapDirIndex(dir) ]->GetComponent<Camera>();
}

