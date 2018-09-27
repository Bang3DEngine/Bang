#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "Bang/Bang.h"
#include "Bang/MeshRenderer.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class VAO;
FORWARD class VBO;

enum class ParticleGenerationShape
{
    BOX
};

class ParticleSystem : public Renderer
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(ParticleSystem)

private:
    struct ParticleData
    {
        Vector3 position;
        Vector3 velocity;
        float remainingLifeTime;
        float   gravityMultiplier;
    };

public:
	ParticleSystem();
	virtual ~ParticleSystem();

    // Component
    virtual void OnStart() override;
    virtual void OnUpdate() override;

    void Restart();

    void SetMesh(Mesh *mesh);
    void SetLifeTime(float lifeTime);
    void SetNumParticles(uint numParticles);
    void SetGenerationShape(ParticleGenerationShape shape);
    void SetGenerationShapeBoxSize(const Vector3 &boxSize);
    void SetGravityMultiplier(float gravityMultiplier);
    void SetInitialVelocityMultiplier(float initialVelocityMultiplier);

    Mesh *GetMesh() const;
    uint GetNumParticles() const;
    const Vector3 &GetGenerationShapeBoxSize() const;
    ParticleGenerationShape GetGenerationShape() const;
    float GetLifeTime() const;
    float GetGravityMultiplier() const;
    float GetInitialVelocityMultiplier() const;

    // Renderer
    virtual void Bind() override;
    virtual void OnRender() override;
    AABox GetAABBox() const override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    VAO *p_particlesVAO = nullptr;
    VBO *p_particlePositionsVBO = nullptr;
    Array<Vector3> m_particlesPositions;
    Array<ParticleData> m_particlesData;

    RH<Mesh> m_particleMesh;
    uint m_numParticles = 0;

    ParticleGenerationShape m_generationShape = ParticleGenerationShape::BOX;
    Vector3 m_generationShapeBoxSize = Vector3::One;

    float m_lifeTime = 1.0f;
    float m_gravityMultiplier = 0.0f;
    float m_initialVelocityMultiplier = 1.0f;

    void InitParticle(uint i);
    Vector3 GetParticleInitialPosition() const;
    Vector3 GetParticleInitialVelocity() const;
    void UpdateVBOData();
};

NAMESPACE_BANG_END

#endif // PARTICLESYSTEM_H

