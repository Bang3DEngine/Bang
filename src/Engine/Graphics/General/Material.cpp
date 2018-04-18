#include "Bang/Material.h"

#include "Bang/Paths.h"
#include "Bang/Shader.h"
#include "Bang/XMLNode.h"
#include "Bang/Vector4.h"
#include "Bang/Texture.h"
#include "Bang/Texture2D.h"
#include "Bang/Resources.h"
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

void Material::SetUvOffset(const Vector2 &uvOffset)
{
    if (uvOffset != GetUvOffset())
    {
        m_uvOffset = uvOffset;
        PropagateMaterialChanged();
    }
}

void Material::SetUvMultiply(const Vector2 &uvMultiply)
{
    if (uvMultiply != GetUvMultiply())
    {
        m_uvMultiply = uvMultiply;
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

void Material::SetTexture(Texture2D* texture)
{
    if (p_texture.Get() != texture)
    {
        if (GetTexture())
        {
            GetTexture()->EventEmitter<ITextureChangedListener>::
                          UnRegisterListener(this);
        }

        p_texture.Set(texture);
        ShaderProgram *sp = GetShaderProgram();
        if (sp) { sp->SetTexture2D("B_Texture0", GetTexture()); }

        if (GetTexture())
        {
            GetTexture()->EventEmitter<ITextureChangedListener>::
                          RegisterListener(this);
        }
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

void Material::SetAlbedoColor(const Color &albedoColor)
{
    if (albedoColor != GetAlbedoColor())
    {
        m_albedoColor = albedoColor;
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

const Vector2 &Material::GetUvOffset() const { return m_uvOffset; }
const Vector2 &Material::GetUvMultiply() const { return m_uvMultiply; }
ShaderProgram* Material::GetShaderProgram() const { return p_shaderProgram.Get(); }
Texture2D* Material::GetTexture() const { return p_texture.Get(); }
bool Material::GetReceivesLighting() const { return m_receivesLighting; }
float Material::GetRoughness() const { return m_roughness; }
const Color& Material::GetAlbedoColor() const { return m_albedoColor; }
RenderPass Material::GetRenderPass() const { return m_renderPass; }

void Material::Bind() const
{
    ShaderProgram *sp = GetShaderProgram();
    if (!sp) { return; }
    sp->Bind();

    sp->SetVector2("B_UvOffset",              GetUvOffset(),         false);
    sp->SetVector2("B_UvMultiply",            GetUvMultiply(),       false);
    sp->SetColor("B_MaterialAlbedoColor",     GetAlbedoColor(),      false);
    sp->SetFloat("B_MaterialRoughness",       GetRoughness(),        false);
    sp->SetBool("B_MaterialReceivesLighting", GetReceivesLighting(), false);

    float alphaCutoff = GetTexture() ? GetTexture()->GetAlphaCutoff() : -1.0f;
    sp->SetTexture2D("B_Texture0",  GetTexture(),             false);
    sp->SetFloat("B_AlphaCutoff",   alphaCutoff,              false);
    sp->SetBool("B_HasTexture",    (GetTexture() != nullptr), false);
}

void Material::UnBind() const
{
    // ShaderProgram *sp = GetShaderProgram();
    // if (!sp) { return; }
    // sp->UnBind();
}

void Material::CloneInto(ICloneable *clone) const
{
    Material *matClone = Cast<Material*>(clone);

    matClone->SetUvOffset(GetUvOffset());
    matClone->SetUvMultiply(GetUvMultiply());
    matClone->SetShaderProgram(GetShaderProgram());
    matClone->SetAlbedoColor(GetAlbedoColor());
    matClone->SetReceivesLighting(GetReceivesLighting());
    matClone->SetRoughness(GetRoughness());
    matClone->SetTexture(GetTexture());
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

    if (xml.Contains("ReceivesLighting"))
    { SetReceivesLighting(xml.Get<bool>("ReceivesLighting")); }

    if (xml.Contains("UvOffset"))
    { SetUvOffset(xml.Get<Vector2>("UvOffset")); }

    if (xml.Contains("UvMultiply"))
    { SetUvMultiply(xml.Get<Vector2>("UvMultiply")); }

    if (xml.Contains("Texture"))
    { SetTexture(Resources::Load<Texture2D>(xml.Get<GUID>("Texture")).Get()); }

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

    xmlInfo->Set("RenderPass",       GetRenderPass());
    xmlInfo->Set("AlbedoColor",     GetAlbedoColor());
    xmlInfo->Set("Roughness",        GetRoughness());
    xmlInfo->Set("ReceivesLighting", GetReceivesLighting());
    xmlInfo->Set("UvMultiply",       GetUvMultiply());
    xmlInfo->Set("UvOffset",         GetUvOffset());

    Texture2D* tex = GetTexture();
    xmlInfo->Set("Texture",  tex ? tex->GetGUID() : GUID::Empty());

    ShaderProgram *sp = GetShaderProgram();
    Shader* vShader = (sp ? sp->GetVertexShader()   : nullptr);
    Shader* fShader = (sp ? sp->GetFragmentShader() : nullptr);
    xmlInfo->Set("VertexShader",   vShader->GetGUID());
    xmlInfo->Set("FragmentShader", fShader->GetGUID());
}
