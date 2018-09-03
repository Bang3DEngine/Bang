#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Bang/Renderer.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class VAO;

class MeshRenderer : public Renderer
{
    COMPONENT(MeshRenderer)

public:
    void SetMesh(Mesh* m);
    Mesh* GetMesh() const;
    Mesh* GetActiveMesh() const;
    Mesh* GetSharedMesh() const;

    virtual void Bind() override;

    void SetCurrentLOD(int lod);
    void SetAutoLOD(bool autoLOD);

    bool GetAutoLOD() const;
    int GetCurrentLOD() const;
    Mesh *GetCurrentLODActiveMesh() const;

    // Renderer
    virtual AABox GetAABBox() const override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    mutable RH<Mesh> p_mesh;
    RH<Mesh> p_sharedMesh;

    bool m_autoLOD = false;
    int m_currentLOD = 0;

    MeshRenderer();
    virtual ~MeshRenderer();

    // Renderer
    virtual void OnRender() override;

};

NAMESPACE_BANG_END

#endif // MESHRENDERER_H
