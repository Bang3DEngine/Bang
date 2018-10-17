#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/GL.h"
#include "Bang/ICloneable.h"
#include "Bang/IEventsResource.h"
#include "Bang/MetaNode.h"
#include "Bang/NeededUniformFlags.h"
#include "Bang/RenderPass.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"

namespace Bang
{
class IEventsResource;
class Path;
class Resource;
class ShaderProgram;
class Texture2D;

class Material : public Asset,
                 public ICloneable,
                 public EventListener<IEventsResource>
{
    ASSET(Material)
    ICLONEABLE(Material)

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
    void SetRenderPass(RenderPass renderPass);
    void SetCullFace(GL::CullFaceExt cullFace);
    void SetRenderWireframe(bool renderWireframe);
    void SetLineWidth(float w);

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
    RenderPass GetRenderPass() const;
    bool IsRenderWireframe() const;
    GL::CullFaceExt GetCullFace() const;
    float GetLineWidth() const;

    virtual void Bind() const;
    virtual void UnBind() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // IEventsResource
    void OnResourceChanged(Resource *changedResource) override;

    // Resource
    void Import(const Path &materialFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    RH<Texture2D> p_albedoTexture;
    RH<Texture2D> p_roughnessTexture;
    RH<Texture2D> p_metalnessTexture;
    RH<Texture2D> p_normalMapTexture;
    RH<ShaderProgram> p_shaderProgram;

    float m_lineWidth = 1.0f;
    float m_normalMapMultiplyFactor = 1.0f;
    RenderPass m_renderPass = RenderPass::SCENE;
    Color m_albedoColor = Color::White;
    Vector2 m_albedoUvOffset = Vector2::Zero;
    Vector2 m_albedoUvMultiply = Vector2::One;
    Vector2 m_normalMapUvOffset = Vector2::Zero;
    Vector2 m_normalMapUvMultiply = Vector2::One;
    GL::CullFaceExt m_cullFace = GL::CullFaceExt::BACK;
    float m_roughness = 1.0f;
    float m_metalness = 0.5f;
    NeededUniformFlags m_neededUniforms = NeededUniformFlag::ALL;
    bool m_receivesLighting = true;
    bool m_renderWireframe = false;

    Material();
    virtual ~Material() override;
};
}

#endif  // MATERIAL_H
