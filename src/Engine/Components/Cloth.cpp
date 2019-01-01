#include "Bang/Cloth.h"

#include "Bang/Array.h"
#include "Bang/Assets.h"
#include "Bang/GL.h"
#include "Bang/GLUniforms.h"
#include "Bang/GameObject.h"
#include "Bang/Input.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/Physics.h"
#include "Bang/PxSceneContainer.h"
#include "Bang/Scene.h"
#include "Bang/ShaderProgram.h"
#include "Bang/Transform.h"
#include "Bang/VAO.h"
#include "Bang/Vector2.h"

using namespace Bang;

Cloth::Cloth()
{
    m_mesh = Assets::Create<Mesh>();

    GetMaterial()->GetShaderProgramProperties().SetCullFace(
        GL::CullFaceExt::NONE);
    GetMaterial()->GetShaderProgramProperties().SetWireframe(false);

    m_debugPointsMesh = Assets::Create<Mesh>();
    m_debugPointsMaterial = Assets::Create<Material>();

    m_debugPointsMaterial.Get()->SetAlbedoColor(Color::Red());
    m_debugPointsMaterial.Get()->SetReceivesLighting(false);

    m_particleParams.physicsStepMode = Particle::PhysicsStepMode::VERLET;
    m_particleParams.gravityMultiplier = 1.0f;
    m_particleParams.computeCollisions = true;
    m_particleParams.bounciness = 0.05f;
    m_particleParams.damping = 0.95f;

    m_offsets.PushBack(Vector2i(-1, 0));
    m_offsets.PushBack(Vector2i(1, 0));
    m_offsets.PushBack(Vector2i(0, -1));
    m_offsets.PushBack(Vector2i(0, 1));
    m_offsets.PushBack(Vector2i(-1, -1));
    m_offsets.PushBack(Vector2i(-1, 1));
    m_offsets.PushBack(Vector2i(1, -1));
    m_offsets.PushBack(Vector2i(1, 1));
    for (const Vector2i &offset : m_offsets)
    {
        m_offsetsLengths.PushBack(Vector2(offset).Length());
    }

    SetSubdivisions(5);
    RecreateMesh();
}

Cloth::~Cloth()
{
}

void Cloth::Reset()
{
    const Particle::Parameters &params = GetParameters();
    for (uint i = 0; i < GetTotalNumPoints(); ++i)
    {
        InitParticle(i, params);
    }
    UpdateMeshPoints();
}

void Cloth::SetClothSize(float clothSize)
{
    if (clothSize != GetClothSize())
    {
        m_clothSize = clothSize;
        RecreateMesh();
    }
}

void Cloth::SetBounciness(float bounciness)
{
    if (bounciness != GetBounciness())
    {
        m_particleParams.bounciness = bounciness;
    }
}

void Cloth::SetFriction(float friction)
{
    m_particleParams.friction = friction;
}

void Cloth::SetDamping(float damping)
{
    if (damping != GetDamping())
    {
        m_particleParams.damping = damping;
    }
}

void Cloth::SetPoint(uint i, const Vector3 &pos)
{
    if (i < m_points.Size())
    {
        m_points[i] = pos;
        m_particlesData[i].position = pos;
        m_particlesData[i].prevPosition = m_particlesData[i].position;
    }
}

void Cloth::SetSpringsDamping(float springsDamping)
{
    m_springsDamping = springsDamping;
}

void Cloth::SetSubdivisions(uint subdivisions)
{
    if (subdivisions != GetSubdivisions())
    {
        m_subdivisions = subdivisions;
        RecreateMesh();
    }
}

void Cloth::SetSpringsForce(float springsForce)
{
    if (springsForce != GetSpringsForce())
    {
        m_springsForce = springsForce;
    }
}

void Cloth::SetFixedPoint(uint i, bool fixed)
{
    if (i < m_fixedPoints.Size())
    {
        m_fixedPoints[i] = fixed;
    }
}

void Cloth::SetSeeDebugPoints(bool seeDebugPoints)
{
    if (seeDebugPoints != GetSeeDebugPoints())
    {
        m_seeDebugPoints = seeDebugPoints;
        m_validMeshPoints = false;
    }
}

void Cloth::SetComputeCollisions(bool computeCollisions)
{
    m_particleParams.computeCollisions = computeCollisions;
}

Mesh *Cloth::GetMesh() const
{
    return m_mesh.Get();
}

float Cloth::GetDamping() const
{
    return GetParameters().damping;
}

float Cloth::GetFriction() const
{
    return GetParameters().friction;
}

float Cloth::GetBounciness() const
{
    return GetParameters().bounciness;
}

float Cloth::GetSpringsDamping() const
{
    return m_springsDamping;
}

float Cloth::GetClothSize() const
{
    return m_clothSize;
}

uint Cloth::GetSubdivisions() const
{
    return m_subdivisions;
}

