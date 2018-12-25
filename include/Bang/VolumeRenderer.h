#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

#include "Bang/AssetHandle.h"
#include "Bang/Renderer.h"

namespace Bang
{
class GBuffer;
class Framebuffer;
class Mesh;
class Material;
class Texture2D;
class Texture3D;
class ShaderProgram;
class VolumeRenderer : public Renderer
{
    COMPONENT_WITHOUT_CLASS_ID(VolumeRenderer)

protected:
    VolumeRenderer();
    virtual ~VolumeRenderer();

    void SetVolumeTexture(Texture3D *volTexture);
    void SetModelPath(const Path &pvmModelPath);
    void SetDensityThreshold(float densityThreshold);
    void SetNumSamples(uint numSamples);
    void SetRenderCubeMin(const Vector3 &renderCubeMin);
    void SetRenderCubeMax(const Vector3 &renderCubeMax);
    void SetUseTransferFunction(bool useTransferFunction);
    void SetSurfaceThickness(float surfaceThickness);
    void SetInvertNormals(bool invertNormals);
    void SetAlphaMultiply(float alphaMultiply);
    void SetTransferFunctionTexture(Texture2D *transferFunctionTexture);
    void SetVolumePropertiesMaterial(Material *volumePropertiesMaterial);

    const Path &GetModelPath() const;
    float GetDensityThreshold() const;
    uint GetNumSamples() const;
    float GetAlphaMultiply() const;
    Texture2D *GetTransferFunctionTexture() const;
    Texture3D *GetVolumeTexture() const;
    const Vector3 &GetRenderCubeMin() const;
    const Vector3 &GetRenderCubeMax() const;
    bool GetUseTransferFunction() const;
    float GetSurfaceThickness() const;
    bool GetInvertNormals() const;
    Material *GetVolumePropertiesMaterial() const;

    // Component
    void OnBeforeRender() override;

    // Renderer
    virtual void OnRender() override;

    // Serializable
    virtual void Reflect() override;

private:
    AH<Mesh> p_cubeMesh;
    AH<Texture2D> p_transferFunctionTexture;
    AH<Texture3D> p_volumeTexture;
    AH<ShaderProgram> p_forwardShaderProgram;
    AH<ShaderProgram> p_deferredShaderProgram;
    AH<Material> m_volumeRenderingMaterial;
    AH<Material> m_volumePropertiesMaterial;

    float m_alphaMultiply = 1.0f;
    bool m_useTransferFunction = false;
    Vector3 m_renderCubeMin = Vector3::Zero();
    Vector3 m_renderCubeMax = Vector3::One();
    Path m_modelPath = Path::Empty();
    uint m_numSamples = 50;
    float m_densityThreshold = 0.5f;
    float m_surfaceThickness = 0.01f;
    bool m_invertNormals = false;

    GBuffer *m_cubeBackFacesGBuffer = nullptr;

    Mesh *GetCubeMesh() const;
    Material *GetVolumeRenderMaterial() const;
};
}

#endif  // VOLUMERENDERER_H
