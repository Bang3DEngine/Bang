#include "Bang/ParticleSystem.h"

#include "Bang/VAO.h"
#include "Bang/VBO.h"
#include "Bang/Box.h"
#include "Bang/Scene.h"
#include "Bang/Random.h"
#include "Bang/Physics.h"
#include "Bang/Segment.h"
#include "Bang/Geometry.h"
#include "Bang/Triangle.h"
#include "Bang/MetaNode.h"
#include "Bang/Resources.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/GLUniforms.h"
#include "Bang/BoxCollider.h"
#include "Bang/MeshFactory.h"
#include "Bang/ShaderProgram.h"
#include "Bang/SphereCollider.h"
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

    m_lifeTime.SetRangeValues(0.1f, 3.0f);

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
    OnGameObjectChanged(nullptr, nullptr);
}

void ParticleSystem::OnUpdate()
{
    Component::OnUpdate();

    const Array<Collider*> &sceneColliders = m_sceneCollidersGatherer.
                                             GetGatheredObjects();
    const Vector3 &gravity = Physics::GetInstance()->GetGravity();
    float dt = SCAST<float>(Time::GetDeltaTime().GetSeconds());

    for (int i = 0; i < GetNumParticles(); ++i)
    {
        UpdateParticleData(i, dt, gravity, sceneColliders);
    }
    UpdateVBOData();
}

