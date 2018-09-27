#include "Bang/ParticleSystem.h"

#include "Bang/VAO.h"
#include "Bang/VBO.h"
#include "Bang/Random.h"
#include "Bang/Physics.h"
#include "Bang/MetaNode.h"
#include "Bang/Resources.h"
#include "Bang/MeshFactory.h"
#include "Bang/MaterialFactory.h"
#include "Bang/ReflectionProbe.h"

USING_NAMESPACE_BANG

ParticleSystem::ParticleSystem()
{
    CONSTRUCT_CLASS_ID(ParticleSystem);
    SetRenderPrimitive( GL::Primitive::TRIANGLES );
    SetMaterial( MaterialFactory::GetDefaultParticles().Get() );

    p_particlePositionsVBO = new VBO();
    SetNumParticles(100);

    SetMesh( MeshFactory::GetCube().Get() );
}

ParticleSystem::~ParticleSystem()
{
    if (p_particlesVAO)
    {
        delete p_particlesVAO;
    }
}

void ParticleSystem::OnStart()
{
    Component::OnStart();

    Restart();
}

void ParticleSystem::OnUpdate()
{
    Component::OnUpdate();

    const Vector3 &gravity = Physics::GetInstance()->GetGravity();
    float dt = SCAST<float>(Time::GetDeltaTime().GetSeconds());

    for (int i = 0; i < GetNumParticles(); ++i)
    {
        ParticleData &particleData = m_particlesData[i];
        particleData.remainingLifeTime -= dt;
        if (particleData.remainingLifeTime > 0.0f)
        {
            Vector3 pPosition = particleData.position;
            Vector3 pVelocity = particleData.velocity;
            float pGravityMultiplier = particleData.gravityMultiplier;

            pVelocity += pGravityMultiplier * gravity * dt;
            pPosition += pVelocity;

            particleData.position  = pPosition;
            particleData.velocity  = pVelocity;
            m_particlesPositions[i] = pPosition;
        }
        else
        {
            InitParticle(i);
        }
    }
    UpdateVBOData();
}

void ParticleSystem::Restart()
{
    for (uint i = 0; i < 10; ++i)
    {
        InitParticle(i);
    }
    UpdateVBOData();
}

void ParticleSystem::SetMesh(Mesh *mesh)
{
    if (m_particleMesh.Get() != mesh)
    {
        m_particleMesh.Set(mesh);

        if (p_particlesVAO)
        {
            delete p_particlesVAO;
            p_particlesVAO = nullptr;
        }

        if (GetMesh())
        {
            const VAO *meshVAO = GetMesh()->GetVAO();
            const VBO *particleMeshPositionsVBO =
                    meshVAO->GetVBOByLocation(Mesh::DefaultPositionsVBOLocation);
            const VBO *particleMeshNormalsVBO   =
                    meshVAO->GetVBOByLocation(Mesh::DefaultNormalsVBOLocation);
            const VBO *particleMeshUvsVBO       =
                    meshVAO->GetVBOByLocation(Mesh::DefaultUvsVBOLocation);

            p_particlesVAO = new VAO();
            {
                uint meshVBOStride = GetMesh()->GetVBOStride();

                int particlesPosBytesSize = 3 * sizeof(float);
                uint particlesPosVBOStride = particlesPosBytesSize;

                p_particlesVAO->SetVBO(particleMeshPositionsVBO,
                                       Mesh::DefaultPositionsVBOLocation,
                                       3, GL::VertexAttribDataType::FLOAT,
                                       false,
                                       meshVBOStride,
                                       GetMesh()->GetVBOPositionsOffset());
                p_particlesVAO->SetVBO(particleMeshNormalsVBO,
                                       Mesh::DefaultNormalsVBOLocation,
                                       3, GL::VertexAttribDataType::FLOAT,
                                       true,
                                       meshVBOStride,
                                       GetMesh()->GetVBONormalsOffset());
                p_particlesVAO->SetVBO(particleMeshUvsVBO,
                                       Mesh::DefaultUvsVBOLocation,
                                       2, GL::VertexAttribDataType::FLOAT,
                                       false,
                                       meshVBOStride,
                                       GetMesh()->GetVBOUvsOffset());
                p_particlesVAO->SetVBO(p_particlePositionsVBO, 3,
                                       3, GL::VertexAttribDataType::FLOAT,
                                       false,
                                       particlesPosVBOStride,
                                       0);
                p_particlesVAO->SetVertexAttribDivisor(3, 1);
                p_particlesVAO->SetIBO(meshVAO->GetIBO());
            }
        }
    }
}

void ParticleSystem::SetLifeTime(float lifeTime)
{
    if (lifeTime != GetLifeTime())
    {
        m_lifeTime = lifeTime;
    }
}

void ParticleSystem::SetGravityMultiplier(float gravityMultiplier)
{
    if (gravityMultiplier != GetGravityMultiplier())
    {
        m_gravityMultiplier = gravityMultiplier;
    }
}

void ParticleSystem::SetInitialVelocityMultiplier(float initialVelocityMultiplier)
{
    if (initialVelocityMultiplier != GetInitialVelocityMultiplier())
    {
        m_initialVelocityMultiplier = initialVelocityMultiplier;
    }
}

void ParticleSystem::SetNumParticles(uint numParticles)
{
    if (numParticles != GetNumParticles())
    {
        uint prevNumParticles = GetNumParticles();
        m_numParticles = numParticles;

        // Resize arrays
        m_particlesPositions.Resize( GetNumParticles() );
        m_particlesData.Resize( GetNumParticles() );

        // Initialize values
        for (uint i = prevNumParticles; i < GetNumParticles(); ++i)
        {
            InitParticle(i);
        }

        // Initialize VBOs
        p_particlePositionsVBO->CreateAndFill(
                                 m_particlesPositions.Data(),
                                 m_particlesPositions.Size() * sizeof(Vector3),
                                 GL::UsageHint::DYNAMIC_DRAW);
    }
}

