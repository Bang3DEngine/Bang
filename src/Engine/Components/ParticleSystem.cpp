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

    p_particleDataVBO = new VBO();
    SetNumParticles(100);

    SetMesh( MeshFactory::GetPlane().Get() );
    SetParticleRenderMode( ParticleRenderMode::ADDITIVE );
}

ParticleSystem::~ParticleSystem()
{
    if (p_particlesVAO)
    {
        delete p_particlesVAO;
    }

    if (p_particleDataVBO)
    {
        delete p_particleDataVBO;
    }
}

void ParticleSystem::OnStart()
{
    Component::OnStart();

    Reset();
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
    UpdateDataVBO();
}

void ParticleSystem::Reset()
{
    const Vector3 &gravity = Physics::GetInstance()->GetGravity();
    for (uint i = 0; i < GetNumParticles(); ++i)
    {
        InitParticle(i, gravity);
    }
    UpdateDataVBO();
}

void ParticleSystem::SetMesh(Mesh *mesh)
{
    if (m_particleMesh.Get() != mesh)
    {
        m_particleMesh.Set(mesh);
        RecreateVAOForMesh();
    }
}

void ParticleSystem::SetLifeTime(const ComplexRandom &lifeTime)
{
    if (lifeTime != GetLifeTime())
    {
        m_lifeTime = lifeTime;
    }
}

void ParticleSystem::SetStartTime(const ComplexRandom &startTime)
{
    if (startTime != GetStartTime())
    {
        m_startTime = startTime;
    }
}

void ParticleSystem::SetStartSize(const ComplexRandom &startSize)
{
    if (startSize != GetStartSize())
    {
        m_startSize = startSize;
    }
}

