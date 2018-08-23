#include "Bang/Material.h"

#include "Bang/Paths.h"
#include "Bang/Shader.h"
#include "Bang/XMLNode.h"
#include "Bang/Vector4.h"
#include "Bang/Texture.h"
#include "Bang/Texture2D.h"
#include "Bang/Resources.h"
#include "Bang/GLUniforms.h"
#include "Bang/ShaderProgram.h"
#include "Bang/TextureFactory.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

Material::Material()
{
    SetShaderProgram( ShaderProgramFactory::GetDefault() );
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

void Material::SetShaderProgram(ShaderProgram* program)
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

void Material::SetAlbedoTexture(Texture2D* texture)
{
    if (p_albedoTexture.Get() != texture)
    {
        if (GetAlbedoTexture())
        {
            GetAlbedoTexture()->EventEmitter<IEventsResource>::
                                UnRegisterListener(this);
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
            GetAlbedoTexture()->EventEmitter<IEventsResource>::
                                RegisterListener(this);
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
            GetNormalMapTexture()->EventEmitter<IEventsResource>::
                                   UnRegisterListener(this);
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
            GetNormalMapTexture()->EventEmitter<IEventsResource>::
                                   RegisterListener(this);
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

const Vector2 &Material::GetAlbedoUvOffset() const { return m_albedoUvOffset; }
const Vector2 &Material::GetAlbedoUvMultiply() const { return m_albedoUvMultiply; }
const Vector2 &Material::GetNormalMapUvOffset() const { return m_normalMapUvOffset; }
const Vector2 &Material::GetNormalMapUvMultiply() const { return m_normalMapUvMultiply; }
float Material::GetNormalMapMultiplyFactor() const { return m_normalMapMultiplyFactor; }
ShaderProgram* Material::GetShaderProgram() const { return p_shaderProgram.Get(); }
Texture2D* Material::GetAlbedoTexture() const { return p_albedoTexture.Get(); }
bool Material::GetReceivesLighting() const { return m_receivesLighting; }
float Material::GetMetalness() const { return m_metalness; }
float Material::GetRoughness() const { return m_roughness; }
const Color& Material::GetAlbedoColor() const { return m_albedoColor; }

Texture2D *Material::GetRoughnessTexture() const
{
    return p_roughnessTexture.Get();
}

Texture2D *Material::GetMetalnessTexture() const
{
    return p_metalnessTexture.Get();
}
Texture2D *Material::GetNormalMapTexture() const { return p_normalMapTexture.Get(); }
RenderPass Material::GetRenderPass() const { return m_renderPass; }
float Material::GetLineWidth() const { return m_lineWidth; }
bool Material::IsRenderWireframe() const { return m_renderWireframe; }
GL::CullFaceExt Material::GetCullFace() const { return m_cullFace; }

void Material::Bind() const
{
    ShaderProgram *sp = GetShaderProgram();
    if (!sp)
    {
        return;
    }

    sp->Bind();

    GL::SetWireframe( IsRenderWireframe() );

    if (GetCullFace() != GL::CullFaceExt::NONE)
    {
        GL::Enable(GL::Enablable::CULL_FACE); // Culling states
        GL::SetCullFace( SCAST<GL::Face>(GetCullFace()) );
    }
    else { GL::Disable(GL::Enablable::CULL_FACE); }

    GL::LineWidth( GetLineWidth() );
    GL::PointSize( GetLineWidth() );

    sp->SetColor(GLUniforms::UniformName_MaterialAlbedoColor, GetAlbedoColor());
    sp->SetVector2(GLUniforms::UniformName_AlbedoUvOffset, GetAlbedoUvOffset());
    sp->SetVector2(GLUniforms::UniformName_AlbedoUvMultiply, GetAlbedoUvMultiply());
    sp->SetBool(GLUniforms::UniformName_MaterialReceivesLighting,
                GetReceivesLighting());

    if (Texture2D *albedoTex = GetAlbedoTexture())
    {
        sp->SetTexture2D(GLUniforms::UniformName_AlbedoTexture, albedoTex);
        sp->SetFloat(GLUniforms::UniformName_AlphaCutoff, albedoTex->GetAlphaCutoff());
        sp->SetBool(GLUniforms::UniformName_HasAlbedoTexture, true);
    }
    else
    {
        sp->SetTexture2D(GLUniforms::UniformName_AlbedoTexture, nullptr);
        sp->SetFloat(GLUniforms::UniformName_AlphaCutoff, -1.0f);
        sp->SetBool(GLUniforms::UniformName_HasAlbedoTexture, false);
    }

    sp->SetFloat(GLUniforms::UniformName_MaterialRoughness, GetRoughness());
    sp->SetFloat(GLUniforms::UniformName_MaterialMetalness, GetMetalness());
    sp->SetVector2(GLUniforms::UniformName_NormalMapUvOffset,
                   GetNormalMapUvOffset());
    sp->SetVector2(GLUniforms::UniformName_NormalMapUvMultiply,
                   GetNormalMapUvMultiply());
    sp->SetFloat(GLUniforms::UniformName_NormalMapMultiplyFactor,
                 GetNormalMapMultiplyFactor());
    sp->SetTexture2D(GLUniforms::UniformName_BRDF_LUT,
                     TextureFactory::GetBRDFLUTTexture());

    if (Texture2D *roughnessTex = GetRoughnessTexture())
    {
        sp->SetTexture2D(GLUniforms::UniformName_RoughnessTexture, roughnessTex);
    }
    else
    {
        sp->SetTexture2D(GLUniforms::UniformName_RoughnessTexture,
                         TextureFactory::GetWhiteTexture());
    }

    if (Texture2D *metalnessTex = GetMetalnessTexture())
    {
        sp->SetTexture2D(GLUniforms::UniformName_MetalnessTexture, metalnessTex);
    }
    else
    {
        sp->SetTexture2D(GLUniforms::UniformName_MetalnessTexture,
                         TextureFactory::GetWhiteTexture());
    }

    if (Texture2D *normalMapTex = GetNormalMapTexture())
    {
        sp->SetTexture2D(GLUniforms::UniformName_NormalMapTexture, normalMapTex);
        sp->SetBool(GLUniforms::UniformName_HasNormalMapTexture, true);
    }
    else
    {
        sp->SetTexture2D(GLUniforms::UniformName_NormalMapTexture, nullptr);
        sp->SetBool(GLUniforms::UniformName_HasNormalMapTexture, false);
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
    Material *matClone = Cast<Material*>(clone);

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
    matClone->SetRoughnessTexture( GetRoughnessTexture() );
    matClone->SetMetalnessTexture( GetMetalnessTexture() );
    matClone->SetRenderWireframe(IsRenderWireframe());
    matClone->SetCullFace(GetCullFace());
}

void Material::OnResourceChanged(Resource *res)
{
    PropagateResourceChanged();
}

void Material::Import(const Path &materialFilepath)
{
    ImportXMLFromFile(materialFilepath);
}

void Material::ImportXML(const XMLNode &xml)
{
    Asset::ImportXML(xml);

    if (xml.Contains("RenderPass"))
    { SetRenderPass(xml.Get<RenderPass>("RenderPass")); }

    if (xml.Contains("AlbedoColor"))
    { SetAlbedoColor(xml.Get<Color>("AlbedoColor")); }

    if (xml.Contains("Roughness"))
    { SetRoughness(xml.Get<float>("Roughness")); }

    if (xml.Contains("Metalness"))
    { SetMetalness(xml.Get<float>("Metalness")); }

    if (xml.Contains("ReceivesLighting"))
    { SetReceivesLighting(xml.Get<bool>("ReceivesLighting")); }

    if (xml.Contains("AlbedoTexture"))
    { SetAlbedoTexture(
          Resources::Load<Texture2D>(xml.Get<GUID>("AlbedoTexture")).Get()); }

    if (xml.Contains("AlbedoUvOffset"))
    { SetAlbedoUvOffset(xml.Get<Vector2>("AlbedoUvOffset")); }

    if (xml.Contains("AlbedoUvMultiply"))
    { SetAlbedoUvMultiply(xml.Get<Vector2>("AlbedoUvMultiply")); }

    if (xml.Contains("RoughnessTexture"))
    { SetRoughnessTexture(
         Resources::Load<Texture2D>(xml.Get<GUID>("RoughnessTexture")).Get()); }

    if (xml.Contains("MetalnessTexture"))
    { SetMetalnessTexture(
         Resources::Load<Texture2D>(xml.Get<GUID>("MetalnessTexture")).Get()); }

    if (xml.Contains("NormalMapTexture"))
    { SetNormalMapTexture(
         Resources::Load<Texture2D>(xml.Get<GUID>("NormalMapTexture")).Get()); }

    if (xml.Contains("NormalMapUvOffset"))
    { SetNormalMapUvOffset(xml.Get<Vector2>("NormalMapUvOffset")); }

    if (xml.Contains("NormalMapUvMultiply"))
    { SetNormalMapUvMultiply(xml.Get<Vector2>("NormalMapUvMultiply")); }

    if (xml.Contains("NormalMapMultiplyFactor"))
    { SetNormalMapMultiplyFactor(xml.Get<float>("NormalMapMultiplyFactor")); }

    RH<Shader> vShader;
    if (xml.Contains("VertexShader"))
    {  vShader = Resources::Load<Shader>(xml.Get<GUID>("VertexShader"));  }

    RH<Shader> fShader;
    if (xml.Contains("FragmentShader"))
    { fShader = Resources::Load<Shader>(xml.Get<GUID>("FragmentShader")); }

    if (xml.Contains("LineWidth"))
    { SetLineWidth(xml.Get<float>("LineWidth")); }

    if (xml.Contains("RenderWireframe"))
    { SetRenderWireframe(xml.Get<bool>("RenderWireframe")); }

    if (xml.Contains("CullFace"))
    { SetCullFace(xml.Get<GL::CullFaceExt>("CullFace")); }

    if (vShader && fShader)
    {
        ShaderProgram *newSp =
                ShaderProgramFactory::Get(vShader.Get()->GetResourceFilepath(),
                                          fShader.Get()->GetResourceFilepath());
        SetShaderProgram(newSp);
    }
}

void Material::ExportXML(XMLNode *xmlInfo) const
{
    Asset::ExportXML(xmlInfo);

    xmlInfo->Set("RenderPass",              GetRenderPass());
    xmlInfo->Set("AlbedoColor",             GetAlbedoColor());
    xmlInfo->Set("Roughness",               GetRoughness());
    xmlInfo->Set("Metalness",               GetMetalness());
    xmlInfo->Set("ReceivesLighting",        GetReceivesLighting());
    xmlInfo->Set("AlbedoUvMultiply",        GetAlbedoUvMultiply());
    xmlInfo->Set("AlbedoUvOffset",          GetAlbedoUvOffset());
    xmlInfo->Set("NormalMapUvMultiply",     GetNormalMapUvMultiply());
    xmlInfo->Set("NormalMapUvOffset",       GetNormalMapUvOffset());
    xmlInfo->Set("NormalMapMultiplyFactor", GetNormalMapMultiplyFactor());
    xmlInfo->Set("LineWidth",               GetLineWidth());
    xmlInfo->Set("RenderWireframe",         IsRenderWireframe());
    xmlInfo->Set("CullFace",                GetCullFace());

    Texture2D* albedoTex = GetAlbedoTexture();
    xmlInfo->Set("AlbedoTexture",
                 albedoTex ? albedoTex->GetGUID() : GUID::Empty());
    Texture2D* roughnessTex = GetRoughnessTexture();
    xmlInfo->Set("RoughnessTexture",
                 roughnessTex ? roughnessTex->GetGUID() : GUID::Empty());
    Texture2D* metalnessTex = GetMetalnessTexture();
    xmlInfo->Set("MetalnessTexture",
                 metalnessTex ? metalnessTex->GetGUID() : GUID::Empty());
    Texture2D* normalTex = GetNormalMapTexture();
    xmlInfo->Set("NormalMapTexture",
                 normalTex ? normalTex->GetGUID() : GUID::Empty());

    ShaderProgram *sp = GetShaderProgram();
    Shader* vShader = (sp ? sp->GetVertexShader()   : nullptr);
    Shader* fShader = (sp ? sp->GetFragmentShader() : nullptr);
    xmlInfo->Set("VertexShader",   vShader->GetGUID());
    xmlInfo->Set("FragmentShader", fShader->GetGUID());
}