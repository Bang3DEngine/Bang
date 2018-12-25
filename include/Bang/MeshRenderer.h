#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Bang/AABox.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/Renderer.h"
#include "Bang/String.h"

namespace Bang
{
class ICloneable;
class Mesh;
class Ray;
class ShaderProgram;
class Texture2D;

class MeshRenderer : public Renderer
{
    COMPONENT(MeshRenderer)

public:
    void SetMesh(Mesh *m);
    Mesh *GetMesh() const;
    Mesh *GetActiveMesh() const;
    Mesh *GetSharedMesh() const;

    void SetCurrentLOD(int lod);
    void SetAutoLOD(bool autoLOD);

    bool GetAutoLOD() const;
    int GetCurrentLOD() const;
    Mesh *GetCurrentLODActiveMesh() const;

    void IntersectRay(const Ray &ray,
                      bool *outIntersected = nullptr,
                      Vector3 *outIntersectionPoint = nullptr,
                      uint *outTriId = nullptr) const;
    void IntersectRay(const Ray &ray,
                      Texture2D *textureToFilterBy,
                      bool *outIntersected = nullptr,
                      Vector3 *outIntersectionPoint = nullptr,
                      uint *outTriId = nullptr) const;
    // Renderer
    virtual void Bind() override;
    virtual void SetUniformsOnBind(ShaderProgram *sp) override;
    virtual AABox GetAABBox() const override;

    // Serializable
    virtual void Reflect() override;

protected:
    mutable AH<Mesh> p_mesh;
    AH<Mesh> p_sharedMesh;

    bool m_autoLOD = false;
    int m_currentLOD = 0;

    void IntersectRay_(const Ray &ray,
                       Texture2D *textureToFilterBy = nullptr,
                       bool *outIntersected = nullptr,
                       Vector3 *outIntersectionPoint = nullptr,
                       uint *outTriId = nullptr) const;

    MeshRenderer();
    virtual ~MeshRenderer() override;

    // Renderer
    virtual void OnRender() override;
};
}

#endif  // MESHRENDERER_H
