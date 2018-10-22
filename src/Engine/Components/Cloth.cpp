#include "Bang/Cloth.h"

#include "Bang/Array.h"
#include "Bang/GL.h"
#include "Bang/GLUniforms.h"
#include "Bang/GameObject.h"
#include "Bang/Input.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Mesh.h"
#include "Bang/Physics.h"
#include "Bang/PxSceneContainer.h"
#include "Bang/Resources.h"
#include "Bang/Scene.h"
#include "Bang/ShaderProgram.h"
#include "Bang/Transform.h"
#include "Bang/VAO.h"
#include "Bang/Vector2.h"

using namespace Bang;

Cloth::Cloth()
{
    m_mesh = Resources::Create<Mesh>();

    GetMaterial()->SetCullFace(GL::CullFaceExt::NONE);
    GetMaterial()->SetRenderWireframe(false);

    m_debugPointsMesh = Resources::Create<Mesh>();
    m_debugPointsMaterial = Resources::Create<Material>();

    m_debugPointsMaterial.Get()->SetAlbedoColor(Color::Red);
    m_debugPointsMaterial.Get()->SetReceivesLighting(false);

    m_particleParams.physicsStepMode = Particle::PhysicsStepMode::VERLET;
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

void Cloth::SetDamping(float damping)
{
    if (damping != GetDamping())
    {
        m_particleParams.damping = damping;
    }
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

void Cloth::SetSeeDebugPoints(bool seeDebugPoints)
{
    if (seeDebugPoints != GetSeeDebugPoints())
    {
        m_seeDebugPoints = seeDebugPoints;
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

float Cloth::GetBounciness() const
{
    return GetParameters().bounciness;
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

bool Cloth::GetComputeCollisions() const
{
    return m_particleParams.computeCollisions;
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
    m_particleParams.gravity = ph->GetGravity();
    m_particleParams.colliders =
        ph->GetPxSceneContainerFromScene(GetGameObject()->GetScene())
            ->GetColliders();

    Time fixedStepDeltaTime = Time::Seconds(1.0 / 60);
    // if (Input::GetKeyDownRepeat(Key::I))
    {
        Particle::FixedStepAll(
            &m_particlesData,
            Time::GetDeltaTime(),
            fixedStepDeltaTime,
            GetParameters(),
            [this](uint i, const Particle::Parameters &params) {
                InitParticle(i, params);
            },
            [](uint) { return true; },
            [this](Time) { AddSpringForces(); });
    }

    /*
    // Relaxation
    if (Input::GetKeyDownRepeat(Key::R))
    {
        // for (uint k = 0; k < 10; ++k)
        {
            for (uint i = 0; i < GetSubdivisions(); ++i)
            {
                for (uint j = 0; j < GetSubdivisions(); ++j)
                {
                    const uint pi = (i * GetSubdivisions()) + j;
                    const Vector3 pPos = m_particlesData[pi].position;
                    const Vector3 pPrevPos = m_particlesData[i].prevPosition;
                    for (int o = 0; o < m_offsets.Size(); ++o)
                    {
                        const Vector2i &off = m_offsets[o];
                        const float offLength = m_offsetsLengths[o];
                        if ((i + off.y) < 0 ||
                            (i + off.y) >= GetSubdivisions() ||
                            (j + off.x) < 0 || (j + off.x) >= GetSubdivisions())
                        {
                            continue;
                        }

                        const int npi =
                            ((i + off.y) * GetSubdivisions() + (j + off.x));
                        const Vector3 npPos = m_particlesData[npi].position;
                        const Vector3 npPrevPos =
                            m_particlesData[npi].prevPosition;
                        const Vector3 diffToNp = (npPos - pPos);
                        const Vector3 dirToNp = diffToNp.NormalizedSafe();
                        const float dist = diffToNp.Length();
                        const float expectedDist =
                            GetSubdivisionLength() * offLength;
                        const float newDist =
                            expectedDist;  // dist;  // expectedDist;
                        const float distDiff = (expectedDist - dist);
                        // Math::Clamp(dist, expectedDist * 0.9f, expectedDist *
                        // 1.1f);
                        const Vector3 center = (pPos + npPos) * 0.5f;
                        m_particlesData[pi].position -=
                            (dirToNp * distDiff * 0.1f);
                        // m_particlesDat9a[pi].prevPosition =
                        //     (m_particlesData[pi].position - pPos)
                        //         .NormalizedSafe() *
                        //     (pPos - pPrevPos).Length();

                        m_particlesData[npi].position +=
                            (dirToNp * distDiff * 0.1f);
                        // m_particlesData[npi].prevPosition =
                        //     (m_particlesData[npi].position - npPos)
                        //         .NormalizedSafe() *
                        //     (npPos - npPrevPos).Length();
                    }
                }
            }
        }
    }

    */
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

    if (GetActiveMaterial())
    {
        GetActiveMaterial()->SetAlbedoColor(Color::White);
        GetActiveMaterial()->SetReceivesLighting(true);
        GetActiveMaterial()->Bind();
    }
    Renderer::OnRender();

    GL::Render(GetMesh()->GetVAO(),
               GL::Primitive::TRIANGLES,
               GetMesh()->GetNumVerticesIds());

    if (GetSeeDebugPoints())
    {
        if (GetActiveMaterial())
        {
            GetActiveMaterial()->SetAlbedoColor(Color::Red);
            GetActiveMaterial()->SetReceivesLighting(false);
            GetActiveMaterial()->Bind();
        }

        GL::Push(GL::Pushable::DEPTH_STATES);
        GL::SetDepthFunc(GL::Function::ALWAYS);
        GL::PointSize(10.0f);
        GL::Render(m_debugPointsMesh.Get()->GetVAO(),
                   GL::Primitive::POINTS,
                   GetTotalNumPoints());
        GL::Pop(GL::Pushable::DEPTH_STATES);
    }
}

void Cloth::OnRender(RenderPass rp)
{
    Renderer::OnRender(rp);
}

void Cloth::SetUniformsOnBind(ShaderProgram *sp)
{
    Renderer::SetUniformsOnBind(sp);

    GLUniforms::SetModelMatrix(Matrix4::Identity);
    GLUniforms::SetAllUniformsToShaderProgram(sp);
}

AABox Cloth::GetAABBox() const
{
    AABox aaBox;
    aaBox.CreateFromPositions(m_points);
    return aaBox;
}

void Cloth::CloneInto(ICloneable *clone) const
{
    Renderer::CloneInto(clone);

    Cloth *cloth = SCAST<Cloth *>(clone);
    cloth->m_points = m_points;
    cloth->m_particlesData = m_particlesData;
}

void Cloth::Reflect()
{
    Renderer::Reflect();

    GetReflectStructPtr()
        ->GetReflectVariablePtr("Material")
        ->GetHintsPtr()
        ->Update(BANG_REFLECT_HINT_HIDDEN(true));

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
                                   "See Debug Points",
                                   SetSeeDebugPoints,
                                   GetSeeDebugPoints,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 1.0f));

    BANG_REFLECT_VAR_MEMBER(Cloth,
                            "Compute collisions",
                            SetComputeCollisions,
                            GetComputeCollisions);
}

void Cloth::InitParticle(uint i, const Particle::Parameters &params)
{
    if (GetGameObject())
    {
        Physics *ph = Physics::GetInstance();

        Particle::Data *pData = &m_particlesData[i];
        *pData = Particle::Data();

        pData->position = m_points[i];
        pData->prevPosition = pData->position;
        pData->totalLifeTime = Math::Infinity<float>();
        pData->remainingLifeTime = pData->totalLifeTime;
        pData->remainingStartTime = 0.001f;
        pData->force = ph->GetGravity();
        pData->size = 1.0f;
    }
}

void Cloth::AddSpringForces()
{
    Physics *ph = Physics::GetInstance();

    const float clothSubdivLength = Math::Max(GetSubdivisionLength(), 0.0001f);
    for (int i = 0; i < GetSubdivisions(); ++i)
    {
        for (int j = 0; j < GetSubdivisions(); ++j)
        {
            Vector3 force = ph->GetGravity();

            const uint particleIndex = (i * GetSubdivisions() + j);
            const uint pi = particleIndex;
            Particle::Data *pData = &m_particlesData[pi];
            for (uint o = 0; o < m_offsets.Size(); ++o)
            {
                const Vector2i &offset = m_offsets[o];
                const float offsetLength = m_offsetsLengths[o];

                if ((i + offset.y) < 0 || (i + offset.y) >= GetSubdivisions() ||
                    (j + offset.x) < 0 || (j + offset.x) >= GetSubdivisions())
                {
                    continue;
                }

                const int neighborParticleIndex =
                    (i + offset.y) * GetSubdivisions() + (j + offset.x);
                const uint npi = neighborParticleIndex;
                float expectedLength = clothSubdivLength * offsetLength;

                Vector3 diff =
                    (m_particlesData[npi].position - pData->position);
                float diffLength = diff.Length();
                Vector3 forceDir = diff.NormalizedSafe();
                float forceMagnitude =
                    (diffLength - expectedLength) / expectedLength;
                force += forceDir * forceMagnitude * GetSpringsForce();
            }

            pData->force = force;
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

void Cloth::RecreateMesh()
{
    m_points.Clear();

    GameObject *go = GetGameObject();
    Transform *tr = (go ? go->GetTransform() : nullptr);
    Vector3 center = (tr ? tr->GetPosition() : Vector3::Zero);
    Quaternion rot = (tr ? tr->GetRotation() : Quaternion::Identity);
    const Vector2 stepSize = Vector2(GetClothSize() / (GetSubdivisions() - 1));
    for (uint i = 0; i < GetSubdivisions(); ++i)
    {
        for (uint j = 0; j < GetSubdivisions(); ++j)
        {
            Vector3 pos = rot * Vector3(i * stepSize.x, 0, j * stepSize.y);
            pos -= rot * Vector3(1, 0, 1) * (GetClothSize() * 0.5f);
            pos += center;
            m_points.PushBack(pos);
        }
    }

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
    return GetClothSize() / SCAST<float>(GetSubdivisions());
}

const Particle::Parameters &Cloth::GetParameters() const
{
    return m_particleParams;
}
