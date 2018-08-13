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
        GameObject *camGo = GameObjectFactory::CreateGameObject();
        Camera *cam = camGo->AddComponent<Camera>();
        cam->SetRenderSize( Vector2i(256) );

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
        cam->RemoveRenderPass(RenderPass::OVERLAY);
        cam->RemoveRenderPass(RenderPass::OVERLAY_POSTPROCESS);
        cam->RemoveRenderPass(RenderPass::CANVAS);
        cam->RemoveRenderPass(RenderPass::CANVAS_POSTPROCESS);
        cam->SetRenderFlags( cam->GetRenderFlags().
                             SetOff(RenderFlag::RENDER_SHADOW_MAPS).
                             SetOff(RenderFlag::RENDER_REFLECTION_PROBES));
        cam->GetGameObject()->GetTransform()->LookInDirection(lookDir, upDir);

        m_cameraGos[i] = camGo;
    }
}

ReflectionProbe::~ReflectionProbe()
{
    for (int i = 0; i < GL::GetAllCubeMapDirs().size(); ++i)
    {
        GameObject::Destroy(m_cameraGos[i]);
    }
}

void ReflectionProbe::RenderReflectionProbe()
{
    if ((Time::GetNow_Millis() - m_lastRenderTimeMillis) / 1000.0f >=
        GetRestTimeSeconds())
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

            GetTextureCubeMapWithoutFiltering()->Resize( cam->GetRenderSize().x );

            GEngine::GetInstance()->RenderToGBuffer(GetGameObject()->GetScene(),
                                                    cam);
        }

        #define __GET_TEX(CubeMapDir) \
            m_cameraGos[ GL::GetCubeMapDirIndex(CubeMapDir) ]-> \
            GetComponent<Camera>()->GetGBuffer()->GetLastDrawnColorTexture()

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
                                            GetTextureCubeMapWithoutFiltering());
            p_textureCubeMapSpecular = CubeMapIBLGenerator::GenerateSpecularIBLCubeMap(
                                            GetTextureCubeMapWithoutFiltering());
        }

        m_lastRenderTimeMillis = Time::GetNow_Millis();
    }

#undef __GET_TEX
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
    m_restTimeSeconds = restTimeSeconds;
    m_lastRenderTimeMillis = 0;
}

void ReflectionProbe::SetFilterForIBL(bool filterForIBL)
{
    m_filterForIBL = filterForIBL;
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
    return m_cameraGos[cmDirIdx]->GetComponent<Camera>();
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
                                      TextureFactory::GetWhiteTextureCubeMap().Get(),
                                      false);
                sp->SetTextureCubeMap("B_ReflectionProbeSpecular",
                                      TextureFactory::GetWhiteTextureCubeMap().Get(),
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
    rpClone->SetIsBoxed( GetIsBoxed() );
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
}

void ReflectionProbe::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    xmlInfo->Set("Size", GetSize());
    xmlInfo->Set("IsBoxed", GetIsBoxed());
}

ReflectionProbe *ReflectionProbe::GetClosestReflectionProbe(Renderer *renderer)
{
    GEngine *ge = GEngine::GetInstance();
    ASSERT(ge);

    Vector3 rendPos = renderer->GetGameObject()->GetTransform()->GetPosition();

    ReflectionProbe *closestReflProbe = nullptr;
    float closestReflProbeSqDist = Math::Infinity<float>();
    List<ReflectionProbe*> reflProbes = ge->GetCurrentReflectionProbes();
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

