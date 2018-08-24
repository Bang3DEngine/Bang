#include "Bang/ReflectionProbe.h"

#include "Bang/Scene.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/XMLNode.h"
#include "Bang/Renderer.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/TextureFactory.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/CubeMapIBLGenerator.h"

USING_NAMESPACE_BANG

ReflectionProbe::ReflectionProbe()
{
    p_textureCubeMapWithoutFiltering.Set(new TextureCubeMap());
    p_textureCubeMapDiffuse.Set(new TextureCubeMap());
    p_textureCubeMapSpecular.Set(new TextureCubeMap());

    for (int i = 0; i < GL::GetAllCubeMapDirs().size(); ++i)
    {
        GL::CubeMapDir cmDir = GL::GetAllCubeMapDirs()[i];

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

        GameObject *camGo = GameObjectFactory::CreateGameObject();
        Camera *cam = camGo->AddComponent<Camera>();
        cam->SetRenderSize( Vector2i(256) );
        cam->SetFovDegrees(90.0f);
        cam->RemoveRenderPass(RenderPass::OVERLAY);
        cam->RemoveRenderPass(RenderPass::OVERLAY_POSTPROCESS);
        cam->RemoveRenderPass(RenderPass::CANVAS);
        cam->RemoveRenderPass(RenderPass::CANVAS_POSTPROCESS);
        cam->SetRenderFlags( cam->GetRenderFlags().
                             SetOff(RenderFlag::RENDER_SHADOW_MAPS).
                             SetOff(RenderFlag::RENDER_REFLECTION_PROBES));
        cam->GetGameObject()->GetTransform()->LookInDirection(lookDir, upDir);

        m_cameras[i] = cam;
    }

    SetRenderSize(256);
    SetCamerasZNear(0.01f);
    SetCamerasZFar(100.0f);
    SetCamerasClearColor(Color::Gray);
    SetCamerasClearMode(Camera::ClearMode::COLOR);
    SetCamerasSkyBoxTexture( TextureFactory::GetDefaultTextureCubeMap() );
}

ReflectionProbe::~ReflectionProbe()
{
    for (int i = 0; i < GL::GetAllCubeMapDirs().size(); ++i)
    {
        GameObject::Destroy( GetCameras()[i]->GetGameObject() );
    }
}

void ReflectionProbe::RenderReflectionProbe(bool force)
{
    bool hasRested = ((Time::GetNow_Millis() - m_lastRenderTimeMillis) / 1000.0f >=
                       GetRestTimeSeconds());
    if (hasRested || force)
    {
        // Render from each of the 6 cameras...
        for (int i = 0; i < GL::GetAllCubeMapDirs().size(); ++i)
        {
            GameObject *camGo = GetCameras()[i]->GetGameObject();
            camGo->GetTransform()->SetPosition( GetGameObject()->GetTransform()->
                                                GetPosition() );

            Camera *cam = camGo->GetComponent<Camera>();
            GEngine::GetInstance()->RenderToGBuffer(GetGameObject()->GetScene(),
                                                    cam);
        }

        #define __GET_TEX(CubeMapDir) \
            GetCameras()[ GL::GetCubeMapDirIndex(CubeMapDir) ]-> \
            GetGBuffer()->GetLastDrawnColorTexture()

        GEngine::GetInstance()->FillCubeMapFromTextures(
                                        GetTextureCubeMapWithoutFiltering(),
                                        __GET_TEX(GL::CubeMapDir::TOP),
                                        __GET_TEX(GL::CubeMapDir::BOT),
                                        __GET_TEX(GL::CubeMapDir::LEFT),
                                        __GET_TEX(GL::CubeMapDir::RIGHT),
                                        __GET_TEX(GL::CubeMapDir::FRONT),
                                        __GET_TEX(GL::CubeMapDir::BACK));

        if (GetFilterForIBL())
        {
            p_textureCubeMapDiffuse  = CubeMapIBLGenerator::GenerateDiffuseIBLCubeMap(
                                            GetTextureCubeMapWithoutFiltering(),
                                            16,
                                            10);
            p_textureCubeMapSpecular = CubeMapIBLGenerator::GenerateSpecularIBLCubeMap(
                                            GetTextureCubeMapWithoutFiltering(),
                                            128,
                                            64);
        }

        m_lastRenderTimeMillis = Time::GetNow_Millis();
    }

#undef __GET_TEX
}