bool Cloth::GetSeeDebugPoints() const
{
    return m_seeDebugPoints;
}

float Cloth::GetSpringsForce() const
{
    return m_springsForce;
}

bool Cloth::IsPointFixed(uint i) const
{
    return i < m_fixedPoints.Size() ? m_fixedPoints[i] : false;
}

bool Cloth::GetComputeCollisions() const
{
    return m_particleParams.computeCollisions;
}

const Array<Vector3> &Cloth::GetPoints() const
{
    return m_points;
}

const Array<bool> &Cloth::GetFixedPoints() const
{
    return m_fixedPoints;
}

void Cloth::OnStart()
{
    Component::OnStart();
    Reset();
}

void Cloth::OnUpdate()
{
    Component::OnUpdate();

    ASSERT(GetTotalNumPoints() >= 2);
    ASSERT(GetSubdivisions() >= 2);

    Physics *ph = Physics::GetInstance();
    m_particleParams.colliders =
        ph->GetPxSceneContainerFromScene(GetGameObject()->GetScene())
            ->GetColliders();

    Time fixedStepDeltaTime = Time::Seconds(1.0 / 60);
    Particle::FixedStepAll(&m_particlesData,
                           Time::GetDeltaTime(),
                           fixedStepDeltaTime,
                           GetParameters(),
                           [this](uint i, const Particle::Parameters &params) {
                               InitParticle(i, params);
                           },
                           [this](uint i) { return !IsPointFixed(i); },
                           [this](Time) { AddSpringForces(); });

    // if (Input::GetKeyDownRepeat(Key::D))
    // {
    //     for (uint i = 0; i < m_particlesData.Size(); ++i)
    //     {
    //         m_particlesData[i].position += 1.0f * Vector3::Down;
    //         Particle::CorrectParticleCollisions(
    //             &m_particlesData[i], 1.0f, GetParameters());
    //     }
    // }

    // Relaxation
    // if (Input::GetKeyDownRepeat(Key::R))
    // {
    //     ConstrainJoints();
    // }

    for (uint i = 0; i < m_particlesData.Size(); ++i)
    {
        const Particle::Data &pData = m_particlesData[i];
        m_points[i] = pData.position;
    }
    UpdateMeshPoints();

    m_validMeshPoints = false;
}

void Cloth::Bind()
{
    Renderer::Bind();
}

void Cloth::OnRender()
{
    if (!IsStarted())
    {
        RecreateMesh();
    }

    if (!m_validMeshPoints)
    {
        UpdateMeshPoints();
        m_validMeshPoints = true;
    }

    Renderer::OnRender();

    GL::Render(GetMesh()->GetVAO(),
               GL::Primitive::TRIANGLES,
               GetMesh()->GetNumVerticesIds());

    if (GetSeeDebugPoints())
    {
        Color prevMatAlbedoColor;
        bool prevReceivesLighting;
        if (GetMaterial())
        {
            prevMatAlbedoColor = GetMaterial()->GetAlbedoColor();
            prevReceivesLighting = GetMaterial()->GetReceivesLighting();
            GetMaterial()->SetAlbedoColor(Color::Red());
            GetMaterial()->SetReceivesLighting(false);
            GetMaterial()->Bind();
        }

        GL::Push(GL::Pushable::DEPTH_STATES);
        GL::SetDepthFunc(GL::Function::ALWAYS);
        GL::PointSize(10.0f);
        GL::Render(m_debugPointsMesh.Get()->GetVAO(),
                   GL::Primitive::POINTS,
                   GetTotalNumPoints());
        GL::Pop(GL::Pushable::DEPTH_STATES);

        if (GetMaterial())
        {
            GetMaterial()->SetAlbedoColor(prevMatAlbedoColor);
            GetMaterial()->SetReceivesLighting(prevReceivesLighting);
        }
    }
}

void Cloth::OnRender(RenderPass rp)
{
    Renderer::OnRender(rp);
}

void Cloth::SetUniformsOnBind(ShaderProgram *sp)
{
    Renderer::SetUniformsOnBind(sp);

    GLUniforms::SetModelMatrix(Matrix4::Identity());
    GLUniforms::SetAllUniformsToShaderProgram(sp);
}

AABox Cloth::GetAABBox() const
{
    AABox aaBox;
    aaBox.CreateFromPositions(m_points);
    return aaBox;
}

void Cloth::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Renderer::CloneInto(clone, cloneGUID);

    Cloth *cloth = SCAST<Cloth *>(clone);
    cloth->m_points = m_points;
    cloth->m_fixedPoints = m_fixedPoints;
    cloth->m_particlesData = m_particlesData;
}

