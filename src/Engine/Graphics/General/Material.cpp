#include "Bang/Material.h"

#include "Bang/Paths.h"
#include "Bang/Shader.h"
#include "Bang/XMLNode.h"
#include "Bang/Vector4.h"
#include "Bang/Texture.h"
#include "Bang/Texture2D.h"
#include "Bang/Resources.h"
#include "Bang/TextureFactory.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

Material::Material()
{
    SetShaderProgram( ShaderProgramFactory::GetDefault() );
}

Material::~Material()
{
}

void Material::SetAlbedoUvOffset(const Vector2 &albedoUvOffset)
{
    if (albedoUvOffset != GetAlbedoUvOffset())
    {
        m_albedoUvOffset = albedoUvOffset;
        PropagateMaterialChanged();
    }
}

void Material::SetAlbedoUvMultiply(const Vector2 &albedoUvMultiply)
{
    if (albedoUvMultiply != GetAlbedoUvMultiply())
    {
        m_albedoUvMultiply = albedoUvMultiply;
        PropagateMaterialChanged();
    }
}

void Material::SetNormalMapUvOffset(const Vector2 &normalMapUvOffset)
{
    if (normalMapUvOffset != GetNormalMapUvOffset())
    {
        m_normalMapUvOffset = normalMapUvOffset;
        PropagateMaterialChanged();
    }
}

void Material::SetNormalMapUvMultiply(const Vector2 &normalMapUvMultiply)
{
    if (normalMapUvMultiply != GetNormalMapUvMultiply())
    {
        m_normalMapUvMultiply = normalMapUvMultiply;
        PropagateMaterialChanged();
    }
}

void Material::SetShaderProgram(ShaderProgram* program)
{
    if (p_shaderProgram.Get() != program)
    {
        p_shaderProgram.Set(program);
        PropagateMaterialChanged();
    }
}

void Material::SetReceivesLighting(bool receivesLighting)
{
    if (receivesLighting != GetReceivesLighting())
    {
        m_receivesLighting = receivesLighting;
        PropagateMaterialChanged();
    }
}

void Material::SetRoughness(float roughness)
{
    if (roughness != GetRoughness())
    {
        m_roughness = roughness;
        PropagateMaterialChanged();
    }
}

void Material::SetMetalness(float metalness)
{
    if (metalness != GetMetalness())
    {
        m_metalness = metalness;
        PropagateMaterialChanged();
    }
}

void Material::SetAlbedoColor(const Color &albedoColor)
{
    if (albedoColor != GetAlbedoColor())
    {
        m_albedoColor = albedoColor;
        PropagateMaterialChanged();
    }
}

void Material::SetAlbedoTexture(Texture2D* texture)
{
    if (p_albedoTexture.Get() != texture)
    {
        if (GetAlbedoTexture())
        {
            GetAlbedoTexture()->EventEmitter<ITextureChangedListener>::
                                UnRegisterListener(this);
        }

        p_albedoTexture.Set(texture);
        ShaderProgram *sp = GetShaderProgram();
        if (sp) { sp->SetTexture2D("B_AlbedoTexture", GetAlbedoTexture(), false); }

        if (GetAlbedoTexture())
        {
            GetAlbedoTexture()->EventEmitter<ITextureChangedListener>::
                                RegisterListener(this);
        }

        PropagateMaterialChanged();
    }
}

void Material::SetNormalMapTexture(Texture2D *texture)
{
    if (texture != GetNormalMapTexture())
    {
        if (GetNormalMapTexture())
        {
            GetNormalMapTexture()->EventEmitter<ITextureChangedListener>::
                                   UnRegisterListener(this);
        }

        p_normalMapTexture.Set(texture);
        ShaderProgram *sp = GetShaderProgram();
        if (sp) { sp->SetTexture2D("B_NormalMapTexture", GetNormalMapTexture(),
                                   false); }

        if (GetNormalMapTexture())
        {
            GetNormalMapTexture()->EventEmitter<ITextureChangedListener>::
                                   RegisterListener(this);
        }

        PropagateMaterialChanged();
    }
}

void Material::SetRenderPass(RenderPass renderPass)
{
    if (renderPass != GetRenderPass())
    {
        m_renderPass = renderPass;
        PropagateMaterialChanged();
    }
}

const Vector2 &Material::GetAlbedoUvOffset() const { return m_albedoUvOffset; }
const Vector2 &Material::GetAlbedoUvMultiply() const { return m_albedoUvMultiply; }
const Vector2 &Material::GetNormalMapUvOffset() const { return m_normalMapUvOffset; }
const Vector2 &Material::GetNormalMapUvMultiply() const { return m_normalMapUvMultiply; }
ShaderProgram* Material::GetShaderProgram() const { return p_shaderProgram.Get(); }
Texture2D* Material::GetAlbedoTexture() const { return p_albedoTexture.Get(); }
bool Material::GetReceivesLighting() const { return m_receivesLighting; }
float Material::GetMetalness() const { return m_metalness; }
float Material::GetRoughness() const { return m_roughness; }
const Color& Material::GetAlbedoColor() const { return m_albedoColor; }
Texture2D *Material::GetNormalMapTexture() const { return p_normalMapTexture.Get(); }
RenderPass Material::GetRenderPass() const { return m_renderPass; }

