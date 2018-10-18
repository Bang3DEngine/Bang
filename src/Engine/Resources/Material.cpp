#include "Bang/Material.h"

#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/Flags.h"
#include "Bang/GLUniforms.h"
#include "Bang/GUID.h"
#include "Bang/IEventsResource.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "Bang/Shader.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureFactory.h"
#include "Bang/TypeTraits.h"
#include "Bang/Vector.tcc"

namespace Bang
{
class Path;
class Resource;
}

using namespace Bang;

Material::Material()
{
    SetShaderProgram(ShaderProgramFactory::GetDefault());
}

Material::~Material()
{
}

void Material::SetLineWidth(float w)
{
    if (w != GetLineWidth())
    {
        m_lineWidth = w;
        PropagateResourceChanged();
    }
}

NeededUniformFlags &Material::GetNeededUniforms()
{
    return m_neededUniforms;
}

const NeededUniformFlags &Material::GetNeededUniforms() const
{
    return m_neededUniforms;
}

void Material::SetRenderWireframe(bool renderWireframe)
{
    if (renderWireframe != IsRenderWireframe())
    {
        m_renderWireframe = renderWireframe;
        PropagateResourceChanged();
    }
}

void Material::SetCullFace(GL::CullFaceExt cullFace)
{
    if (cullFace != GetCullFace())
    {
        m_cullFace = cullFace;
        PropagateResourceChanged();
    }
}

void Material::SetAlbedoUvOffset(const Vector2 &albedoUvOffset)
{
    if (albedoUvOffset != GetAlbedoUvOffset())
    {
        m_albedoUvOffset = albedoUvOffset;
        PropagateResourceChanged();
    }
}

void Material::SetAlbedoUvMultiply(const Vector2 &albedoUvMultiply)
{
    if (albedoUvMultiply != GetAlbedoUvMultiply())
    {
        m_albedoUvMultiply = albedoUvMultiply;
        PropagateResourceChanged();
    }
}

void Material::SetNormalMapUvOffset(const Vector2 &normalMapUvOffset)
{
    if (normalMapUvOffset != GetNormalMapUvOffset())
    {
        m_normalMapUvOffset = normalMapUvOffset;
        PropagateResourceChanged();
    }
}

void Material::SetNormalMapUvMultiply(const Vector2 &normalMapUvMultiply)
{
    if (normalMapUvMultiply != GetNormalMapUvMultiply())
    {
        m_normalMapUvMultiply = normalMapUvMultiply;
        PropagateResourceChanged();
    }
}

void Material::SetNormalMapMultiplyFactor(float normalMapMultiplyFactor)
{
    if (normalMapMultiplyFactor != GetNormalMapMultiplyFactor())
    {
        m_normalMapMultiplyFactor = normalMapMultiplyFactor;
        PropagateResourceChanged();
    }
}

void Material::SetShaderProgram(ShaderProgram *program)
{
    if (p_shaderProgram.Get() != program)
    {
        p_shaderProgram.Set(program);
        PropagateResourceChanged();
    }
}

void Material::SetReceivesLighting(bool receivesLighting)
{
    if (receivesLighting != GetReceivesLighting())
    {
        m_receivesLighting = receivesLighting;
        PropagateResourceChanged();
    }
}

void Material::SetRoughness(float roughness)
{
    if (roughness != GetRoughness())
    {
        m_roughness = roughness;
        PropagateResourceChanged();
    }
}

void Material::SetMetalness(float metalness)
{
    if (metalness != GetMetalness())
    {
        m_metalness = metalness;
        PropagateResourceChanged();
    }
}

void Material::SetAlbedoColor(const Color &albedoColor)
{
    if (albedoColor != GetAlbedoColor())
    {
        m_albedoColor = albedoColor;
        PropagateResourceChanged();
    }
}

void Material::SetRoughnessTexture(Texture2D *roughnessTexture)
{
    p_roughnessTexture.Set(roughnessTexture);
}

void Material::SetMetalnessTexture(Texture2D *metalnessTexture)
{
    p_metalnessTexture.Set(metalnessTexture);
}

void Material::SetAlbedoTexture(Texture2D *texture)
{
    if (p_albedoTexture.Get() != texture)
    {
        if (GetAlbedoTexture())
        {
            GetAlbedoTexture()
                ->EventEmitter<IEventsResource>::UnRegisterListener(this);
        }

        p_albedoTexture.Set(texture);
        ShaderProgram *sp = GetShaderProgram();
        if (sp)
        {
            sp->SetTexture2D(GLUniforms::UniformName_AlbedoTexture,
                             GetAlbedoTexture());
        }

        if (GetAlbedoTexture())
        {
            GetAlbedoTexture()->EventEmitter<IEventsResource>::RegisterListener(
                this);
        }

        PropagateResourceChanged();
    }
}

