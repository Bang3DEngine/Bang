#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

#include "Bang/Renderer.h"
#include "Bang/ResourceHandle.h"

namespace Bang
{
class GBuffer;
class Framebuffer;
class Mesh;
class Material;
class Texture3D;
class ShaderProgram;
class VolumeRenderer : public Renderer
{
    COMPONENT(VolumeRenderer)

protected:
    VolumeRenderer();
    virtual ~VolumeRenderer();

    void SetVolumeTexture(Texture3D *volTexture);
    void SetModelPath(const Path &pvmModelPath);
    void SetDensityThreshold(float densityThreshold);
    void SetNumSamples(uint numSamples);

    const Path &GetModelPath() const;
    float GetDensityThreshold() const;
    uint GetNumSamples() const;
    Texture3D *GetVolumeTexture() const;

    // Renderer
    virtual void OnRender() override;

    // Serializable
    virtual void Reflect() override;

private:
    RH<Mesh> p_cubeMesh;
    RH<Texture3D> p_volumeTexture;
    RH<ShaderProgram> p_cubeShaderProgram;
    RH<Material> m_volumeRenderingMaterial;

    Path m_modelPath = Path::Empty();
    uint m_numSamples = 50;
    float m_densityThreshold = 0.5f;

    GBuffer *m_cubeBackFacesGBuffer = nullptr;

    Mesh *GetCubeMesh() const;
    Material *GetVolumeRenderMaterial() const;
};
}

#endif  // VOLUMERENDERER_H