void ParticleSystem::Restart()
{
    const Vector3 &gravity = Physics::GetInstance()->GetGravity();
    for (uint i = 0; i < GetNumParticles(); ++i)
    {
        InitParticle(i, gravity);
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

void ParticleSystem::SetLifeTime(const ComplexRandom &lifeTime)
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

void ParticleSystem::SetGenerationShapeConeFOVRads(float coneFOVRads)
{
    if (coneFOVRads != GetGenerationShapeConeFOVRads())
    {
        m_generationShapeConeFOVRads = coneFOVRads;
    }
}

void ParticleSystem::SetSimulationSpace(ParticleSimulationSpace simulationSpace)
{
    if (simulationSpace != GetSimulationSpace())
    {
        m_simulationSpace = simulationSpace;
    }
}

void ParticleSystem::SetNumParticles(uint numParticles)
{
    if (numParticles != GetNumParticles())
    {
        uint prevNumParticles = GetNumParticles();
        m_numParticles = numParticles;

        const Vector3 &gravity = Physics::GetInstance()->GetGravity();

        // Resize arrays
        m_particlesPositions.Resize( GetNumParticles() );
        m_particlesData.Resize( GetNumParticles() );

        // Initialize values
        for (uint i = prevNumParticles; i < GetNumParticles(); ++i)
        {
            InitParticle(i, gravity);
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

void ParticleSystem::SetPhysicsStepMode(ParticlePhysicsStepMode stepMode)
{
    if (stepMode != GetPhysicsStepMode())
    {
        m_physicsStepMode = stepMode;
    }
}

ParticleGenerationShape ParticleSystem::GetGenerationShape() const
{
    return m_generationShape;
}

const ComplexRandom& ParticleSystem::GetLifeTime() const
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

float ParticleSystem::GetGenerationShapeConeFOVRads() const
{
    return m_generationShapeConeFOVRads;
}

ParticlePhysicsStepMode ParticleSystem::GetPhysicsStepMode() const
{
    return m_physicsStepMode;
}

ParticleSimulationSpace ParticleSystem::GetSimulationSpace() const
{
    return m_simulationSpace;
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

void ParticleSystem::OnGameObjectChanged(GameObject*, GameObject*)
{
    if (GetGameObject())
    {
        if (Scene *scene = GetGameObject()->GetScene())
        {
            m_sceneCollidersGatherer.SetRoot( SCAST<GameObject*>(scene) );
        }
    }
}

void ParticleSystem::OnRender()
{
    Renderer::OnRender();

    GL::RenderInstanced(p_particlesVAO,
                        GL::Primitive::TRIANGLES,
                        m_particleMesh.Get()->GetNumVertices(),
                        m_numParticles);
}

void ParticleSystem::SetUniformsOnBind(ShaderProgram *sp)
{
    Renderer::SetUniformsOnBind(sp);

    Matrix4 modelMat = Matrix4::Identity;
    switch (GetSimulationSpace())
    {
        case ParticleSimulationSpace::LOCAL:
            modelMat = GetGameObject()->GetTransform()->GetLocalToWorldMatrix();
        break;

        case ParticleSimulationSpace::WORLD:
        break;
    }
    Matrix4 normalMat = GLUniforms::CalculateNormalMatrix(modelMat);
    sp->SetMatrix4(GLUniforms::UniformName_Model,  modelMat);
    sp->SetMatrix4(GLUniforms::UniformName_Normal, normalMat);
}

void ParticleSystem::InitParticle(uint i, const Vector3 &gravity)
{
    ASSERT(i >= 0);
    ASSERT(i < GetNumParticles());

    ParticleData &particleData = m_particlesData[i];
    particleData.position = GetParticleInitialPosition();
    particleData.velocity = GetParticleInitialVelocity();
    particleData.remainingLifeTime = GetLifeTime().GenerateRandom();
    particleData.force = (GetGravityMultiplier() * gravity);

    m_particlesPositions[i] = particleData.position;
}

void ParticleSystem::UpdateParticleData(uint i,
                                        float dt,
                                        const Vector3 &gravity,
                                        const Array<Collider*> &sceneColliders)
{
    ParticleData &particleData = m_particlesData[i];
    particleData.remainingLifeTime -= dt;
    if (particleData.remainingLifeTime > 0.0f)
    {
        constexpr float pMass = 1.0f;

        const Vector3 pPrevPos  = particleData.position;
        Vector3 pPosition = particleData.position;
        Vector3 pVelocity = particleData.velocity;
        Vector3 pForce    = particleData.force;
        Vector3 pAcc      = (pForce / pMass);

        pPosition += pVelocity * dt;
        pVelocity += pAcc * dt;

        if (sceneColliders.Size() >= 1)
        {
            Vector3 pPositionNoInt = pPosition;
            Vector3 pVelocityNoInt = pVelocity;
            for (Collider *collider : sceneColliders)
            {
                CollideParticle(i, collider,
                                pPrevPos, pPositionNoInt, pVelocityNoInt,
                                &pPosition, &pVelocity);
            }
        }

        particleData.position  = pPosition;
        particleData.velocity  = pVelocity;
        m_particlesPositions[i] = pPosition;
    }
    else
    {
        InitParticle(i, gravity);
    }
}

void ParticleSystem::CollideParticle(uint i,
                                     Collider *collider,
                                     const Vector3 &prevPositionNoInt,
                                     const Vector3 &newPositionNoInt,
                                     const Vector3 &newVelocityNoInt,
                                     Vector3 *newPositionAfterInt_,
                                     Vector3 *newVelocityAfterInt_)
{
    Vector3 &newPositionAfterInt = *newPositionAfterInt_;
    Vector3 &newVelocityAfterInt = *newVelocityAfterInt_;

    Segment dispSegment(prevPositionNoInt, newPositionNoInt);

    bool collided = false;
    Vector3 collisionPoint;
    Vector3 collisionNormal;
    switch (collider->GetPhysicsObjectType())
    {
        case PhysicsObject::Type::SPHERE_COLLIDER:
        {
            SphereCollider *spCol = SCAST<SphereCollider*>(collider);
            Sphere sphere = spCol->GetSphereWorld();

            Ray dispRay(dispSegment.GetOrigin(), dispSegment.GetDirection());
            Geometry::IntersectRaySphere(dispRay,
                                         sphere,
                                         &collided,
                                         &collisionPoint);
            if (collided)
            {
                collided = Vector3::SqDistance(dispSegment.GetDestiny(),
                                               collisionPoint) <=
                                        dispSegment.GetSqLength();
                if (collided)
                {
                    Vector3 c = sphere.GetCenter();
                    collisionNormal = (collisionPoint - c).NormalizedSafe();
                }

            }
        }
        break;

        case PhysicsObject::Type::BOX_COLLIDER:
        {
            BoxCollider *boxCol = SCAST<BoxCollider*>(collider);
            Box box = boxCol->GetBoxWorld();

            Geometry::IntersectSegmentBox(dispSegment,
                                          box,
                                          &collided,
                                          &collisionPoint,
                                          &collisionNormal);
        }
        break;

        default:
        break;
    }

    if (collided)
    {
        Vector3 correctedDisp = (newPositionNoInt - collisionPoint);
        newVelocityAfterInt = Vector3::Reflect(newVelocityNoInt, collisionNormal);
        newPositionAfterInt = collisionPoint +
            (newVelocityAfterInt.NormalizedSafe() * correctedDisp.Length());
    }
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

        case ParticleGenerationShape::CONE:
            particleInitialPosition = Vector3::Zero;
        break;
    }

    Transform *tr = GetGameObject() ? GetGameObject()->GetTransform() : nullptr;
    Vector3 goPos = tr ? tr->GetPosition() : Vector3::Zero;
    particleInitialPosition += goPos;

    return particleInitialPosition;
}

Vector3 ParticleSystem::GetParticleInitialVelocity() const
{
    Vector3 particleInitialVelocity = Vector3::Zero;
    switch (GetGenerationShape())
    {
        case ParticleGenerationShape::BOX:
            particleInitialVelocity = Vector3::Forward;
        break;

        case ParticleGenerationShape::CONE:
        {
            float hFovR = GetGenerationShapeConeFOVRads() * 0.5f;
            particleInitialVelocity =
                Vector3(( Math::Tan( Random::GetRange(-hFovR, hFovR) ) ),
                        ( Math::Tan( Random::GetRange(-hFovR, hFovR) ) ),
                        -1.0f).
                    Normalized();
        }
        break;
    }

    Transform *tr = GetGameObject() ? GetGameObject()->GetTransform() : nullptr;
    Quaternion goRot = tr ? tr->GetRotation() : Quaternion::Identity;

    particleInitialVelocity *= GetInitialVelocityMultiplier();
    particleInitialVelocity = goRot * particleInitialVelocity;

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
    psClone->SetGenerationShapeConeFOVRads( GetGenerationShapeConeFOVRads() );
    psClone->SetSimulationSpace( GetSimulationSpace() );
    psClone->SetPhysicsStepMode( GetPhysicsStepMode() );
    psClone->SetInitialVelocityMultiplier( GetInitialVelocityMultiplier() );
    psClone->SetGravityMultiplier( GetGravityMultiplier() );
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
        SetLifeTime( metaNode.Get<ComplexRandom>("LifeTime") );
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

    if (metaNode.Contains("GenerationShapeConeFOV"))
    {
        SetGenerationShapeConeFOVRads(
                    metaNode.Get<float>("GenerationShapeConeFOV") );
    }

    if (metaNode.Contains("SimulationSpace"))
    {
        SetSimulationSpace(
                    metaNode.Get<ParticleSimulationSpace>("SimulationSpace") );
    }

    if (metaNode.Contains("PhysicsStepMode"))
    {
        SetPhysicsStepMode(
                    metaNode.Get<ParticlePhysicsStepMode>("PhysicsStepMode") );
    }

    if (metaNode.Contains("GravityMultiplier"))
    {
        SetGravityMultiplier( metaNode.Get<float>("GravityMultiplier") );
    }

    if (metaNode.Contains("InitialVelocityMultiplier"))
    {
        SetInitialVelocityMultiplier(
                    metaNode.Get<float>("InitialVelocityMultiplier") );
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
    metaNode->Set("GenerationShapeConeFOV", GetGenerationShapeConeFOVRads());
    metaNode->Set("SimulationSpace", GetSimulationSpace());
    metaNode->Set("PhysicsStepMode", GetPhysicsStepMode());
    metaNode->Set("GravityMultiplier", GetGravityMultiplier());
    metaNode->Set("InitialVelocityMultiplier", GetInitialVelocityMultiplier());
}