void Material::SetNormalMapTexture(Texture2D *texture)
{
    if (texture != GetNormalMapTexture())
    {
        if (GetNormalMapTexture())
        {
            GetNormalMapTexture()
                ->EventEmitter<IEventsResource>::UnRegisterListener(this);
        }

        p_normalMapTexture.Set(texture);
        ShaderProgram *sp = GetShaderProgram();
        if (sp)
        {
            sp->SetTexture2D(GLUniforms::UniformName_NormalMapTexture,
                             GetNormalMapTexture());
        }

        if (GetNormalMapTexture())
        {
            GetNormalMapTexture()
                ->EventEmitter<IEventsResource>::RegisterListener(this);
        }

        PropagateResourceChanged();
    }
}

void Material::SetRenderPass(RenderPass renderPass)
{
    if (renderPass != GetRenderPass())
    {
        m_renderPass = renderPass;
        PropagateResourceChanged();
    }
}

const Vector2 &Material::GetAlbedoUvOffset() const
{
    return m_albedoUvOffset;
}
const Vector2 &Material::GetAlbedoUvMultiply() const
{
    return m_albedoUvMultiply;
}
const Vector2 &Material::GetNormalMapUvOffset() const
{
    return m_normalMapUvOffset;
}
const Vector2 &Material::GetNormalMapUvMultiply() const
{
    return m_normalMapUvMultiply;
}
float Material::GetNormalMapMultiplyFactor() const
{
    return m_normalMapMultiplyFactor;
}
ShaderProgram *Material::GetShaderProgram() const
{
    return p_shaderProgram.Get();
}
Texture2D *Material::GetAlbedoTexture() const
{
    return p_albedoTexture.Get();
}
bool Material::GetReceivesLighting() const
{
    return m_receivesLighting;
}
float Material::GetMetalness() const
{
    return m_metalness;
}
float Material::GetRoughness() const
{
    return m_roughness;
}
const Color &Material::GetAlbedoColor() const
{
    return m_albedoColor;
}
Texture2D *Material::GetRoughnessTexture() const
{
    return p_roughnessTexture.Get();
}

Texture2D *Material::GetMetalnessTexture() const
{
    return p_metalnessTexture.Get();
}
Texture2D *Material::GetNormalMapTexture() const
{
    return p_normalMapTexture.Get();
}
RenderPass Material::GetRenderPass() const
{
    return m_renderPass;
}
float Material::GetLineWidth() const
{
    return m_lineWidth;
}
bool Material::IsRenderWireframe() const
{
    return m_renderWireframe;
}
GL::CullFaceExt Material::GetCullFace() const
{
    return m_cullFace;
}

void Material::Bind() const
{
    ShaderProgram *sp = GetShaderProgram();
    if (!sp || !sp->IsLinked())
    {
        return;
    }

    sp->Bind();

    GL::SetWireframe(IsRenderWireframe());
    GL::LineWidth(GetLineWidth());
    GL::PointSize(GetLineWidth());

    if (GetCullFace() != GL::CullFaceExt::NONE)
    {
        GL::Enable(GL::Enablable::CULL_FACE);  // Culling states
        GL::SetCullFace(SCAST<GL::Face>(GetCullFace()));
    }
    else
    {
        GL::Disable(GL::Enablable::CULL_FACE);
    }

    if (GetNeededUniforms().IsOn(NeededUniformFlag::MATERIAL_ALBEDO))
    {
        sp->SetColor(GLUniforms::UniformName_MaterialAlbedoColor,
                     GetAlbedoColor());

        if (Texture2D *albedoTex = GetAlbedoTexture())
        {
            sp->SetTexture2D(GLUniforms::UniformName_AlbedoTexture, albedoTex);
            sp->SetVector2(GLUniforms::UniformName_AlbedoUvOffset,
                           GetAlbedoUvOffset());
            sp->SetVector2(GLUniforms::UniformName_AlbedoUvMultiply,
                           GetAlbedoUvMultiply());
            sp->SetFloat(GLUniforms::UniformName_AlphaCutoff,
                         albedoTex->GetAlphaCutoff());
            sp->SetBool(GLUniforms::UniformName_HasAlbedoTexture, true);
        }
        else
        {
            sp->SetTexture2D(GLUniforms::UniformName_AlbedoTexture, nullptr);
            sp->SetBool(GLUniforms::UniformName_HasAlbedoTexture, false);
        }
    }

    if (GetNeededUniforms().IsOn(NeededUniformFlag::MATERIAL_PBR))
    {
        sp->SetBool(GLUniforms::UniformName_MaterialReceivesLighting,
                    GetReceivesLighting());
        sp->SetFloat(GLUniforms::UniformName_MaterialRoughness, GetRoughness());
        sp->SetFloat(GLUniforms::UniformName_MaterialMetalness, GetMetalness());
        sp->SetTexture2D(GLUniforms::UniformName_BRDF_LUT,
                         TextureFactory::GetBRDFLUTTexture());

        if (Texture2D *roughnessTex = GetRoughnessTexture())
        {
            sp->SetTexture2D(GLUniforms::UniformName_RoughnessTexture,
                             roughnessTex);
        }
        else
        {
            sp->SetTexture2D(GLUniforms::UniformName_RoughnessTexture,
                             TextureFactory::GetWhiteTexture());
        }

        if (Texture2D *metalnessTex = GetMetalnessTexture())
        {
            sp->SetTexture2D(GLUniforms::UniformName_MetalnessTexture,
                             metalnessTex);
        }
        else
        {
            sp->SetTexture2D(GLUniforms::UniformName_MetalnessTexture,
                             TextureFactory::GetWhiteTexture());
        }

        if (Texture2D *normalMapTex = GetNormalMapTexture())
        {
            sp->SetTexture2D(GLUniforms::UniformName_NormalMapTexture,
                             normalMapTex);
            sp->SetVector2(GLUniforms::UniformName_NormalMapUvOffset,
                           GetNormalMapUvOffset());
            sp->SetVector2(GLUniforms::UniformName_NormalMapUvMultiply,
                           GetNormalMapUvMultiply());
            sp->SetFloat(GLUniforms::UniformName_NormalMapMultiplyFactor,
                         GetNormalMapMultiplyFactor());
            sp->SetBool(GLUniforms::UniformName_HasNormalMapTexture, true);
        }
        else
        {
            sp->SetTexture2D(GLUniforms::UniformName_NormalMapTexture, nullptr);
            sp->SetBool(GLUniforms::UniformName_HasNormalMapTexture, false);
        }
    }
}

