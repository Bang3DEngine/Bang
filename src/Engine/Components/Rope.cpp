#include "Bang/Rope.h"

#include <sys/types.h>
#include <memory>

#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Color.h"
#include "Bang/GL.h"
#include "Bang/GLUniforms.h"
#include "Bang/GameObject.h"
#include "Bang/Material.h"
#include "Bang/Math.h"
#include "Bang/Matrix4.h"
#include "Bang/Matrix4.tcc"
#include "Bang/Mesh.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/PxSceneContainer.h"
#include "Bang/ShaderProgram.h"
#include "Bang/Time.h"
#include "Bang/Transform.h"
#include "Bang/Vector3.h"

namespace Bang
{
class ICloneable;
class ShaderProgram;
}  // namespace Bang

using namespace Bang;

Rope::Rope()
{
    SetRenderPrimitive(GL::Primitive::LINE_STRIP);
    SetNumPoints(10);
    SetFixedPoint(0, true);
    GetMaterial()->GetShaderProgramProperties().SetLineWidth(3.0f);
    SetCastsShadows(true);

    m_ropeDebugPointsMesh = Assets::Create<Mesh>();

    m_particleParams.physicsStepMode = Particle::PhysicsStepMode::VERLET;
    m_particleParams.gravityMultiplier = 1.0f;
    m_particleParams.computeCollisions = true;
    m_particleParams.bounciness = 0.05f;
    m_particleParams.damping = 0.95f;
}

Rope::~Rope()
{
}

void Rope::OnStart()
{
    LineRenderer::OnStart();
    Reset();
}

void Rope::OnUpdate()
{
    LineRenderer::OnUpdate();
    ASSERT(GetNumPoints() >= 2);

    Physics *ph = Physics::GetInstance();
    m_particleParams.colliders =
        ph->GetPxSceneContainerFromScene(GetGameObject()->GetScene())
            ->GetColliders();

    if (IsPointFixed(0))
    {
        m_particlesData[0].position =
            GetGameObject()->GetTransform()->GetPosition();
    }

    Time fixedStepDeltaTime = Time::Seconds(1.0 / 60);
    Particle::FixedStepAll(&m_particlesData,
                           Time::GetDeltaTime(),
                           fixedStepDeltaTime,
                           GetParameters(),
                           [this](uint i, const Particle::Parameters &params) {
                               InitParticle(i, params);
                           },
                           [this](uint i) { return !IsPointFixed(i); },
                           [this](Time) {
                               AddSpringForces();
                               // ConstrainPointsPositions();
                           });

    for (uint i = 0; i < m_particlesData.Size(); ++i)
    {
        const Particle::Data &pData = m_particlesData[i];
        m_points[i] = pData.position;
    }

    m_validLineRendererPoints = false;
}

void Rope::Bind()
{
    LineRenderer::Bind();
}

void Rope::SetUniformsOnBind(ShaderProgram *sp)
{
    LineRenderer::SetUniformsOnBind(sp);

    GLUniforms::SetModelMatrix(Matrix4::Identity());
    GLUniforms::SetAllUniformsToShaderProgram(sp);
}

void Rope::OnRender()
{
    if (!m_validLineRendererPoints)
    {
        UpdateLineRendererPoints();
        m_validLineRendererPoints = true;
    }

    if (GetActiveMaterial())
    {
        GetActiveMaterial()->SetAlbedoColor(Color::White());
        GetActiveMaterial()->Bind();
    }
    LineRenderer::OnRender();

    if (GetSeeDebugPoints() && IsStarted())
    {
        if (GetActiveMaterial())
        {
            GetActiveMaterial()->SetAlbedoColor(Color::Red());
            GetActiveMaterial()->SetReceivesLighting(false);
            GetActiveMaterial()->Bind();
        }

        GL::Push(GL::Pushable::DEPTH_STATES);
        GL::SetDepthFunc(GL::Function::ALWAYS);
        GL::PointSize(10.0f);
        GL::Render(m_ropeDebugPointsMesh.Get()->GetVAO(),
                   GL::Primitive::POINTS,
                   GetNumPoints());
        GL::Pop(GL::Pushable::DEPTH_STATES);
    }
}

