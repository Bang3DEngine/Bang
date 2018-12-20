#ifndef DECALRENDERER_H
#define DECALRENDERER_H

#include "Bang/Bang.h"
#include "Bang/Renderer.h"

namespace Bang
{
class Texture2D;
class DecalRenderer : public Renderer
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(DecalRenderer);

public:
    DecalRenderer();
    virtual ~DecalRenderer() override;

    // Renderer
    void OnRender() override;

    void SetIsProjective(bool isProjective);
    void SetFieldOfView(float fieldOfView);
    void SetZNear(float zNear);
    void SetZFar(float zFar);
    void SetDecalTexture(Texture2D *decalTexture);

    float GetZNear() const;
    float GetZFar() const;
    float GetFieldOfView() const;
    Vector3 GetBoxSize() const;
    float GetAspectRatio() const;
    bool GetIsProjective() const;
    Texture2D *GetDecalTexture() const;
    Matrix4 GetProjectionMatrix() const;

    // IReflectable
    virtual void Reflect() override;

private:
    RH<Mesh> m_cubeMesh;
    bool m_projective = false;

    float m_zNear = 0.05f;
    float m_zFar = 100.0f;
    float m_fieldOfView = 60.0f;
};
}

#endif  // DECALRENDERER_H
