#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Asset.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/GL.h"
#include "Bang/ICloneable.h"
#include "Bang/IEventsAsset.h"
#include "Bang/MetaNode.h"
#include "Bang/NeededUniformFlags.h"
#include "Bang/RenderPass.h"
#include "Bang/ShaderProgramProperties.h"
#include "Bang/String.h"

namespace Bang
{
class IEventsAsset;
class Path;
class Asset;
class ShaderProgram;
class Texture2D;

class Material : public Asset, public EventListener<IEventsAsset>
{
    ASSET(Material)

public:
    enum class UniformFlags
    {
        Matrices,
        InverseMatrices,

    };

    void SetAlbedoUvOffset(const Vector2 &albedoUvOffset);
    void SetAlbedoUvMultiply(const Vector2 &albedoUvMultiply);
    void SetNormalMapUvOffset(const Vector2 &normalMapUvOffset);
    void SetNormalMapUvMultiply(const Vector2 &normalMapUvMultiply);
    void SetNormalMapMultiplyFactor(float normalMapMultiplyFactor);
    void SetShaderProgram(ShaderProgram *program);
    void SetReceivesLighting(bool receivesLighting);
    void SetRoughness(float roughness);
    void SetMetalness(float metalness);
    void SetAlbedoTexture(Texture2D *albedoTexture);
    void SetAlbedoColor(const Color &albedoColor);
    void SetRoughnessTexture(Texture2D *roughnessTexture);
    void SetMetalnessTexture(Texture2D *metalnessTexture);
    void SetNormalMapTexture(Texture2D *normalMapTexture);
    void SetShaderProgramProperties(const ShaderProgramProperties &spProps);
    void SetNeededUniforms(const NeededUniformFlags &neededUniformFlags);
    void BindMaterialUniforms(ShaderProgram *sp) const;

    NeededUniformFlags &GetNeededUniforms();
    const NeededUniformFlags &GetNeededUniforms() const;
    const Vector2 &GetAlbedoUvOffset() const;
    const Vector2 &GetAlbedoUvMultiply() const;
    const Vector2 &GetNormalMapUvOffset() const;
    const Vector2 &GetNormalMapUvMultiply() const;
    float GetNormalMapMultiplyFactor() const;
    ShaderProgram *GetShaderProgram() const;
    bool GetReceivesLighting() const;
    float GetMetalness() const;
    float GetRoughness() const;
    Texture2D *GetAlbedoTexture() const;
    const Color &GetAlbedoColor() const;
    Texture2D *GetRoughnessTexture() const;
    Texture2D *GetMetalnessTexture() const;
    Texture2D *GetNormalMapTexture() const;
    ShaderProgramProperties &GetShaderProgramProperties();
    const ShaderProgramProperties &GetShaderProgramProperties() const;

    virtual void Bind() const;
    virtual void UnBind() const;

    // IEventsAsset
    void OnAssetChanged(Asset *changedAsset) override;

    // Asset
    void Import(const Path &materialFilepath) override;

    // Serializable
    virtual void Reflect() override;

protected:
    AH<Texture2D> p_albedoTexture;
    AH<Texture2D> p_roughnessTexture;
    AH<Texture2D> p_metalnessTexture;
    AH<Texture2D> p_normalMapTexture;
    AH<ShaderProgram> p_shaderProgram;
    ShaderProgramProperties m_shaderProgramProperties;

    float m_normalMapMultiplyFactor = 1.0f;
    Color m_albedoColor = Color::White();
    Vector2 m_albedoUvOffset = Vector2::Zero();
    Vector2 m_albedoUvMultiply = Vector2::One();
    Vector2 m_normalMapUvOffset = Vector2::Zero();
    Vector2 m_normalMapUvMultiply = Vector2::One();
    float m_roughness = 1.0f;
    float m_metalness = 0.5f;
    NeededUniformFlags m_neededUniforms = NeededUniformFlag::ALL;
    bool m_receivesLighting = true;

    Material();
    virtual ~Material() override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

private:
};
}

#endif  // MATERIAL_H