AABox Rope::GetAABBox() const
{
    AABox aaBox;
    aaBox.CreateFromPositions(m_points);
    return aaBox;
}

void Rope::Reset()
{
    const Particle::Parameters &params = GetParameters();
    for (uint i = 0; i < GetNumPoints(); ++i)
    {
        InitParticle(i, params);
    }
}

void Rope::SetDamping(float damping)
{
    m_particleParams.damping = damping;
}

void Rope::SetSpringsForce(float springsForce)
{
    if (springsForce != GetSpringsForce())
    {
        m_springsForce = springsForce;
    }
}

void Rope::SetSpringsDamping(float springsDamping)
{
    m_springsDamping = springsDamping;
}

void Rope::SetFixedPoint(uint i, bool fixed)
{
    if (i < m_fixedPoints.Size())
    {
        m_fixedPoints[i] = fixed;
    }
}

void Rope::SetFixedPoints(const Array<bool> &pointsFixed)
{
    m_fixedPoints = pointsFixed;
    SetNumPoints(m_fixedPoints.Size());
}

void Rope::SetNumPoints(uint numPoints_)
{
    uint numPoints = Math::Max(numPoints_, 2u);
    if (numPoints != GetNumPoints())
    {
        m_points.Resize(numPoints, Vector3::Zero());
        m_fixedPoints.Resize(numPoints, false);
        m_particlesData.Resize(numPoints);
        m_validLineRendererPoints = false;

        Reset();
    }
}

void Rope::SetBounciness(float bounciness)
{
    m_particleParams.bounciness = bounciness;
}

void Rope::SetRopeLength(float ropeLength)
{
    m_ropeLength = ropeLength;
}

void Rope::SetPoints(const Array<Vector3> &points)
{
    m_points = points;
    m_validLineRendererPoints = false;
    SetNumPoints(points.Size());
}

void Rope::SetSeeDebugPoints(bool seeDebugPoints)
{
    m_seeDebugPoints = seeDebugPoints;
}

bool Rope::IsPointFixed(uint i) const
{
    ASSERT(i < m_fixedPoints.Size());
    return m_fixedPoints[i];
}

float Rope::GetSpringsForce() const
{
    return m_springsForce;
}

float Rope::GetRopeLength() const
{
    return m_ropeLength;
}

float Rope::GetBounciness() const
{
    return GetParameters().bounciness;
}

float Rope::GetDamping() const
{
    return GetParameters().damping;
}

uint Rope::GetNumPoints() const
{
    return m_points.Size();
}

bool Rope::GetSeeDebugPoints() const
{
    return m_seeDebugPoints;
}

float Rope::GetSpringsDamping() const
{
    return m_springsDamping;
}

void Rope::InitParticle(uint i, const Particle::Parameters &params)
{
    if (GetGameObject())
    {
        BANG_UNUSED(params);

        Transform *tr = GetGameObject()->GetTransform();
        Particle::Data *pData = &m_particlesData[i];
        *pData = Particle::Data();

        if (i == 0)
        {
            pData->position = tr->GetPosition();
        }
        else
        {
            pData->position = m_particlesData[i - 1].position +
                              Vector3::Down() * GetPartLength();
        }

        pData->prevPosition = pData->position;
        pData->totalLifeTime = Math::Infinity<float>();
        pData->remainingLifeTime = pData->totalLifeTime;
        pData->remainingStartTime = 0.001f;
        pData->size = 1.0f;
    }
}

const Particle::Parameters &Rope::GetParameters() const
{
    return m_particleParams;
}

float Rope::GetPartLength() const
{
    return GetRopeLength() / (GetNumPoints() - 1);
}