void Material::UnBind() const
{
    if (ShaderProgram *sp = GetShaderProgram())
    {
        sp->UnBind();
    }
}

void Material::CloneInto(ICloneable *clone) const
{
    Serializable::CloneInto(clone);

    Material *matClone = Cast<Material *>(clone);

    matClone->GetNeededUniforms().SetOn(GetNeededUniforms().GetValue());
    matClone->SetShaderProgram(GetShaderProgram());
    matClone->SetAlbedoColor(GetAlbedoColor());
    matClone->SetReceivesLighting(GetReceivesLighting());
    matClone->SetRoughness(GetRoughness());
    matClone->SetMetalness(GetMetalness());
    matClone->SetAlbedoTexture(GetAlbedoTexture());
    matClone->SetAlbedoUvOffset(GetAlbedoUvOffset());
    matClone->SetAlbedoUvMultiply(GetAlbedoUvMultiply());
    matClone->SetNormalMapTexture(GetNormalMapTexture());
    matClone->SetNormalMapUvOffset(GetNormalMapUvOffset());
    matClone->SetNormalMapUvMultiply(GetNormalMapUvMultiply());
    matClone->SetNormalMapMultiplyFactor(GetNormalMapMultiplyFactor());
    matClone->SetRenderPass(GetRenderPass());
    matClone->SetLineWidth(GetLineWidth());
    matClone->SetRoughnessTexture(GetRoughnessTexture());
    matClone->SetMetalnessTexture(GetMetalnessTexture());
    matClone->SetRenderWireframe(IsRenderWireframe());
    matClone->SetCullFace(GetCullFace());
}

void Material::OnResourceChanged(Resource *res)
{
    PropagateResourceChanged();
}

void Material::Import(const Path &materialFilepath)
{
    ImportMetaFromFile(materialFilepath);
}

