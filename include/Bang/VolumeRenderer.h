#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

#include "Bang/Renderer.h"
#include "Bang/ResourceHandle.h"

namespace Bang
{
class Mesh;
class Material;
class ShaderProgram;
class VolumeRenderer : public Renderer
{
    COMPONENT(VolumeRenderer)

protected:
    VolumeRenderer();
    virtual ~VolumeRenderer();

    void SetModelPath(const Path &pvmModelPath);

    const Path &GetModelPath() const;

    // Renderer
    virtual void OnRender() override;

    // Serializable
    virtual void Reflect() override;

private:
    Path m_modelPath;
    RH<Mesh> p_cubeMesh;
    RH<ShaderProgram> p_cubeShaderProgram;

    Mesh *GetCubeMesh() const;
};
}

#endif  // VOLUMERENDERER_H
