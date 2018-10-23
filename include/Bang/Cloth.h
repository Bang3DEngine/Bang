#ifndef CLOTH_H
#define CLOTH_H

#include "Bang/Bang.h"
#include "Bang/Particle.h"
#include "Bang/Renderer.h"
#include "Bang/ResourceHandle.h"

namespace Bang
{
class Mesh;
class Cloth : public Renderer
{
    COMPONENT(Cloth)

public:
    Cloth();
    virtual ~Cloth() override;

    void Reset();

    void SetClothSize(float clothSize);
    void SetBounciness(float bounciness);
    void SetFriction(float friction);
    void SetDamping(float damping);
    void SetSpringsDamping(float springsDamping);
    void SetSubdivisions(uint subdivisions);
    void SetSpringsForce(float springsForce);
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
    bool GetComputeCollisions() const;

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
    void CloneInto(ICloneable *clone) const override;

    // Serializable
    void Reflect() override;

private:
    RH<Mesh> m_mesh;
    bool m_validMeshPoints = false;
    Array<Vector3> m_points;
    Array<Particle::Data> m_particlesData;
    Particle::Parameters m_particleParams;
    Array<Vector2i> m_offsets;
    Array<float> m_offsetsLengths;

    uint m_subdivisions = 0;
    float m_clothSize = 1.0f;
    float m_springsForce = 0.0f;
    float m_springsDamping = 1.0f;

    RH<Mesh> m_debugPointsMesh;
    RH<Material> m_debugPointsMaterial;
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