void ParticleSystem::SetGenerationShape(ParticleGenerationShape shape)
{
    if (shape != GetGenerationShape())
    {
        m_generationShape = shape;
    }
}

void ParticleSystem::SetGenerationShapeBoxSize(const Vector3 &boxSize)
{
    if (boxSize != GetGenerationShapeBoxSize())
    {
        m_generationShapeBoxSize = boxSize;
    }
}

ParticleGenerationShape ParticleSystem::GetGenerationShape() const
{
    return m_generationShape;
}

float ParticleSystem::GetLifeTime() const
{
    return m_lifeTime;
}

const Vector3 &ParticleSystem::GetGenerationShapeBoxSize() const
{
    return m_generationShapeBoxSize;
}

uint ParticleSystem::GetNumParticles() const
{
    return m_numParticles;
}

float ParticleSystem::GetGravityMultiplier() const
{
    return m_gravityMultiplier;
}

float ParticleSystem::GetInitialVelocityMultiplier() const
{
    return m_initialVelocityMultiplier;
}

Mesh *ParticleSystem::GetMesh() const
{
    return m_particleMesh.Get();
}

void ParticleSystem::Bind()
{
    Renderer::Bind();
}

AABox ParticleSystem::GetAABBox() const
{
    return AABox::Empty;
}

void ParticleSystem::OnRender()
{
    Renderer::OnRender();

    GL::RenderInstanced(p_particlesVAO,
                        GL::Primitive::TRIANGLES,
                        m_particleMesh.Get()->GetNumVertices(),
                        m_numParticles);
}

void ParticleSystem::InitParticle(uint i)
{
    ASSERT(i >= 0);
    ASSERT(i < GetNumParticles());

    ParticleData &particleData = m_particlesData[i];
    particleData.position = GetParticleInitialPosition();
    particleData.velocity = GetParticleInitialVelocity();
    particleData.gravityMultiplier = GetGravityMultiplier();
    particleData.remainingLifeTime = GetLifeTime();

    m_particlesPositions[i] = particleData.position;
}

Vector3 ParticleSystem::GetParticleInitialPosition() const
{
    Vector3 particleInitialPosition = Vector3::Zero;
    switch (GetGenerationShape())
    {
        case ParticleGenerationShape::BOX:
            particleInitialPosition = Random::GetRandomVector3() *
                                      (GetGenerationShapeBoxSize() * 0.5f);
        break;
    }
    return particleInitialPosition;
}

Vector3 ParticleSystem::GetParticleInitialVelocity() const
{
    Vector3 particleInitialVelocity = Vector3::Zero;
    switch (GetGenerationShape())
    {
        case ParticleGenerationShape::BOX:
            particleInitialVelocity = Vector3::Up * GetInitialVelocityMultiplier();
        break;
    }
    return particleInitialVelocity;
}

void ParticleSystem::UpdateVBOData()
{
    p_particlePositionsVBO->Update(m_particlesPositions.Data(),
                                   m_particlesPositions.Size() * sizeof(Vector3),
                                   0);
}

void ParticleSystem::CloneInto(ICloneable *clone) const
{
    Renderer::CloneInto(clone);

    ParticleSystem *psClone = SCAST<ParticleSystem*>(clone);
    psClone->SetMesh( GetMesh() );
    psClone->SetLifeTime( GetLifeTime() );
    psClone->SetNumParticles( GetNumParticles() );
    psClone->SetGenerationShape( GetGenerationShape() );
    psClone->SetGenerationShapeBoxSize( GetGenerationShapeBoxSize() );
    psClone->SetGravityMultiplier( GetGravityMultiplier() );
    psClone->SetInitialVelocityMultiplier( GetInitialVelocityMultiplier() );
}

void ParticleSystem::ImportMeta(const MetaNode &metaNode)
{
    Renderer::ImportMeta(metaNode);

    if (metaNode.Contains("Mesh"))
    {
        GUID meshGUID = metaNode.Get<GUID>("Mesh");
        RH<Mesh> mesh = Resources::Load<Mesh>(meshGUID);
        SetMesh( mesh.Get() );
    }

    if (metaNode.Contains("LifeTime"))
    {
        SetLifeTime( metaNode.Get<float>("LifeTime") );
    }

    if (metaNode.Contains("NumParticles"))
    {
        SetNumParticles( metaNode.Get<uint>("NumParticles") );
    }

    if (metaNode.Contains("GenerationShape"))
    {
        SetGenerationShape(
                    metaNode.Get<ParticleGenerationShape>("GenerationShape") );
    }

    if (metaNode.Contains("GenerationShapeBoxSize"))
    {
        SetGenerationShapeBoxSize(
                    metaNode.Get<Vector3>("GenerationShapeBoxSize") );
    }
}

void ParticleSystem::ExportMeta(MetaNode *metaNode) const
{
    Renderer::ExportMeta(metaNode);

    metaNode->Set("Mesh", GetMesh() ? GetMesh()->GetGUID() : GUID::Empty());
    metaNode->Set("LifeTime", GetLifeTime());
    metaNode->Set("NumParticles", GetNumParticles());
    metaNode->Set("GenerationShape", GetGenerationShape());
    metaNode->Set("GenerationShapeBoxSize", GetGenerationShapeBoxSize());
}