void Rope::AddSpringForces()
{
    Physics *ph = Physics::GetInstance();
    const float ropePartLength = Math::Max(GetPartLength(), 0.0001f);
    for (uint i = 0; i < GetNumPoints(); ++i)
    {
        Particle::Data *pData = &m_particlesData[i];

        Vector3 springsForce = ph->GetGravity();
        for (uint j = i - 1; j < i; ++j)
        // for (uint j = i-1; j <= i+1; j += 2)
        {
            if (j < m_particlesData.Size())
            {
                Vector3 diff = (m_particlesData[j].position - pData->position);
                float diffLength = diff.Length();
                Vector3 forceDir = diff.NormalizedSafe();
                float forceMagnitude =
                    (diffLength - ropePartLength) / ropePartLength;
                springsForce += forceDir * forceMagnitude * GetSpringsForce();
                springsForce += GetSpringsDamping() *
                                (m_particlesData[j].velocity - pData->velocity);
            }
        }
        pData->extraForce = springsForce;
    }
}

void Rope::ConstrainPointsPositions()
{
    Array<Particle::Data> &pDatas = m_particlesData;
    constexpr float RangeFactor = 0.1f;
    const float ropePartLength = GetPartLength();
    const float minLength = ropePartLength * (1.0f - RangeFactor);
    const float maxLength = ropePartLength * (1.0f + RangeFactor);
    for (uint i = 1; i < GetNumPoints(); ++i)
    {
        if (!IsPointFixed(i))
        {
            Vector3 diff = (pDatas[i].position - pDatas[i - 1].position);
            float newLength = Math::Clamp(diff.Length(), minLength, maxLength);
            pDatas[i].position =
                pDatas[i - 1].position + (newLength * diff.NormalizedSafe());
        }
    }
}

void Rope::UpdateLineRendererPoints()
{
    Array<Vector3> pointsToRender;
    for (uint i = 0; i < GetNumPoints(); ++i)
    {
        pointsToRender.PushBack(m_points[i]);
    }

    if (m_seeDebugPoints)
    {
        m_ropeDebugPointsMesh.Get()->SetPositionsPool(pointsToRender);
        m_ropeDebugPointsMesh.Get()->UpdateVAOs();
    }
    LineRenderer::SetPoints(pointsToRender);
}

void Rope::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Rope *ropeClone = SCAST<Rope *>(clone);

    ropeClone->SetFixedPoints(m_fixedPoints);
    ropeClone->SetPoints(GetPoints());

    LineRenderer::CloneInto(clone, cloneGUID);
}

void Rope::Reflect()
{
    LineRenderer::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(Rope,
                                   "Num Points",
                                   SetNumPoints,
                                   GetNumPoints,
                                   BANG_REFLECT_HINT_MIN_VALUE(2.0f));
    BANG_REFLECT_VAR_MEMBER_HINTED(Rope,
                                   "Rope Length",
                                   SetRopeLength,
                                   GetRopeLength,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.001f));
    BANG_REFLECT_VAR_MEMBER_HINTED(Rope,
                                   "Springs Force",
                                   SetSpringsForce,
                                   GetSpringsForce,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    BANG_REFLECT_VAR_MEMBER_HINTED(Rope,
                                   "Springs Damping",
                                   SetSpringsDamping,
                                   GetSpringsDamping,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    BANG_REFLECT_VAR_MEMBER_HINTED(Rope,
                                   "Bounciness",
                                   SetBounciness,
                                   GetBounciness,
                                   BANG_REFLECT_HINT_SLIDER(0.05f, 1.0f));
    BANG_REFLECT_VAR_MEMBER_HINTED(Rope,
                                   "Damping",
                                   SetDamping,
                                   GetDamping,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 1.0f));
    BANG_REFLECT_VAR_MEMBER(
        Rope, "See Debug Points", SetSeeDebugPoints, GetSeeDebugPoints);
}
