#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Bang/AABox.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/Renderer.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class ICloneable;
FORWARD class Mesh;
FORWARD class ShaderProgram;
FORWARD class VAO;

class MeshRenderer : public Renderer
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(MeshRenderer)

public:
    void SetMesh(Mesh* m);
    Mesh* GetMesh() const;
    Mesh* GetActiveMesh() const;
    Mesh* GetSharedMesh() const;

    void SetCurrentLOD(int lod);
    void SetAutoLOD(bool autoLOD);

    bool GetAutoLOD() const;
    int GetCurrentLOD() const;
    Mesh *GetCurrentLODActiveMesh() const;

    // Renderer
    virtual void Bind() override;
    virtual void SetUniformsOnBind(ShaderProgram *sp) override;
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