void Material::Bind() const
{
    ShaderProgram *sp = GetShaderProgram();
    if (!sp) { return; }
    sp->Bind();

    sp->SetColor("B_MaterialAlbedoColor",      GetAlbedoColor(),         false);
    sp->SetFloat("B_MaterialRoughness",        GetRoughness(),           false);
    sp->SetFloat("B_MaterialMetalness",        GetMetalness(),           false);
    sp->SetBool("B_MaterialReceivesLighting",  GetReceivesLighting(),    false);
    sp->SetVector2("B_AlbedoUvOffset",         GetAlbedoUvOffset(),      false);
    sp->SetVector2("B_AlbedoUvMultiply",       GetAlbedoUvMultiply(),    false);
    sp->SetVector2("B_NormalMapUvOffset",      GetNormalMapUvOffset(),   false);
    sp->SetVector2("B_NormalMapUvMultiply",    GetNormalMapUvMultiply(), false);

    if (Texture2D *albedoTex = GetAlbedoTexture())
    {
        sp->SetTexture2D("B_AlbedoTexture",  albedoTex,                   false);
        sp->SetFloat("B_AlphaCutoff",        albedoTex->GetAlphaCutoff(), false);
        sp->SetBool("B_HasAlbedoTexture",    true,                        false);
    }
    else
    {
        sp->SetTexture2D("B_AlbedoTexture",  nullptr, false);
        sp->SetFloat("B_AlphaCutoff",        -1.0f,   false);
        sp->SetBool("B_HasAlbedoTexture",    false,   false);
    }

    if (GetNormalMapTexture())
    {
        sp->SetTexture2D("B_NormalMapTexture",  GetNormalMapTexture(), false);
        sp->SetBool("B_HasNormalMapTexture",    true,                  false);
    }
    else
    {
        sp->SetTexture2D("B_NormalMapTexture", nullptr, false);
        sp->SetBool("B_HasNormalMapTexture",   false,   false);
    }
}

void Material::UnBind() const
{
    ShaderProgram *sp = GetShaderProgram();
    if (!sp) { return; }
    sp->UnBind();
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
    matClone->SetRenderPass(GetRenderPass());
}

void Material::OnTextureChanged(const Texture*)
{
    PropagateMaterialChanged();
}

void Material::PropagateMaterialChanged()
{
    EventEmitter<IMaterialChangedListener>::PropagateToListeners(
                &IMaterialChangedListener::OnMaterialChanged, this);
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

    if (xml.Contains("NormalMapTexture"))
    { SetNormalMapTexture(
         Resources::Load<Texture2D>(xml.Get<GUID>("NormalMapTexture")).Get()); }

    if (xml.Contains("NormalMapUvOffset"))
    { SetNormalMapUvOffset(xml.Get<Vector2>("NormalMapUvOffset")); }

    if (xml.Contains("NormalMapUvMultiply"))
    { SetNormalMapUvMultiply(xml.Get<Vector2>("NormalMapUvMultiply")); }

    RH<Shader> vShader;
    if (xml.Contains("VertexShader"))
    {  vShader = Resources::Load<Shader>(xml.Get<GUID>("VertexShader"));  }

    RH<Shader> fShader;
    if (xml.Contains("FragmentShader"))
    { fShader = Resources::Load<Shader>(xml.Get<GUID>("FragmentShader")); }

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

    xmlInfo->Set("RenderPass",          GetRenderPass());
    xmlInfo->Set("AlbedoColor",         GetAlbedoColor());
    xmlInfo->Set("Roughness",           GetRoughness());
    xmlInfo->Set("Metalness",           GetMetalness());
    xmlInfo->Set("ReceivesLighting",    GetReceivesLighting());
    xmlInfo->Set("AlbedoUvMultiply",    GetAlbedoUvMultiply());
    xmlInfo->Set("AlbedoUvOffset",      GetAlbedoUvOffset());
    xmlInfo->Set("NormalMapUvMultiply", GetNormalMapUvMultiply());
    xmlInfo->Set("NormalMapUvOffset",   GetNormalMapUvOffset());

    Texture2D* albedoTex = GetAlbedoTexture();
    xmlInfo->Set("AlbedoTexture",
                 albedoTex ? albedoTex->GetGUID() : GUID::Empty());
    Texture2D* normalTex = GetNormalMapTexture();
    xmlInfo->Set("NormalMapTexture",
                 normalTex ? normalTex->GetGUID() : GUID::Empty());

    ShaderProgram *sp = GetShaderProgram();
    Shader* vShader = (sp ? sp->GetVertexShader()   : nullptr);
    Shader* fShader = (sp ? sp->GetFragmentShader() : nullptr);
    xmlInfo->Set("VertexShader",   vShader->GetGUID());
    xmlInfo->Set("FragmentShader", fShader->GetGUID());
}
