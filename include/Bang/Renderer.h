#ifndef RENDERER_H
#define RENDERER_H

#include "Bang/GL.h"
#include "Bang/Component.h"
#include "Bang/RenderPass.h"
#include "Bang/EventEmitter.h"
#include "Bang/ResourceHandle.h"
#include "Bang/IEventsResource.h"
#include "Bang/IEventsRendererChanged.h"

NAMESPACE_BANG_BEGIN

FORWARD class Camera;
FORWARD class Material;
FORWARD class SceneManager;
FORWARD class ReflectionProbe;

class Renderer : public Component,
                 public EventListener<IEventsResource>,
                 public EventEmitter<IEventsRendererChanged>
{
    COMPONENT(Renderer)

public:
    virtual void Bind();
    virtual void OnRender();
    virtual void UnBind();

    // Component
    virtual void OnRender(RenderPass renderPass) override;

    void SetVisible(bool visible);
    void SetMaterial(Material *mat);
    void SetCastsShadows(bool castsShadows);
    void SetReceivesShadows(bool receivesShadows);
    void SetViewProjMode(GL::ViewProjMode viewProjMode);
    void SetRenderPrimitive(GL::Primitive renderPrimitive);
    void SetUseReflectionProbes(bool useReflectionProbes);

    bool IsVisible() const;
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
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

protected:
    Renderer();
    virtual ~Renderer();

    void PropagateRendererChanged();

    // Renderer
    virtual Matrix4 GetModelMatrixUniform() const;

private:
    bool m_visible = true;
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
