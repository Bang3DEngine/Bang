#include "Bang/Material.h"

#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/Extensions.h"
#include "Bang/Flags.h"
#include "Bang/GLUniforms.h"
#include "Bang/GUID.h"
#include "Bang/IEventsAsset.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Shader.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureFactory.h"

namespace Bang
{
class Path;
class Asset;
}  // namespace Bang

using namespace Bang;

Material::Material()
{
    SetShaderProgram(ShaderProgramFactory::GetDefault());
}

Material::~Material()
{
}

NeededUniformFlags &Material::GetNeededUniforms()
{
    return m_neededUniforms;
}

const NeededUniformFlags &Material::GetNeededUniforms() const
{
    return m_neededUniforms;
}

void Material::SetNeededUniforms(const NeededUniformFlags &neededUniformFlags)
{
    m_neededUniforms = neededUniformFlags;
}

void Material::SetAlbedoUvOffset(const Vector2 &albedoUvOffset)
{
    if (albedoUvOffset != GetAlbedoUvOffset())
    {
        m_albedoUvOffset = albedoUvOffset;
        PropagateAssetChanged();
    }
}

void Material::SetAlbedoUvMultiply(const Vector2 &albedoUvMultiply)
{
    if (albedoUvMultiply != GetAlbedoUvMultiply())
    {
        m_albedoUvMultiply = albedoUvMultiply;
        PropagateAssetChanged();
    }
}

void Material::SetNormalMapUvOffset(const Vector2 &normalMapUvOffset)
{
    if (normalMapUvOffset != GetNormalMapUvOffset())
    {
        m_normalMapUvOffset = normalMapUvOffset;
        PropagateAssetChanged();
    }
}

void Material::SetNormalMapUvMultiply(const Vector2 &normalMapUvMultiply)
{
    if (normalMapUvMultiply != GetNormalMapUvMultiply())
    {
        m_normalMapUvMultiply = normalMapUvMultiply;
        PropagateAssetChanged();
    }
}

void Material::SetNormalMapMultiplyFactor(float normalMapMultiplyFactor)
{
    if (normalMapMultiplyFactor != GetNormalMapMultiplyFactor())
    {
        m_normalMapMultiplyFactor = normalMapMultiplyFactor;
        PropagateAssetChanged();
    }
}

void Material::SetShaderProgram(ShaderProgram *program)
{
    if (program != GetShaderProgram())
    {
        if (GetShaderProgram())
        {
            GetShaderProgram()->EventEmitter<IEventsAsset>::UnRegisterListener(
                this);
        }

        p_shaderProgram.Set(program);

        if (GetShaderProgram())
        {
            SetShaderProgramProperties(
                GetShaderProgram()->GetLoadedProperties());
            GetShaderProgram()->EventEmitter<IEventsAsset>::RegisterListener(
                this);
        }

        PropagateAssetChanged();
    }
}

void Material::SetReceivesLighting(bool receivesLighting)
{
    if (receivesLighting != GetReceivesLighting())
    {
        m_receivesLighting = receivesLighting;
        PropagateAssetChanged();
    }
}

void Material::SetRoughness(float roughness)
{
    if (roughness != GetRoughness())
    {
        m_roughness = roughness;
        PropagateAssetChanged();
    }
}

void Material::SetMetalness(float metalness)
{
    if (metalness != GetMetalness())
    {
        m_metalness = metalness;
        PropagateAssetChanged();
    }
}

void Material::SetAlbedoColor(const Color &albedoColor)
{
    if (albedoColor != GetAlbedoColor())
    {
        m_albedoColor = albedoColor;
        PropagateAssetChanged();
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
            GetAlbedoTexture()->EventEmitter<IEventsAsset>::UnRegisterListener(
                this);
        }

        p_albedoTexture.Set(texture);
        if (ShaderProgram *sp = GetShaderProgram())
        {
            sp->SetTexture2D(GLUniforms::UniformName_AlbedoTexture,
                             GetAlbedoTexture());
        }

        if (GetAlbedoTexture())
        {
            GetAlbedoTexture()->EventEmitter<IEventsAsset>::RegisterListener(
                this);
        }

        PropagateAssetChanged();
    }
}

void Material::SetNormalMapTexture(Texture2D *texture)
{
    if (texture != GetNormalMapTexture())
    {
        if (GetNormalMapTexture())
        {
            GetNormalMapTexture()
                ->EventEmitter<IEventsAsset>::UnRegisterListener(this);
        }

        p_normalMapTexture.Set(texture);
        if (ShaderProgram *sp = GetShaderProgram())
        {
            sp->SetTexture2D(GLUniforms::UniformName_NormalMapTexture,
                             GetNormalMapTexture());
        }

        if (GetNormalMapTexture())
        {
            GetNormalMapTexture()->EventEmitter<IEventsAsset>::RegisterListener(
                this);
        }

        PropagateAssetChanged();
    }
}

