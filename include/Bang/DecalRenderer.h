#ifndef DECALRENDERER_H
#define DECALRENDERER_H

#include "Bang/Bang.h"
#include "Bang/Renderer.h"

namespace Bang
{
class Texture2D;
class DecalRenderer : public Renderer
{
    COMPONENT(DecalRenderer);

public:
    DecalRenderer();
    virtual ~DecalRenderer() override;

    // Renderer
    virtual void OnRender() override;
    virtual Matrix4 GetModelMatrixUniform() const override;

    void SetIsPerspective(bool isPerspective);
    void SetFieldOfViewDegrees(float fieldOfViewDegrees);
    void SetAspectRatio(float aspectRatio);
    void SetZNear(float zNear);
    void SetZFar(float zFar);
    void SetBoxSize(const Vector3 &boxSize);
    void SetDecalTexture(Texture2D *decalTexture);

    float GetZNear() const;
    float GetZFar() const;
    float GetFieldOfViewDegrees() const;
    const Vector3 &GetBoxSize() const;
    float GetAspectRatio() const;
    bool GetIsPerspective() const;
    Texture2D *GetDecalTexture() const;
    Matrix4 GetViewMatrix() const;
    Matrix4 GetProjectionMatrix() const;

    // IReflectable
    virtual void Reflect() override;

private:
    AH<Mesh> m_cubeMesh;
    bool m_perspective = false;

    float m_zNear = 0.05f;
    float m_zFar = 100.0f;
    float m_fieldOfViewDegrees = 60.0f;
    float m_aspectRatio = 1.0f;
    Vector3 m_boxSize = Vector3::One();
};
}

#endif  // DECALRENDERER_H
