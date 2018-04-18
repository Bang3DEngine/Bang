#ifndef MATERIAL_H
#define MATERIAL_H

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
    void SetUvOffset(const Vector2& uvOffset);
    void SetUvMultiply(const Vector2& uvMultiply);
    void SetShaderProgram(ShaderProgram *program);
    void SetTexture(Texture2D *texture);
    void SetReceivesLighting(bool receivesLighting);
    void SetRoughness(float roughness);
    void SetAlbedoColor(const Color &albedoColor);
    void SetRenderPass(RenderPass renderPass);

    const Vector2& GetUvOffset() const;
    const Vector2& GetUvMultiply() const;
    ShaderProgram* GetShaderProgram() const;
    Texture2D* GetTexture() const;
    bool GetReceivesLighting() const;
    float GetRoughness() const;
    const Color& GetAlbedoColor() const;
    RenderPass GetRenderPass() const;

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
    RH<Texture2D> p_texture;
    RH<ShaderProgram> p_shaderProgram;

    RenderPass m_renderPass = RenderPass::Scene;
    Color m_albedoColor    = Color::White;
    float m_roughness       = 0.5f;
    bool m_receivesLighting = true;
    Vector2 m_uvOffset      = Vector2::Zero;
    Vector2 m_uvMultiply    = Vector2::One;

    Material();
    virtual ~Material();

    void PropagateMaterialChanged();
};

NAMESPACE_BANG_END

#endif // MATERIAL_H