void ReflectionProbe::SetRenderSize(int size)
{
    if (size != GetRenderSize())
    {
        m_renderSize = size;

        #ifdef DEBUG
        ASSERT( Math::IsPowerOfTwo(size) );
        #endif

        for (Camera *cam : GetCameras())
        {
            cam->SetRenderSize( Vector2i(size) );
            GetTextureCubeMapWithoutFiltering()->Resize( size );
        }
    }
}

void ReflectionProbe::SetCamerasClearColor(const Color &clearColor)
{
    if (clearColor != GetCamerasClearColor())
    {
        m_camerasClearColor = clearColor;
        for (Camera *cam : GetCameras())
        {
            cam->SetClearColor( GetCamerasClearColor() );
        }
    }
}

void ReflectionProbe::SetCamerasSkyBoxTexture(TextureCubeMap *skyBoxTexture)
{
    if (skyBoxTexture != GetCamerasSkyBoxTexture())
    {
        m_camerasSkyBoxTexture.Set( skyBoxTexture );
        for (Camera *cam : GetCameras())
        {
            cam->SetSkyBoxTexture( GetCamerasSkyBoxTexture() );
        }
    }
}

void ReflectionProbe::SetCamerasClearMode(Camera::ClearMode clearMode)
{
    if (clearMode != GetCamerasClearMode())
    {
        m_camerasClearMode = clearMode;
        for (Camera *cam : GetCameras())
        {
            cam->SetClearMode( GetCamerasClearMode() );
        }
    }
}

void ReflectionProbe::SetCamerasZNear(float zNear)
{
    if (zNear != GetCamerasZNear())
    {
        m_camerasZNear = zNear;
        for (Camera *cam : GetCameras())
        {
            cam->SetZNear( GetCamerasZNear() );
        }
    }
}

void ReflectionProbe::SetCamerasZFar(float zFar)
{
    if (zFar != GetCamerasZFar())
    {
        m_camerasZFar = zFar;
        for (Camera *cam : GetCameras())
        {
            cam->SetZFar( GetCamerasZFar() );
        }
    }
}

void ReflectionProbe::SetSize(const Vector3 &size)
{
    if (size != GetSize())
    {
        m_size = size;
    }
}

void ReflectionProbe::SetIsBoxed(bool isBoxed)
{
    if (isBoxed != GetIsBoxed())
    {
        m_isBoxed = isBoxed;
    }
}

void ReflectionProbe::SetRestTimeSeconds(float restTimeSeconds)
{
    if (restTimeSeconds != GetRestTimeSeconds())
    {
        m_restTimeSeconds = restTimeSeconds;
        m_lastRenderTimeMillis = 0;
    }
}

void ReflectionProbe::SetFilterForIBL(bool filterForIBL)
{
    if (filterForIBL != GetFilterForIBL())
    {
        m_filterForIBL = filterForIBL;
    }
}

int ReflectionProbe::GetRenderSize() const
{
    return m_renderSize;
}

bool ReflectionProbe::GetIsBoxed() const
{
    return m_isBoxed;
}

float ReflectionProbe::GetRestTimeSeconds() const
{
    return m_restTimeSeconds;
}

bool ReflectionProbe::GetFilterForIBL() const
{
    return m_filterForIBL;
}

const Vector3 &ReflectionProbe::GetSize() const
{
    return m_size;
}

Camera* ReflectionProbe::GetCamera(GL::CubeMapDir cubeMapDir) const
{
    int cmDirIdx = GL::GetCubeMapDirIndex(cubeMapDir);
    return GetCameras()[cmDirIdx];
}

