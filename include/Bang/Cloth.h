#ifndef CLOTH_H
#define CLOTH_H

#include "Bang/AssetHandle.h"
#include "Bang/Bang.h"
#include "Bang/Particle.h"
#include "Bang/Renderer.h"

namespace Bang
{
class Mesh;
class Cloth : public Renderer
{
    COMPONENT_WITHOUT_CLASS_ID(Cloth)

public:
    Cloth();
    virtual ~Cloth() override;

    void Reset();

    void SetClothSize(float clothSize);
    void SetBounciness(float bounciness);
    void SetFriction(float friction);
    void SetDamping(float damping);
    void SetPoint(uint i, const Vector3 &pos);
    void SetSpringsDamping(float springsDamping);
    void SetSubdivisions(uint subdivisions);
    void SetSpringsForce(float springsForce);
    void SetFixedPoint(uint i, bool fixed);
    void SetSeeDebugPoints(bool seeDebugPoints);
    void SetComputeCollisions(bool computeCollisions);

    Mesh *GetMesh() const;
    float GetDamping() const;
    float GetFriction() const;
    float GetBounciness() const;
    float GetSpringsDamping() const;
    float GetClothSize() const;
    uint GetSubdivisions() const;
    bool GetSeeDebugPoints() const;
    float GetSpringsForce() const;
    bool IsPointFixed(uint i) const;
    bool GetComputeCollisions() const;
    const Array<Vector3> &GetPoints() const;
    const Array<bool> &GetFixedPoints() const;

    // Component
    void OnStart() override;
    void OnUpdate() override;

    // Renderer
    void Bind() override;
    void OnRender() override;
    void OnRender(RenderPass rp) override;
    void SetUniformsOnBind(ShaderProgram *sp) override;
    AABox GetAABBox() const override;

    // ICloneable
    void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    void Reflect() override;

private:
    AH<Mesh> m_mesh;
    bool m_validMeshPoints = false;
    Array<Vector3> m_points;
    Array<Particle::Data> m_particlesData;
    Particle::Parameters m_particleParams;
    Array<Vector2i> m_offsets;
    Array<bool> m_fixedPoints;
    Array<float> m_offsetsLengths;

    uint m_subdivisions = 0;
    float m_clothSize = 1.0f;
    float m_springsForce = 50.0f;
    float m_springsDamping = 1.0f;

    AH<Mesh> m_debugPointsMesh;
    AH<Material> m_debugPointsMaterial;
    bool m_seeDebugPoints = false;

    void InitParticle(uint i, const Particle::Parameters &params);
    void AddSpringForces();
    void UpdateMeshPoints();
    void ConstrainJoints();
    void RecreateMesh();

    uint GetTotalNumPoints() const;
    float GetSubdivisionLength() const;
    const Particle::Parameters &GetParameters() const;
};
}

#endif  // CLOTH_H
