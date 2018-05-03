#ifndef MATERIAL_H
#define MATERIAL_H

#include "Bang/GL.h"
#include "Bang/Asset.h"
#include "Bang/Color.h"
#include "Bang/Vector2.h"
#include "Bang/RenderPass.h"
#include "Bang/ResourceHandle.h"
#include "Bang/ITextureChangedListener.h"
#include "Bang/IMaterialChangedListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture2D;
FORWARD class ShaderProgram;

class Material : public Asset,
                 public ICloneable,
                 public ITextureChangedListener,
                 public EventEmitter<IMaterialChangedListener>
{
    ASSET(Material)

public:
    void SetAlbedoUvOffset(const Vector2& albedoUvOffset);
    void SetAlbedoUvMultiply(const Vector2& albedoUvMultiply);
    void SetNormalMapUvOffset(const Vector2& normalMapUvOffset);
    void SetNormalMapUvMultiply(const Vector2& normalMapUvMultiply);
    void SetShaderProgram(ShaderProgram *program);
    void SetReceivesLighting(bool receivesLighting);
    void SetRoughness(float roughness);
    void SetMetalness(float metalness);
    void SetAlbedoTexture(Texture2D *texture);
    void SetAlbedoColor(const Color &albedoColor);
    void SetNormalMapTexture(Texture2D *texture);
    void SetRenderPass(RenderPass renderPass);
    void SetCullFace(GL::CullFaceExt cullFace);
    void SetRenderWireframe(bool renderWireframe);
    void SetLineWidth(float w);

    const Vector2& GetAlbedoUvOffset() const;
    const Vector2& GetAlbedoUvMultiply() const;
    const Vector2& GetNormalMapUvOffset() const;
    const Vector2& GetNormalMapUvMultiply() const;
    ShaderProgram* GetShaderProgram() const;
    bool GetReceivesLighting() const;
    float GetMetalness() const;
    float GetRoughness() const;
    Texture2D* GetAlbedoTexture() const;
    const Color& GetAlbedoColor() const;
    Texture2D* GetNormalMapTexture() const;
    RenderPass GetRenderPass() const;
    bool IsRenderWireframe() const;
    GL::CullFaceExt GetCullFace() const;
    float GetLineWidth() const;

    virtual void Bind() const;
    virtual void UnBind() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // ITextureChangedListener
    void OnTextureChanged(const Texture *changedTexture) override;

    // Resource
    void Import(const Path &materialFilepath) override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

protected:
    RH<Texture2D> p_albedoTexture;
    RH<Texture2D> p_normalMapTexture;
    RH<ShaderProgram> p_shaderProgram;

    RenderPass m_renderPass       = RenderPass::Scene;
    Color m_albedoColor           = Color::White;
    float m_roughness             = 1.0f;
    float m_metalness             = 0.5f;
    bool m_receivesLighting       = true;
    Vector2 m_albedoUvOffset      = Vector2::Zero;
    Vector2 m_albedoUvMultiply    = Vector2::One;
    Vector2 m_normalMapUvOffset   = Vector2::Zero;
    Vector2 m_normalMapUvMultiply = Vector2::One;
    float m_lineWidth             = 1.0f;
    GL::CullFaceExt m_cullFace    = GL::CullFaceExt::Back;
    bool m_renderWireframe        = false;

    Material();
    virtual ~Material();

    void PropagateMaterialChanged();
};

NAMESPACE_BANG_END

#endif // MATERIAL_H