void ParticleSystem::SetBillboard(bool billboard)
{
    if (billboard != GetBillboard())
    {
        m_billboard = billboard;
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

void ParticleSystem::SetStartColor(const Color &startColor)
{
    if (startColor != GetStartColor())
    {
        m_startColor = startColor;
    }
}

void ParticleSystem::SetEndColor(const Color &endColor)
{
    if (endColor != GetEndColor())
    {
        m_endColor = endColor;
    }
}

void ParticleSystem::SetParticleRenderMode(ParticleRenderMode particleRenderMode)
{
    if (particleRenderMode != GetParticleRenderMode())
    {
        m_particleRenderMode = particleRenderMode;

        Material *newMaterial = nullptr;
        switch (GetParticleRenderMode())
        {
            case ParticleRenderMode::ADDITIVE:
                newMaterial = MaterialFactory::GetParticlesAdditive().Get();
            break;

            case ParticleRenderMode::MESH:
                newMaterial = MaterialFactory::GetParticlesMesh().Get();
            break;
        }

        SetMaterial(newMaterial);
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
        m_particlesVBOData.Resize( GetNumParticles() );
        m_particlesData.Resize( GetNumParticles() );

        // Initialize values
        for (uint i = prevNumParticles; i < GetNumParticles(); ++i)
        {
            InitParticle(i, gravity);
        }

        // Initialize VBOs
        p_particleDataVBO->CreateAndFill(
                         m_particlesVBOData.Data(),
                         m_particlesVBOData.Size() * sizeof(ParticleVBOData),
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

const ComplexRandom &ParticleSystem::GetStartTime() const
{
    return m_startTime;
}

const ComplexRandom &ParticleSystem::GetStartSize() const
{
    return m_startSize;
}

const Color &ParticleSystem::GetStartColor() const
{
    return m_startColor;
}

const Color &ParticleSystem::GetEndColor() const
{
    return m_endColor;
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

ParticleRenderMode ParticleSystem::GetParticleRenderMode() const
{
    return m_particleRenderMode;
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

bool ParticleSystem::GetBillboard() const
{
    return m_billboard;
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

    Transform *tr = GetGameObject()->GetTransform();
    Matrix4 modelMat = Matrix4::Identity;
    if (GetBillboard())
    {
        Vector3 trPos   = Vector3::Zero;
        Vector3 trScale = Vector3::One;
        Quaternion rot  = Quaternion::Identity;
        if (GetSimulationSpace() == ParticleSimulationSpace::LOCAL)
        {
            trPos = tr->GetPosition();
            trScale = tr->GetScale();
        }

        if (Camera *cam = Camera::GetActive())
        {
            if (Transform *camTR = cam->GetGameObject()->GetTransform())
            {
                rot = camTR->GetRotation();
            }
        }

        modelMat = Matrix4::TranslateMatrix(trPos) *
                   Matrix4::RotateMatrix(rot) *
                   Matrix4::ScaleMatrix(trScale);
    }
    else
    {
        if (GetSimulationSpace() == ParticleSimulationSpace::LOCAL)
        {
            modelMat = tr->GetLocalToWorldMatrix();
        }
    }

    GLUniforms::SetModelMatrix(modelMat);
    GLUniforms::SetAllUniformsToShaderProgram(sp,
                                              NeededUniformFlag::MODEL |
                                              NeededUniformFlag::NORMAL);
}

void ParticleSystem::InitParticle(uint i, const Vector3 &gravity)
{
    ASSERT(i >= 0);
    ASSERT(i < GetNumParticles());

    ParticleData &particleData = m_particlesData[i];
    particleData.position = GetParticleInitialPosition();
    particleData.velocity = GetParticleInitialVelocity();
    particleData.totalLifeTime = GetLifeTime().GenerateRandom();
    particleData.remainingLifeTime = particleData.totalLifeTime;
    particleData.remainingStartTime = GetStartTime().GenerateRandom();
    particleData.force = (GetGravityMultiplier() * gravity);
    particleData.size = GetStartSize().GenerateRandom();
    particleData.startColor = GetStartColor();
    particleData.endColor = GetEndColor();

    m_particlesVBOData[i].position = Vector3::Infinity;
    m_particlesVBOData[i].size = 0.0f;
    m_particlesVBOData[i].color = Color::Zero;
}

void ParticleSystem::UpdateParticleData(uint i,
                                        float dt,
                                        const Vector3 &gravity,
                                        const Array<Collider*> &sceneColliders)
{
    ParticleData &particleData = m_particlesData[i];
    if (particleData.remainingStartTime <= 0.0f)
    {
        particleData.remainingLifeTime -= dt;
        if (particleData.remainingLifeTime > 0.0f)
        {
            float lifeTimePercent = 1.0f - (particleData.remainingLifeTime /
                                            particleData.totalLifeTime);

            Color pColor = Color::Lerp(particleData.startColor,
                                       particleData.endColor,
                                       lifeTimePercent);

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
            m_particlesVBOData[i].position = pPosition;
            m_particlesVBOData[i].size = particleData.size;
            m_particlesVBOData[i].color = pColor;
        }
        else
        {
            InitParticle(i, gravity);
        }
    }
    else
    {
        particleData.remainingStartTime -= dt;
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

void ParticleSystem::RecreateVAOForMesh()
{
    if (p_particlesVAO)
    {
        delete p_particlesVAO;
        p_particlesVAO = nullptr;
    }

    if (GetMesh())
    {
        p_particlesVAO = new VAO();

        {
            const VAO *meshVAO = GetMesh()->GetVAO();
            const VBO *particleMeshPositionsVBO =
                    meshVAO->GetVBOByLocation(Mesh::DefaultPositionsVBOLocation);
            const VBO *particleMeshNormalsVBO   =
                    meshVAO->GetVBOByLocation(Mesh::DefaultNormalsVBOLocation);
            const VBO *particleMeshUvsVBO       =
                    meshVAO->GetVBOByLocation(Mesh::DefaultUvsVBOLocation);

            uint meshVBOStride = GetMesh()->GetVBOStride();

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
            p_particlesVAO->SetIBO(meshVAO->GetIBO());
        }

        {
            int particlesPosBytesSize   = (3 * sizeof(float));
            int particlesSizeBytesSize  = (1 * sizeof(float));
            int particlesColorBytesSize = (4 * sizeof(float));
            uint particlesPosVBOOffset = 0;
            uint particlesSizeVBOOffset = particlesPosVBOOffset +
                                          particlesPosBytesSize;
            uint particlesColorVBOOffset = particlesSizeVBOOffset +
                                           particlesSizeBytesSize;
            uint particlesDataVBOStride = particlesPosBytesSize +
                                          particlesSizeBytesSize +
                                          particlesColorBytesSize;


            // Particle specific attributes
            p_particlesVAO->SetVBO(p_particleDataVBO, 3,
                                   3, GL::VertexAttribDataType::FLOAT,
                                   false,
                                   particlesDataVBOStride,
                                   particlesPosVBOOffset);
            p_particlesVAO->SetVertexAttribDivisor(3, 1);

            p_particlesVAO->SetVBO(p_particleDataVBO, 4,
                                   1, GL::VertexAttribDataType::FLOAT,
                                   false,
                                   particlesDataVBOStride,
                                   particlesSizeVBOOffset);
            p_particlesVAO->SetVertexAttribDivisor(4, 1);

            p_particlesVAO->SetVBO(p_particleDataVBO, 5,
                                   4, GL::VertexAttribDataType::FLOAT,
                                   false,
                                   particlesDataVBOStride,
                                   particlesColorVBOOffset);
            p_particlesVAO->SetVertexAttribDivisor(5, 1);
        }
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

void ParticleSystem::UpdateDataVBO()
{
    p_particleDataVBO->Update(m_particlesVBOData.Data(),
                              m_particlesVBOData.Size() * sizeof(ParticleVBOData),
                              0);
}

void ParticleSystem::CloneInto(ICloneable *clone) const
{
    Renderer::CloneInto(clone);

    ParticleSystem *psClone = SCAST<ParticleSystem*>(clone);
    psClone->SetMesh( GetMesh() );
    psClone->SetLifeTime( GetLifeTime() );
    psClone->SetStartTime( GetStartTime() );
    psClone->SetStartSize( GetStartSize() );
    psClone->SetBillboard( GetBillboard() );
    psClone->SetParticleRenderMode( GetParticleRenderMode() );
    psClone->SetStartColor( GetStartColor() );
    psClone->SetEndColor( GetEndColor() );
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

    if (metaNode.Contains("StartTime"))
    {
        SetStartTime( metaNode.Get<ComplexRandom>("StartTime") );
    }

    if (metaNode.Contains("StartSize"))
    {
        SetStartSize( metaNode.Get<ComplexRandom>("StartSize") );
    }

    if (metaNode.Contains("Billboard"))
    {
        SetBillboard( metaNode.Get<bool>("Billboard") );
    }

    if (metaNode.Contains("ParticleRenderMode"))
    {
        SetParticleRenderMode(
                    metaNode.Get<ParticleRenderMode>("ParticleRenderMode") );
    }

    if (metaNode.Contains("StartColor"))
    {
        SetStartColor( metaNode.Get<Color>("StartColor") );
    }

    if (metaNode.Contains("EndColor"))
    {
        SetEndColor( metaNode.Get<Color>("EndColor") );
    }

    if (metaNode.Contains("NumParticles"))
    {
        SetNumParticles( metaNode.Get<uint>("NumParticles") );
    }

    if (metaNode.Contains("ParticleSize"))
    {
        SetStartTime( metaNode.Get<ComplexRandom>("ParticleSize") );
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
    metaNode->Set("StartTime", GetStartTime());
    metaNode->Set("StartSize", GetStartTime());
    metaNode->Set("Billboard", GetBillboard());
    metaNode->Set("ParticleRenderMode", GetParticleRenderMode());
    metaNode->Set("StartColor", GetStartColor());
    metaNode->Set("EndColor", GetEndColor());
    metaNode->Set("ParticleSize", GetStartSize());
    metaNode->Set("NumParticles", GetNumParticles());
    metaNode->Set("GenerationShape", GetGenerationShape());
    metaNode->Set("GenerationShapeBoxSize", GetGenerationShapeBoxSize());
    metaNode->Set("GenerationShapeConeFOV", GetGenerationShapeConeFOVRads());
    metaNode->Set("SimulationSpace", GetSimulationSpace());
    metaNode->Set("PhysicsStepMode", GetPhysicsStepMode());
    metaNode->Set("GravityMultiplier", GetGravityMultiplier());
    metaNode->Set("InitialVelocityMultiplier", GetInitialVelocityMultiplier());
}