const std::array<Camera*, 6> &ReflectionProbe::GetCameras() const
{
    return m_cameras;
}

TextureCubeMap *ReflectionProbe::GetCamerasSkyBoxTexture() const
{
    return m_camerasSkyBoxTexture.Get();
}

const Color &ReflectionProbe::GetCamerasClearColor() const
{
    return m_camerasClearColor;
}

Camera::ClearMode ReflectionProbe::GetCamerasClearMode() const
{
    return m_camerasClearMode;
}

float ReflectionProbe::GetCamerasZNear() const
{
    return m_camerasZNear;
}

float ReflectionProbe::GetCamerasZFar() const
{
    return m_camerasZFar;
}

void ReflectionProbe::SetRendererUniforms(Renderer *renderer)
{
    if (Material *mat = renderer->GetActiveMaterial())
    {
        if (ShaderProgram *sp = mat->GetShaderProgram())
        {
            bool usingReflectionProbes = false;
            if (renderer->GetUseReflectionProbes())
            {
                if (ReflectionProbe *closestReflProbe =
                    GetClosestReflectionProbe(renderer))
                {
                    if (closestReflProbe->GetIsBoxed())
                    {
                        sp->SetVector3("B_ReflectionProbeCenter",
                                       closestReflProbe->GetGameObject()->
                                       GetTransform()->GetPosition(),
                                       false);
                        sp->SetVector3("B_ReflectionProbeSize",
                                       closestReflProbe->GetSize(),
                                       false);
                    }
                    else
                    {
                        sp->SetVector3("B_ReflectionProbeSize", -Vector3::One, false);
                    }

                    sp->SetBool("B_UseReflectionProbe", true, false);
                    sp->SetTextureCubeMap("B_ReflectionProbeDiffuse",
                                          closestReflProbe->GetTextureCubeMapDiffuse(),
                                          false);
                    sp->SetTextureCubeMap("B_ReflectionProbeSpecular",
                                          closestReflProbe->GetTextureCubeMapSpecular(),
                                          false);
                    usingReflectionProbes = true;
                }
            }

            if (!usingReflectionProbes)
            {
                sp->SetBool("B_UseReflectionProbe", false, false);
                sp->SetTextureCubeMap("B_ReflectionProbeDiffuse",
                                      TextureFactory::GetWhiteTextureCubeMap(),
                                      false);
                sp->SetTextureCubeMap("B_ReflectionProbeSpecular",
                                      TextureFactory::GetWhiteTextureCubeMap(),
                                      false);
            }
        }
    }
}

TextureCubeMap *ReflectionProbe::GetTextureCubeMapDiffuse() const
{
    return GetFilterForIBL() ? p_textureCubeMapDiffuse.Get() :
                               GetTextureCubeMapWithoutFiltering();
}

TextureCubeMap *ReflectionProbe::GetTextureCubeMapSpecular() const
{
    return GetFilterForIBL() ? p_textureCubeMapSpecular.Get() :
                               GetTextureCubeMapWithoutFiltering();
}

TextureCubeMap *ReflectionProbe::GetTextureCubeMapWithoutFiltering() const
{
    return p_textureCubeMapWithoutFiltering.Get();
}

void ReflectionProbe::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);

    ReflectionProbe *rpClone = SCAST<ReflectionProbe*>(clone);
    rpClone->SetSize( GetSize() );
    rpClone->SetRenderSize( GetRenderSize() );
    rpClone->SetFilterForIBL( GetFilterForIBL() );
    rpClone->SetRestTimeSeconds( GetRestTimeSeconds() );
    rpClone->SetIsBoxed( GetIsBoxed() );
    rpClone->SetCamerasZNear( GetCamerasZNear() );
    rpClone->SetCamerasZFar( GetCamerasZFar() );
    rpClone->SetCamerasClearMode( GetCamerasClearMode() );
    rpClone->SetCamerasSkyBoxTexture( GetCamerasSkyBoxTexture() );
    rpClone->SetCamerasClearColor( rpClone->GetCamerasClearColor() );
}

