#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

#include "Bang/Renderer.h"
#include "Bang/ResourceHandle.h"

namespace Bang
{
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

    const Path &GetModelPath() const;
    Texture3D *GetVolumeTexture() const;

    // Renderer
    virtual void OnRender() override;
    virtual void SetUniformsOnBind(ShaderProgram *sp) override;

    // Serializable
    virtual void Reflect() override;

private:
    Path m_modelPath;
    RH<Mesh> p_cubeMesh;
    RH<Texture3D> p_volumeTexture;
    RH<ShaderProgram> p_cubeShaderProgram;

    Mesh *GetCubeMesh() const;
};
}

#endif  // VOLUMERENDERER_H
