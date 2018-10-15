#include "Bang/Rope.h"

#include "Bang/VAO.h"
#include "Bang/Mesh.h"
#include "Bang/Physics.h"
#include "Bang/Material.h"
#include "Bang/Resources.h"
#include "Bang/Transform.h"
#include "Bang/GLUniforms.h"
#include "Bang/PxSceneContainer.h"

USING_NAMESPACE_BANG

Rope::Rope()
{
    SetRenderPrimitive(GL::Primitive::LINE_STRIP);
    SetNumPoints(10);
    SetPointFixed(0, true);
    GetMaterial()->SetLineWidth(3.0f);
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

    Particle::Parameters params = GetParameters();

    Time fixedStepDeltaTime = Time::Seconds(1.0 / 60);
    Particle::FixedStepAll(
                &m_particlesData,
                Time::GetDeltaTime(),
                fixedStepDeltaTime,
                params,
                [this](uint i, const Particle::Parameters &params)
                {
                    InitParticle(i, params);
                },
                [this](uint i)
                {
                    return !IsPointFixed(i);
                });

    if (IsPointFixed(0))
    {
        m_particlesData[0].position = GetGameObject()->GetTransform()->
                                      GetPosition();
    }

    for (uint i = 0; i < m_particlesData.Size(); ++i)
    {
        const Particle::Data &pData = m_particlesData[i];
        m_points[i] = pData.position;
    }

    m_validLineRendererPoints = false;
}

void Rope::OnRender()
{
    if (!m_validLineRendererPoints)
    {
        UpdateLineRendererPoints();
        m_validLineRendererPoints = true;
    }

    LineRenderer::OnRender();
}

void Rope::SetUniformsOnBind(ShaderProgram *sp)
{
    LineRenderer::SetUniformsOnBind(sp);
}

void Rope::Reset()
{
    const Particle::Parameters params = GetParameters();
    for (uint i = 0; i < GetNumPoints(); ++i)
    {
        InitParticle(i, params);
    }
}

void Rope::SetPointFixed(uint i, bool fixed)
{
    if (i < m_fixedPoints.Size())
    {
        m_fixedPoints[i] = fixed;
    }
}

void Rope::SetNumPoints(uint numPoints)
{
    ASSERT(numPoints >= 2);

    if (numPoints != GetPoints().Size())
    {
        m_points.Resize(numPoints, Vector3::Zero);
        m_fixedPoints.Resize(numPoints, false);

        m_validLineRendererPoints = false;
        m_particlesData.Resize(numPoints);

        Reset();
    }
}

void Rope::SetPoints(const Array<Vector3> &points)
{
    m_points = points;
    m_validLineRendererPoints = false;
}

bool Rope::IsPointFixed(uint i) const
{
    ASSERT(i < m_fixedPoints.Size());
    return m_fixedPoints[i];
}

uint Rope::GetNumPoints() const
{
    return m_points.Size();
}

void Rope::InitParticle(uint i, const Particle::Parameters &params)
{
    if (GetGameObject())
    {
        Transform *tr = GetGameObject()->GetTransform();
        Particle::Data *pData = &m_particlesData[i];
        *pData = Particle::Data();

        pData->position = tr->GetPosition() + Random::GetInsideUnitSphere() * 10.0f;
        pData->prevPosition = pData->position;
        pData->totalLifeTime = Math::Infinity<float>();
        pData->remainingLifeTime = pData->totalLifeTime;
        pData->remainingStartTime = 0.001f;
        pData->force = params.gravity;
        pData->size = 1.0f;
    }
}

Particle::Parameters Rope::GetParameters() const
{
    Particle::Parameters params;
    if (GetGameObject())
    {
        Physics *ph = Physics::GetInstance();
        params.bounciness = 0.1f;
        params.gravity = ph->GetGravity();
        params.colliders =
                ph->GetPxSceneContainerFromScene(GetGameObject()->GetScene())->
                GetColliders();
        params.physicsStepMode = Particle::PhysicsStepMode::VERLET;
        params.computeCollisions = true;
    }
    return params;
}

void Rope::UpdateLineRendererPoints()
{
    Array<Vector3> pointsToRender;
    for (uint i = 0; i < GetNumPoints(); ++i)
    {
        const Vector3 &worldPoint = m_points[i];
        Transform *tr = GetGameObject()->GetTransform();
        Vector3 localPoint = tr->GetLocalToWorldMatrixInv().
                             TransformedPoint(worldPoint);
        pointsToRender.PushBack(localPoint);
    }
    LineRenderer::SetPoints(pointsToRender);
}

