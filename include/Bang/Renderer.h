#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "Bang/AABox.h"
#include "Bang/Array.tcc"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/GL.h"
#include "Bang/IEventsAsset.h"
#include "Bang/IEventsRendererChanged.h"
#include "Bang/MetaNode.h"
#include "Bang/RenderPass.h"
#include "Bang/String.h"

namespace Bang
{
class Camera;
class ICloneable;
class Material;
class Asset;
class ShaderProgram;

class Renderer : public Component,
                 public EventListener<IEventsAsset>,
                 public EventEmitter<IEventsRendererChanged>
{
    COMPONENT(Renderer)

public:
    virtual void Bind();
    virtual void SetUniformsOnBind(ShaderProgram *sp);
    virtual void OnRender();
    virtual void UnBind();

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetVisible(bool visible);
    void SetMaterial(Material *sharedMaterial,
                     Material *materialCopy = nullptr);
    void SetDepthMask(bool depthMask);
    void SetCastsShadows(bool castsShadows);
    void SetReceivesShadows(bool receivesShadows);
    void SetViewProjMode(GL::ViewProjMode viewProjMode);
    void SetRenderPrimitive(GL::Primitive renderPrimitive);
    void SetUseReflectionProbes(bool useReflectionProbes);

    bool IsVisible() const;
    bool GetDepthMask() const;
    Material *GetSharedMaterial() const;
    Material *GetActiveMaterial() const;
    Material *GetCopiedMaterial() const;
    Material *GetMaterial() const;
    bool GetCastsShadows() const;
    bool GetReceivesShadows() const;
    GL::ViewProjMode GetViewProjMode() const;
    GL::Primitive GetRenderPrimitive() const;
    bool GetUseReflectionProbes() const;

    // IEventsAssetChanged
    void OnAssetChanged(Asset *changedAsset) override;

    // Renderer
    virtual AABox GetAABBox() const;
    virtual AARect GetBoundingRect(Camera *camera) const;

    // Serializable
    virtual void Reflect() override;

protected:
    Renderer();
    virtual ~Renderer() override;

    void PropagateRendererChanged();

    // Renderer
    virtual Matrix4 GetModelMatrixUniform() const;

private:
    bool m_visible = true;
    bool m_depthMask = true;
    bool m_castsShadows = true;
    bool m_receivesShadows = true;
    bool m_useReflectionProbes = false;
    GL::Primitive m_renderPrimitive = GL::Primitive::TRIANGLES;
    GL::ViewProjMode m_viewProjMode = GL::ViewProjMode::WORLD;

    mutable AH<Material> p_copiedMaterial;
    AH<Material> p_sharedMaterial;

    friend class GEngine;
};
}  // namespace Bang

#endif  // RENDERER_H
