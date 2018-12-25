#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/ComplexRandom.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Math.h"
#include "Bang/MetaNode.h"
#include "Bang/Particle.h"
#include "Bang/Renderer.h"
#include "Bang/String.h"
#include "Bang/Vector3.h"

namespace Bang
{
class ICloneable;
class Mesh;
class ShaderProgram;
class Texture2D;
class VAO;
class VBO;

enum class ParticleGenerationShape
{
    BOX,
    CONE
};

enum class ParticleRenderMode
{
    ADDITIVE,
    MESH
};

enum class ParticleSimulationSpace
{
    LOCAL,
    WORLD
};

class ParticleSystem : public Renderer
{
    COMPONENT(ParticleSystem)

private:
    struct ParticleVBOData
    {
        Vector3 position;
        float size;
        Color color;
        float animationFrame;
    };

public:
    ParticleSystem();
    virtual ~ParticleSystem() override;

    // Component
    virtual void OnStart() override;
    virtual void OnUpdate() override;

    void Reset();

    void SetMesh(Mesh *mesh);
    void SetTexture(Texture2D *texture);
    void SetAnimationSheetSize(const Vector2i &animationSheetSize);
    void SetAnimationSpeed(float animationSpeed);
    void SetLifeTime(const ComplexRandom &lifeTime);
    void SetStartTime(const ComplexRandom &startTime);
    void SetStartSize(const ComplexRandom &startSize);
    void SetBillboard(bool billboard);
    void SetBounciness(float bounciness);
    void SetNumParticles(uint numParticles);
    void SetGenerationShape(ParticleGenerationShape shape);
    void SetGenerationShapeBoxSize(const Vector3 &boxSize);
    void SetPhysicsStepMode(Particle::PhysicsStepMode stepMode);
    void SetGravityMultiplier(float gravityMultiplier);
    void SetInitialVelocityMultiplier(float initialVelocityMultiplier);
    void SetGenerationShapeConeFOVRads(float coneFOVRads);
    void SetStartColor(const Color &startColor);
    void SetEndColor(const Color &endColor);
    void SetComputeCollisions(bool computeCollisions);
    void SetParticleRenderMode(ParticleRenderMode particleRenderMode);
    void SetSimulationSpace(ParticleSimulationSpace simulationSpace);

    Mesh *GetMesh() const;
    bool GetBillboard() const;
    uint GetNumParticles() const;
    const Vector3 &GetGenerationShapeBoxSize() const;
    ParticleGenerationShape GetGenerationShape() const;
    const ComplexRandom &GetLifeTime() const;
    const ComplexRandom &GetStartTime() const;
    const ComplexRandom &GetStartSize() const;
    const Color &GetStartColor() const;
    const Color &GetEndColor() const;
    Texture2D *GetTexture() const;
    const Vector2i &GetAnimationSheetSize() const;
    float GetBounciness() const;
    float GetAnimationSpeed() const;
    bool GetComputeCollisions() const;
    float GetGravityMultiplier() const;
    float GetInitialVelocityMultiplier() const;
    float GetGenerationShapeConeFOVRads() const;
    ParticleRenderMode GetParticleRenderMode() const;
    const Particle::Parameters &GetParticlesParameters() const;
    Particle::PhysicsStepMode GetPhysicsStepMode() const;
    ParticleSimulationSpace GetSimulationSpace() const;

    // Renderer
    virtual void Bind() override;
    virtual void OnRender() override;
    virtual void SetUniformsOnBind(ShaderProgram *sp) override;
    AABox GetAABBox() const override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    AABox m_aabox;
    bool m_emitOnStart = true;
    bool m_isEmitting = false;

    VAO *p_particlesVAO = nullptr;
    VBO *p_particleDataVBO = nullptr;
    Array<Particle::Data> m_particlesData;
    Array<ParticleVBOData> m_particlesVBOData;

    AH<Mesh> m_particleMesh;
    uint m_numParticles = 0;

    Particle::Parameters m_particlesParameters;
    ParticleSimulationSpace m_simulationSpace = ParticleSimulationSpace::WORLD;

    ParticleGenerationShape m_generationShape = ParticleGenerationShape::BOX;
    Vector3 m_generationShapeBoxSize = Vector3::One();
    float m_generationShapeConeFOVRads = SCAST<float>(Math::Pi / 4);

    ParticleRenderMode m_particleRenderMode = Undef<ParticleRenderMode>();

    AH<Texture2D> p_texture;
    bool m_billboard = true;

    Color m_startColor = Color::White();
    Color m_endColor = Color::White();
    ComplexRandom m_lifeTime = ComplexRandom(0.1f, 3.0f);
    ComplexRandom m_startTime = ComplexRandom(0.1f, 5.0f);
    ComplexRandom m_startSize = ComplexRandom(0.3f, 1.0f);

    float m_initialVelocityMultiplier = 1.0f;

    uint m_stepsPerSecond = 60;

    void InitParticle(uint i, const Particle::Parameters &params);
    bool IsParticleActive(uint i) const;
    void RecreateVAOForMesh();
    void UpdateDataVBO();

    Vector3 GetParticleInitialPosition() const;
    Vector3 GetParticleInitialVelocity() const;
};
}  // namespace Bang

#endif  // PARTICLESYSTEM_H
