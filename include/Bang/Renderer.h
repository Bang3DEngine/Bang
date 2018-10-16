#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "Bang/AABox.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/GL.h"
#include "Bang/IEvents.h"
#include "Bang/IEventsRendererChanged.h"
#include "Bang/IEventsResource.h"
#include "Bang/MetaNode.h"
#include "Bang/RenderPass.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class Camera;
FORWARD class ICloneable;
FORWARD class IEventsRendererChanged;
FORWARD class IEventsResource;
FORWARD class Resource;
FORWARD class ShaderProgram;
FORWARD class Material;
FORWARD class ReflectionProbe;
FORWARD class SceneManager;

class Renderer : public Component,
                 public EventListener<IEventsResource>,
                 public EventEmitter<IEventsRendererChanged>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(Renderer)

public:
    virtual void Bind();
    virtual void SetUniformsOnBind(ShaderProgram *sp);
    virtual void OnRender();
    virtual void UnBind();

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetVisible(bool visible);
    void SetMaterial(Material *mat);
    void SetDepthMask(bool depthMask);
    void SetCastsShadows(bool castsShadows);
    void SetReceivesShadows(bool receivesShadows);
    void SetViewProjMode(GL::ViewProjMode viewProjMode);
    void SetRenderPrimitive(GL::Primitive renderPrimitive);
    void SetUseReflectionProbes(bool useReflectionProbes);

    bool IsVisible() const;
    bool GetDepthMask() const;
    Material* GetSharedMaterial() const;
    Material* GetActiveMaterial() const;
    Material* GetMaterial() const;
    bool GetCastsShadows() const;
    bool GetReceivesShadows() const;
    GL::ViewProjMode GetViewProjMode() const;
    GL::Primitive GetRenderPrimitive() const;
    bool GetUseReflectionProbes() const;

    // IEventsResourceChanged
    void OnResourceChanged(Resource *changedResource) override;

    // Renderer
    virtual AABox GetAABBox() const;
    virtual AARect GetBoundingRect(Camera *camera) const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

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

    mutable RH<Material> p_material;
    RH<Material> p_sharedMaterial;


    friend class GEngine;
};

NAMESPACE_BANG_END

#endif // RENDERER_H