void Material::SetShaderProgramProperties(
    const ShaderProgramProperties &spProps)
{
    m_shaderProgramProperties = spProps;
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

ShaderProgramProperties &Material::GetShaderProgramProperties()
{
    return m_shaderProgramProperties;
}

const ShaderProgramProperties &Material::GetShaderProgramProperties() const
{
    return m_shaderProgramProperties;
}

void Material::Bind() const
{
    ShaderProgram *sp = GetShaderProgram();
    if (sp && sp->IsLinked())
    {
        sp->Bind();

        const GL::CullFaceExt &cullFace =
            GetShaderProgramProperties().GetCullFace();
        if (cullFace != GL::CullFaceExt::NONE)
        {
            GL::Enable(GL::Enablable::CULL_FACE);
            GL::SetCullFace(SCAST<GL::Face>(cullFace));
        }
        else
        {
            GL::Disable(GL::Enablable::CULL_FACE);
        }

        GL::SetWireframe(GetShaderProgramProperties().GetWireframe());
        GL::LineWidth(GetShaderProgramProperties().GetLineWidth());
        GL::PointSize(GetShaderProgramProperties().GetLineWidth());

        BindMaterialUniforms(sp);
    }
}

void Material::BindMaterialUniforms(ShaderProgram *sp) const
{
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

void Material::OnAssetChanged(Asset *)
{
    PropagateAssetChanged();
}

void Material::Import(const Path &materialFilepath)
{
    ImportMetaFromFile(materialFilepath);
}

void Material::Reflect()
{
    Asset::Reflect();

    BANG_REFLECT_VAR_MEMBER(
        Material, "Albedo Color", SetAlbedoColor, GetAlbedoColor);
    BANG_REFLECT_VAR_ASSET("Albedo Texture",
                           SetAlbedoTexture,
                           GetAlbedoTexture,
                           Texture2D,
                           BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(true));
    BANG_REFLECT_VAR_MEMBER(
        Material, "Albedo Uv Offset", SetAlbedoUvOffset, GetAlbedoUvOffset);
    BANG_REFLECT_VAR_MEMBER(Material,
                            "Albedo Uv Multiply",
                            SetAlbedoUvMultiply,
                            GetAlbedoUvMultiply);

    BANG_REFLECT_VAR_MEMBER(Material,
                            "Normal Map Factor",
                            SetNormalMapMultiplyFactor,
                            GetNormalMapMultiplyFactor);
    BANG_REFLECT_VAR_ASSET("Normal Texture",
                           SetNormalMapTexture,
                           GetNormalMapTexture,
                           Texture2D,
                           BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(true));
    BANG_REFLECT_VAR_MEMBER(Material,
                            "Normal Map Uv Offset",
                            SetNormalMapUvOffset,
                            GetNormalMapUvOffset);
    BANG_REFLECT_VAR_MEMBER(Material,
                            "Normal Map Uv Multiply",
                            SetNormalMapUvMultiply,
                            GetNormalMapUvMultiply);

    BANG_REFLECT_VAR_MEMBER_HINTED(Material,
                                   "Roughness",
                                   SetRoughness,
                                   GetRoughness,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 1.0f));
    BANG_REFLECT_VAR_ASSET("Roughness Texture",
                           SetRoughnessTexture,
                           GetRoughnessTexture,
                           Texture2D,
                           BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(true));

    BANG_REFLECT_VAR_MEMBER_HINTED(Material,
                                   "Metalness",
                                   SetMetalness,
                                   GetMetalness,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 1.0f));
    BANG_REFLECT_VAR_ASSET("Metalness Texture",
                           SetMetalnessTexture,
                           GetMetalnessTexture,
                           Texture2D,
                           BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(true));

    BANG_REFLECT_VAR_MEMBER(Material,
                            "Receives Lighting",
                            SetReceivesLighting,
                            GetReceivesLighting);

    BANG_REFLECT_VAR_ENUM_FLAGS(
        "Needed Uniforms", SetNeededUniforms, GetNeededUniforms);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Needed Uniforms", "Model", NeededUniformFlag::MODEL);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Needed Uniforms", "Model Inv", NeededUniformFlag::MODEL_INV);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Needed Uniforms", "Normal", NeededUniformFlag::NORMAL);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Needed Uniforms", "PVM", NeededUniformFlag::PVM);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Needed Uniforms", "PVM Inv", NeededUniformFlag::PVM_INV);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Needed Uniforms", "Skyboxes", NeededUniformFlag::SKYBOXES);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE("Needed Uniforms",
                                       "Material Albedo",
                                       NeededUniformFlag::MATERIAL_ALBEDO);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Needed Uniforms", "Material PBR", NeededUniformFlag::MATERIAL_PBR);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Needed Uniforms", "Time", NeededUniformFlag::TIME);

    Array<String> extensions;
    extensions.PushBack(Extensions::GetShaderProgramExtension());
    extensions.PushBack(Extensions::GetUnifiedShaderExtension());

    BANG_REFLECT_VAR_ASSET("Shader Program",
                           SetShaderProgram,
                           GetShaderProgram,
                           ShaderProgram,
                           BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(false) +
                               BANG_REFLECT_HINT_EXTENSIONS(extensions));
}

void Material::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Asset::CloneInto(clone, cloneGUID);

    Material *matClone = SCAST<Material *>(clone);
    matClone->SetShaderProgramProperties(GetShaderProgramProperties());
}