void Material::ImportMeta(const MetaNode &meta)
{
    Asset::ImportMeta(meta);

    if (meta.Contains("NeededUniforms"))
    {
        GetNeededUniforms().SetTo(
            meta.Get<FlagsPrimitiveType>("NeededUniforms"));
    }

    if (meta.Contains("RenderPass"))
    {
        SetRenderPass(meta.Get<RenderPass>("RenderPass"));
    }

    if (meta.Contains("AlbedoColor"))
    {
        SetAlbedoColor(meta.Get<Color>("AlbedoColor"));
    }

    if (meta.Contains("Roughness"))
    {
        SetRoughness(meta.Get<float>("Roughness"));
    }

    if (meta.Contains("Metalness"))
    {
        SetMetalness(meta.Get<float>("Metalness"));
    }

    if (meta.Contains("ReceivesLighting"))
    {
        SetReceivesLighting(meta.Get<bool>("ReceivesLighting"));
    }

    if (meta.Contains("AlbedoTexture"))
    {
        SetAlbedoTexture(
            Resources::Load<Texture2D>(meta.Get<GUID>("AlbedoTexture")).Get());
    }

    if (meta.Contains("AlbedoUvOffset"))
    {
        SetAlbedoUvOffset(meta.Get<Vector2>("AlbedoUvOffset"));
    }

    if (meta.Contains("AlbedoUvMultiply"))
    {
        SetAlbedoUvMultiply(meta.Get<Vector2>("AlbedoUvMultiply"));
    }

    if (meta.Contains("RoughnessTexture"))
    {
        SetRoughnessTexture(
            Resources::Load<Texture2D>(meta.Get<GUID>("RoughnessTexture"))
                .Get());
    }

    if (meta.Contains("MetalnessTexture"))
    {
        SetMetalnessTexture(
            Resources::Load<Texture2D>(meta.Get<GUID>("MetalnessTexture"))
                .Get());
    }

    if (meta.Contains("NormalMapTexture"))
    {
        SetNormalMapTexture(
            Resources::Load<Texture2D>(meta.Get<GUID>("NormalMapTexture"))
                .Get());
    }

    if (meta.Contains("NormalMapUvOffset"))
    {
        SetNormalMapUvOffset(meta.Get<Vector2>("NormalMapUvOffset"));
    }

    if (meta.Contains("NormalMapUvMultiply"))
    {
        SetNormalMapUvMultiply(meta.Get<Vector2>("NormalMapUvMultiply"));
    }

    if (meta.Contains("NormalMapMultiplyFactor"))
    {
        SetNormalMapMultiplyFactor(meta.Get<float>("NormalMapMultiplyFactor"));
    }

    RH<Shader> vShader;
    if (meta.Contains("VertexShader"))
    {
        vShader = Resources::Load<Shader>(meta.Get<GUID>("VertexShader"));
    }

    RH<Shader> fShader;
    if (meta.Contains("FragmentShader"))
    {
        fShader = Resources::Load<Shader>(meta.Get<GUID>("FragmentShader"));
    }

    if (meta.Contains("LineWidth"))
    {
        SetLineWidth(meta.Get<float>("LineWidth"));
    }

    if (meta.Contains("RenderWireframe"))
    {
        SetRenderWireframe(meta.Get<bool>("RenderWireframe"));
    }

    if (meta.Contains("CullFace"))
    {
        SetCullFace(meta.Get<GL::CullFaceExt>("CullFace"));
    }

    if (vShader && fShader)
    {
        ShaderProgram *newSp =
            ShaderProgramFactory::Get(vShader.Get()->GetResourceFilepath(),
                                      fShader.Get()->GetResourceFilepath());
        SetShaderProgram(newSp);
    }
}

void Material::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);

    metaNode->Set("NeededUniforms", GetNeededUniforms());
    metaNode->Set("RenderPass", GetRenderPass());
    metaNode->Set("AlbedoColor", GetAlbedoColor());
    metaNode->Set("Roughness", GetRoughness());
    metaNode->Set("Metalness", GetMetalness());
    metaNode->Set("ReceivesLighting", GetReceivesLighting());
    metaNode->Set("AlbedoUvMultiply", GetAlbedoUvMultiply());
    metaNode->Set("AlbedoUvOffset", GetAlbedoUvOffset());
    metaNode->Set("NormalMapUvMultiply", GetNormalMapUvMultiply());
    metaNode->Set("NormalMapUvOffset", GetNormalMapUvOffset());
    metaNode->Set("NormalMapMultiplyFactor", GetNormalMapMultiplyFactor());
    metaNode->Set("LineWidth", GetLineWidth());
    metaNode->Set("RenderWireframe", IsRenderWireframe());
    metaNode->Set("CullFace", GetCullFace());

    Texture2D *albedoTex = GetAlbedoTexture();
    metaNode->Set("AlbedoTexture",
                  albedoTex ? albedoTex->GetGUID() : GUID::Empty());
    Texture2D *roughnessTex = GetRoughnessTexture();
    metaNode->Set("RoughnessTexture",
                  roughnessTex ? roughnessTex->GetGUID() : GUID::Empty());
    Texture2D *metalnessTex = GetMetalnessTexture();
    metaNode->Set("MetalnessTexture",
                  metalnessTex ? metalnessTex->GetGUID() : GUID::Empty());
    Texture2D *normalTex = GetNormalMapTexture();
    metaNode->Set("NormalMapTexture",
                  normalTex ? normalTex->GetGUID() : GUID::Empty());

    ShaderProgram *sp = GetShaderProgram();
    Shader *vShader = (sp ? sp->GetVertexShader() : nullptr);
    Shader *fShader = (sp ? sp->GetFragmentShader() : nullptr);
    metaNode->Set("VertexShader", vShader->GetGUID());
    metaNode->Set("FragmentShader", fShader->GetGUID());
}