void ReflectionProbe::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);

    if (xmlInfo.Contains("Size"))
    {
        SetSize(xmlInfo.Get<Vector3>("Size"));
    }

    if (xmlInfo.Contains("IsBoxed"))
    {
        SetIsBoxed(xmlInfo.Get<bool>("IsBoxed"));
    }

    if (xmlInfo.Contains("RenderSize"))
    {
        SetRenderSize(xmlInfo.Get<int>("RenderSize"));
    }

    if (xmlInfo.Contains("FilterForIBL"))
    {
        SetFilterForIBL(xmlInfo.Get<bool>("FilterForIBL"));
    }

    if (xmlInfo.Contains("RestTimeSeconds"))
    {
        SetRestTimeSeconds(xmlInfo.Get<float>("RestTimeSeconds"));
    }

    if (xmlInfo.Contains("CamerasZNear"))
    {
        SetCamerasZNear(xmlInfo.Get<float>("CamerasZNear"));
    }

    if (xmlInfo.Contains("CamerasZFar"))
    {
        SetCamerasZFar(xmlInfo.Get<float>("CamerasZFar"));
    }

    if (xmlInfo.Contains("CamerasClearColor"))
    {
        SetCamerasClearColor(xmlInfo.Get<Color>("CamerasClearColor"));
    }

    if (xmlInfo.Contains("CamerasClearMode"))
    {
        SetCamerasClearMode( SCAST<Camera::ClearMode>(
                                 xmlInfo.Get<int>("CamerasClearMode")) );
    }

    if (xmlInfo.Contains("CamerasSkyBoxTexture"))
    {
        SetCamerasSkyBoxTexture(Resources::Load<TextureCubeMap>(
                                xmlInfo.Get<GUID>("CamerasSkyBoxTexture")).Get());
    }
}

void ReflectionProbe::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    xmlInfo->Set("Size", GetSize());
    xmlInfo->Set("IsBoxed", GetIsBoxed());
    xmlInfo->Set("RenderSize", GetRenderSize());
    xmlInfo->Set("FilterForIBL", GetFilterForIBL());
    xmlInfo->Set("RestTimeSeconds", GetRestTimeSeconds());

    xmlInfo->Set("CamerasZFar", GetCamerasZFar());
    xmlInfo->Set("CamerasZNear", GetCamerasZNear());
    xmlInfo->Set("CamerasClearColor", GetCamerasClearColor());
    xmlInfo->Set("CamerasClearMode", SCAST<int>(GetCamerasClearMode()));
    xmlInfo->Set("CamerasSkyBoxTexture",
             GetCamerasSkyBoxTexture() ? GetCamerasSkyBoxTexture()->GetGUID() :
                                         GUID::Empty());
}

ReflectionProbe *ReflectionProbe::GetClosestReflectionProbe(Renderer *renderer)
{
    GEngine *ge = GEngine::GetInstance();
    ASSERT(ge);

    Vector3 rendPos = renderer->GetGameObject()->GetTransform()->GetPosition();

    ReflectionProbe *closestReflProbe = nullptr;
    float closestReflProbeSqDist = Math::Infinity<float>();
    Array<ReflectionProbe*> reflProbes = ge->GetCurrentReflectionProbes();
    for (ReflectionProbe *reflProbe : reflProbes)
    {
        if (reflProbe->IsActive())
        {
            Vector3 reflProbePos = reflProbe->GetGameObject()->
                                   GetTransform()->GetPosition();
            float sqDist = Vector3::SqDistance(rendPos, reflProbePos);
            if (sqDist < closestReflProbeSqDist)
            {
                closestReflProbe = reflProbe;
                closestReflProbeSqDist = sqDist;
            }
        }
    }

    return closestReflProbe;
}