void Cloth::Reflect()
{
    Renderer::Reflect();

    // GetReflectStructPtr()
    //     ->GetReflectVariablePtr("Material")
    //     ->GetHintsPtr()
    //     ->Update(BANG_REFLECT_HINT_SHOWN(false));

    ReflectVar<bool>(
        "Wireframe",
        [this](bool w) {
            GetMaterial()->GetShaderProgramProperties().SetWireframe(w);
        },
        [this]() -> bool {
            return GetMaterial()->GetShaderProgramProperties().GetWireframe();
        });

    BANG_REFLECT_VAR_MEMBER_HINTED(Cloth,
                                   "Size",
                                   SetClothSize,
                                   GetClothSize,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(Cloth,
                                   "Subdivisions",
                                   SetSubdivisions,
                                   GetSubdivisions,
                                   BANG_REFLECT_HINT_MIN_VALUE(2.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(Cloth,
                                   "Springs Force",
                                   SetSpringsForce,
                                   GetSpringsForce,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(Cloth,
                                   "Bounciness",
                                   SetBounciness,
                                   GetBounciness,
                                   BANG_REFLECT_HINT_SLIDER(0.05f, 1.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(Cloth,
                                   "Damping",
                                   SetDamping,
                                   GetDamping,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 1.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(Cloth,
                                   "Friction",
                                   SetFriction,
                                   GetFriction,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(Cloth,
                                   "Springs Damping",
                                   SetSpringsDamping,
                                   GetSpringsDamping,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(Cloth,
                                   "See Debug Points",
                                   SetSeeDebugPoints,
                                   GetSeeDebugPoints,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 1.0f));

    BANG_REFLECT_VAR_MEMBER(Cloth,
                            "Compute collisions",
                            SetComputeCollisions,
                            GetComputeCollisions);

    ReflectVar<bool>("Top Left Fixed",
                     [this](bool f) { SetFixedPoint(0, f); },
                     [this]() { return IsPointFixed(0); });
    ReflectVar<bool>(
        "Top Right Fixed",
        [this](bool f) { SetFixedPoint(GetSubdivisions() - 1, f); },
        [this]() { return IsPointFixed(GetSubdivisions() - 1); });
    ReflectVar<bool>(
        "Bot Left Fixed",
        [this](bool f) { SetFixedPoint(GetTotalNumPoints() - 1, f); },
        [this]() { return IsPointFixed(GetTotalNumPoints() - 1); });
    ReflectVar<bool>(
        "Bot Right Fixed",
        [this](bool f) {
            SetFixedPoint(GetTotalNumPoints() - GetSubdivisions(), f);
        },
        [this]() {
            return IsPointFixed(GetTotalNumPoints() - GetSubdivisions());
        });
}

void Cloth::InitParticle(uint i, const Particle::Parameters &params)
{
    BANG_UNUSED(params);

    Particle::Data *pData = &m_particlesData[i];
    *pData = Particle::Data();

    pData->position = m_points[i];
    pData->prevPosition = pData->position;
    pData->totalLifeTime = Math::Infinity<float>();
    pData->remainingLifeTime = pData->totalLifeTime;
    pData->remainingStartTime = 0.0f;
    pData->size = 1.0f;
}

void Cloth::AddSpringForces()
{
    const float clothSubdivLength = Math::Max(GetSubdivisionLength(), 0.0001f);
    for (uint i = 0; i < GetSubdivisions(); ++i)
    {
        for (uint j = 0; j < GetSubdivisions(); ++j)
        {
            Vector3 springsForce = Vector3::Zero();

            const uint particleIndex = (i * GetSubdivisions() + j);
            const uint pi = particleIndex;
            Particle::Data *pData = &m_particlesData[pi];
            for (uint o = 0; o < m_offsets.Size(); ++o)
            {
                const Vector2i &offset = m_offsets[o];
                const float offsetLength = m_offsetsLengths[o];

                const uint ii = SCAST<uint>(i + offset.y);
                const uint jj = SCAST<uint>(j + offset.x);
                if (ii >= GetSubdivisions() || jj >= GetSubdivisions())
                {
                    continue;
                }

                const uint neighborParticleIndex = ii * GetSubdivisions() + jj;
                const uint npi = neighborParticleIndex;
                float expectedLength = clothSubdivLength * offsetLength;

                Vector3 diff =
                    (m_particlesData[npi].position - pData->position);
                float diffLength = diff.Length();
                Vector3 forceDir = diff.NormalizedSafe();
                float forceMagnitude = (diffLength - expectedLength);
                if (Math::Abs(forceMagnitude) > 0.001f)
                {
                    springsForce +=
                        forceDir * forceMagnitude * GetSpringsForce();
                }

                springsForce +=
                    GetSpringsDamping() * (m_particlesData[npi].velocity -
                                           m_particlesData[pi].velocity);
            }

            pData->extraForce = springsForce;
        }
    }
}

void Cloth::UpdateMeshPoints()
{
    GetMesh()->SetPositionsPool(m_points);
    GetMesh()->UpdateVertexNormals();
    GetMesh()->UpdateVAOs();

    if (GetSeeDebugPoints())
    {
        m_debugPointsMesh.Get()->SetPositionsPool(m_points);
        m_debugPointsMesh.Get()->UpdateVAOs();
    }

    m_particlesData.Resize(GetTotalNumPoints());
}

void Cloth::ConstrainJoints()
{
    uint steps = 1;
    for (uint k = 0; k < steps; ++k)
    {
        for (uint i = 0; i < GetSubdivisions(); ++i)
        {
            for (uint j = 0; j < GetSubdivisions(); ++j)
            {
                const uint pi = (i * GetSubdivisions()) + j;
                const Vector3 pPos = m_particlesData[pi].position;
                for (uint o = 0; o < m_offsets.Size(); ++o)
                {
                    const Vector2i &off = m_offsets[o];
                    const float offLength = m_offsetsLengths[o];
                    if ((i + off.y) < 0 || (i + off.y) >= GetSubdivisions() ||
                        (j + off.x) < 0 || (j + off.x) >= GetSubdivisions())
                    {
                        continue;
                    }

                    const int npi =
                        ((i + off.y) * GetSubdivisions() + (j + off.x));
                    const Vector3 npPos = m_particlesData[npi].position;
                    const Vector3 diffToNp = (npPos - pPos);
                    const Vector3 dirToNp = diffToNp.NormalizedSafe();
                    const float expectedDist =
                        GetSubdivisionLength() * offLength;
                    const float newDist = expectedDist;
                    // Math::Clamp(dist, expectedDist * 0.9f, expectedDist *
                    // 1.1f);

                    const Vector3 center = (pPos + npPos) * 0.5f;
                    m_particlesData[pi].position =
                        center - (dirToNp * newDist * 0.5f);
                    m_particlesData[npi].position =
                        center + (dirToNp * newDist * 0.5f);
                }
            }
        }
    }

    float dtSecs = SCAST<float>(Time::GetDeltaTime().GetSeconds());
    for (uint i = 0; i < m_particlesData.Size(); ++i)
    {
        Particle::CorrectParticleCollisions(
            &m_particlesData[i], dtSecs, GetParameters());
    }
}

void Cloth::RecreateMesh()
{
    m_points.Clear();
    Array<bool> newFixedPoints;

    GameObject *go = GetGameObject();
    Transform *tr = (go ? go->GetTransform() : nullptr);
    Vector3 center = (tr ? tr->GetPosition() : Vector3::Zero());
    Quaternion rot = (tr ? tr->GetRotation() : Quaternion::Identity());
    const Vector2 stepSize = Vector2(GetClothSize() / (GetSubdivisions() - 1));
    for (uint i = 0; i < GetSubdivisions(); ++i)
    {
        for (uint j = 0; j < GetSubdivisions(); ++j)
        {
            Vector3 pos = rot * Vector3(i * stepSize.x, 0, j * stepSize.y);
            pos -= rot * Vector3(1, 0, 1) * (GetClothSize() * 0.5f);
            pos += center;
            m_points.PushBack(pos);
            newFixedPoints.PushBack(IsPointFixed(i * GetSubdivisions() + j));
        }
    }
    m_fixedPoints = newFixedPoints;

    m_particlesData.Resize(GetTotalNumPoints());
    for (uint i = 0; i < GetSubdivisions(); ++i)
    {
        m_particlesData[i].position = m_points[i];
    }

    Array<Mesh::VertexId> triangleVertexIndices;
    const uint totalNumPoints = m_points.Size();
    for (uint i = 0; i < totalNumPoints - GetSubdivisions(); ++i)
    {
        if ((i % GetSubdivisions()) != (GetSubdivisions() - 1))
        {
            if (i + GetSubdivisions() < totalNumPoints)
            {
                triangleVertexIndices.PushBack(i);
                triangleVertexIndices.PushBack(i + 1);
                triangleVertexIndices.PushBack(i + GetSubdivisions());
            }

            if (i + GetSubdivisions() + 1 < totalNumPoints)
            {
                triangleVertexIndices.PushBack(i + 1);
                triangleVertexIndices.PushBack(i + GetSubdivisions() + 1);
                triangleVertexIndices.PushBack(i + GetSubdivisions());
            }
        }
    }
    GetMesh()->SetTrianglesVertexIds(triangleVertexIndices);

    UpdateMeshPoints();
}

uint Cloth::GetTotalNumPoints() const
{
    return GetSubdivisions() * GetSubdivisions();
}

float Cloth::GetSubdivisionLength() const
{
    return GetClothSize() / SCAST<float>(GetSubdivisions() - 1);
}

const Particle::Parameters &Cloth::GetParameters() const
{
    return m_particleParams;
}
