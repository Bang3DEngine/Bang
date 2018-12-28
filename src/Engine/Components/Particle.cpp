#include "Bang/Particle.h"

#include "Bang/Box.h"
#include "Bang/BoxCollider.h"
#include "Bang/Collider.h"
#include "Bang/GameObject.h"
#include "Bang/Geometry.h"
#include "Bang/Mesh.h"
#include "Bang/MeshCollider.h"
#include "Bang/Physics.h"
#include "Bang/PhysicsComponent.h"
#include "Bang/Plane.h"
#include "Bang/Ray.h"
#include "Bang/Segment.h"
#include "Bang/Sphere.h"
#include "Bang/SphereCollider.h"
#include "Bang/Time.h"
#include "Bang/Transform.h"
#include "Bang/Triangle.h"
#include "Bang/Vector3.h"

using namespace Bang;

void Particle::Step(Particle::Data *pData_, Time dt, const Parameters &params)
{
    float dtSecs = SCAST<float>(dt.GetSeconds());

    Particle::Data &pData = *pData_;
    if (pData.remainingStartTime <= 0.0f)
    {
        pData.remainingLifeTime -= dtSecs;
        if (pData.remainingLifeTime > 0.0f)
        {
            float lifeTimePercent =
                1.0f - (pData.remainingLifeTime / pData.totalLifeTime);

            // Render related
            pData.currentColor =
                Color::Lerp(pData.startColor, pData.endColor, lifeTimePercent);

            {
                float passedLifeTime =
                    (pData.totalLifeTime - pData.remainingLifeTime);
                const Vector2i &sheetSize = params.animationSheetSize;
                uint animationFrame =
                    SCAST<uint>(passedLifeTime * params.animationSpeed);
                animationFrame = animationFrame % (sheetSize.x * sheetSize.y);
                pData.currentFrame = animationFrame;
            }

            Particle::MoveParticle(&pData, dt, params);

            pData.prevDeltaTimeSecs = dtSecs;
        }
    }
    else
    {
        pData.remainingStartTime -= dtSecs;
    }
}

void Particle::MoveParticle(Particle::Data *pData,
                            Time dt,
                            const Particle::Parameters &params)
{
    double dtSecs = dt.GetSeconds();
    Particle::StepPositionAndVelocity(pData, SCAST<float>(dtSecs), params);
    Particle::CorrectParticleCollisions(pData, SCAST<float>(dtSecs), params);
}

void Particle::CorrectParticleCollisions(Particle::Data *pData,
                                         float dtSecs,
                                         const Particle::Parameters &params)
{
    pData->frictionForce = Vector3::Zero();
    if (params.computeCollisions && params.colliders.Size() >= 1)
    {
        Vector3 pPrevPos = pData->prevPosition;
        for (Collider *collider : params.colliders)
        {
            if (collider->IsEnabledRecursively())
            {
                const Vector3 pPositionWithoutCollide = pData->position;
                const Vector3 pVelocityWithoutCollide = pData->velocity;

                Vector3 posAfterCollision;
                Vector3 velocityAfterCollision;
                Vector3 frictionForce;
                const bool collided =
                    CollideParticle(collider,
                                    params,
                                    pPrevPos,
                                    pPositionWithoutCollide,
                                    pVelocityWithoutCollide,
                                    pData->GetGravityForce(params),
                                    &posAfterCollision,
                                    &velocityAfterCollision,
                                    &frictionForce);
                if (collided)
                {
                    pPrevPos = pData->position;
                    pData->prevPosition =
                        posAfterCollision - (velocityAfterCollision * dtSecs);
                    pData->position = posAfterCollision;
                    pData->velocity = velocityAfterCollision;
                    pData->frictionForce += frictionForce;
                }
            }
        }
    }
}

void Particle::ExecuteFixedStepped(Time totalDeltaTime,
                                   Time fixedStepDeltaTime,
                                   std::function<void(Time)> func)
{
    double dtSecs = totalDeltaTime.GetSeconds();
    double fixedDeltaTimeSecs = fixedStepDeltaTime.GetSeconds();

    uint stepsToSimulate = uint(Math::Round(dtSecs / fixedDeltaTimeSecs));
    stepsToSimulate = Math::Clamp(stepsToSimulate, 1u, 10u);
    for (uint step = 0; step < stepsToSimulate; ++step)
    {
        func(fixedStepDeltaTime);
    }
}

void Particle::FixedStepAll(
    Array<Particle::Data> *particlesDatas,
    Time totalDeltaTime,
    Time fixedStepDeltaTime,
    const Particle::Parameters &params,
    std::function<void(uint, const Particle::Parameters &)> initParticleFunc)

{
    Particle::FixedStepAll(particlesDatas,
                           totalDeltaTime,
                           fixedStepDeltaTime,
                           params,
                           initParticleFunc,
                           [](uint) { return true; });
}

void Particle::FixedStepAll(
    Array<Particle::Data> *particlesDatas,
    Time totalDeltaTime,
    Time fixedStepDeltaTime,
    const Particle::Parameters &params,
    std::function<void(uint, const Particle::Parameters &)> initParticleFunc,
    std::function<bool(uint)> canUpdateParticleFunc)
{
    Particle::FixedStepAll(particlesDatas,
                           totalDeltaTime,
                           fixedStepDeltaTime,
                           params,
                           initParticleFunc,
                           canUpdateParticleFunc,
                           [](Time) {});
}

void Particle::FixedStepAll(
    Array<Particle::Data> *particlesDatas,
    Time totalDeltaTime,
    Time fixedStepDeltaTime,
    const Particle::Parameters &params,
    std::function<void(uint i, const Particle::Parameters &)> initParticleFunc,
    std::function<bool(uint i)> canUpdateParticleFunc,
    std::function<void(Time dt)> extraFuncToExecuteBeforeEveryStep)
{
    Particle::ExecuteFixedStepped(
        totalDeltaTime, fixedStepDeltaTime, [&](Time dt) {
            extraFuncToExecuteBeforeEveryStep(dt);
            for (uint i = 0; i < particlesDatas->Size(); ++i)
            {
                if (canUpdateParticleFunc(i))
                {
                    Particle::Data &pData = particlesDatas->At(i);
                    Particle::Step(&pData, dt, params);

                    if (pData.remainingStartTime <= 0 &&
                        pData.remainingLifeTime <= 0.0f)
                    {
                        initParticleFunc(i, params);
                    }
                }
            }
        });
}

void Particle::StepPositionAndVelocity(Particle::Data *pData,
                                       float dt,
                                       const Parameters &params)
{
    constexpr float pMass = 1.0f;
    Vector3 pPrevPos = pData->position;
    const Vector3 &pPrevPrevPos = pData->prevPosition;
    const Vector3 &pPrevVelocity = pData->velocity;
    const Vector3 pForce = pData->GetNetForce(params);
    const Vector3 pAcc = (pForce / pMass);

    Vector3 pNewPosition;
    Vector3 pNewVelocity;
    switch (params.physicsStepMode)
    {
        case Particle::PhysicsStepMode::EULER:
            pNewPosition = pPrevPos + (pPrevVelocity * dt);
            pNewVelocity = pPrevVelocity + (pAcc * dt);
            pNewVelocity *= params.damping;
            break;

        case Particle::PhysicsStepMode::EULER_SEMI:
            pNewVelocity = pPrevVelocity + (pAcc * dt);
            pNewVelocity *= params.damping;
            pNewPosition = pPrevPos + (pNewVelocity * dt);
            break;

        case Particle::PhysicsStepMode::VERLET:
        {
            float timeStepRatio = (dt / pData->prevDeltaTimeSecs);
            Vector3 disp = timeStepRatio * (pPrevPos - pPrevPrevPos);
            disp += (pAcc * (dt * dt));
            pNewPosition = pPrevPos + disp * (params.damping);
            pNewVelocity = (pNewPosition - pPrevPos) / dt;
        }
        break;
    }

    pData->position = pNewPosition;
    pData->velocity = pNewVelocity;
    pData->prevPosition = pPrevPos;
}

bool Particle::CollideParticle(Collider *collider,
                               const Parameters &params,
                               const Vector3 &prevPositionNoInt,
                               const Vector3 &newPositionNoInt,
                               const Vector3 &newVelocityNoInt,
                               const Vector3 &particleGravityForce,
                               Vector3 *newPositionAfterInt,
                               Vector3 *newVelocityAfterInt,
                               Vector3 *frictionForce)
{
    Segment dispSegment(prevPositionNoInt, newPositionNoInt);

    bool collided = false;
    Vector3 collisionPoint;
    Vector3 collisionNormal;
    switch (collider->GetPhysicsComponentType())
    {
        case PhysicsComponent::Type::SPHERE_COLLIDER:
        {
            SphereCollider *spCol = SCAST<SphereCollider *>(collider);
            Sphere sphere = spCol->GetSphereWorld();

            Ray dispRay(dispSegment.GetOrigin(), dispSegment.GetDirection());
            Geometry::IntersectRaySphere(
                dispRay, sphere, &collided, &collisionPoint);
            if (collided)
            {
                collided = Vector3::SqDistance(dispSegment.GetOrigin(),
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

        case PhysicsComponent::Type::BOX_COLLIDER:
        {
            BoxCollider *boxCol = SCAST<BoxCollider *>(collider);
            Box box = boxCol->GetBoxWorld();

            Geometry::IntersectSegmentBox(
                dispSegment, box, &collided, &collisionPoint, &collisionNormal);
        }
        break;

        case PhysicsComponent::Type::MESH_COLLIDER:
        {
            MeshCollider *meshCol = SCAST<MeshCollider *>(collider);
            if (Mesh *mesh = meshCol->GetMesh())
            {
                for (uint triIdx = 0; triIdx < mesh->GetNumTriangles();
                     ++triIdx)
                {
                    Triangle tri = mesh->GetTriangle(triIdx);
                    Transform *tr = collider->GetGameObject()->GetTransform();
                    tri = tr->GetLocalToWorldMatrix() * tri;

                    Geometry::IntersectSegmentTriangle(
                        dispSegment, tri, &collided, &collisionPoint);
                    if (collided)
                    {
                        collisionNormal = tri.GetNormal();
                    }
                }
            }
        }
        break;

        default: break;
    }

    if (collided)
    {
        const Vector3 cpos = collisionPoint;
        const Vector3 cnorm = collisionNormal;
        const float bouncinessEpsilon = (1.0f + params.bounciness);
        const Plane collisionPlane(collisionPoint, collisionNormal);
        float planePointNoIntDist =
            collisionPlane.GetDistanceTo(newPositionNoInt);
        float correctionDist = Math::Sign(planePointNoIntDist) *
                               Math::Max(Math::Abs(planePointNoIntDist), 0.1f);
        Vector3 newPos = newPositionNoInt -
                         bouncinessEpsilon * correctionDist * cnorm +
                         (cnorm * 0.02f);
        *newPositionAfterInt = newPos;

        Vector3 newVel =
            newVelocityNoInt -
            bouncinessEpsilon * cnorm *
                collisionPlane.GetDistanceTo(cpos + newVelocityNoInt);
        *newVelocityAfterInt = newVel;

        float normalForceLength = Math::Abs(
            Vector3::Dot(collisionPlane.GetNormal(), particleGravityForce));
        *frictionForce =
            params.friction *
            -collisionPlane.GetProjectedVector(*newVelocityAfterInt)
                 .NormalizedSafe() *
            normalForceLength;
    }

    return collided;
}

Vector3 Particle::Data::GetNetForce(const Particle::Parameters &params) const
{
    Physics *ph = Physics::GetInstance();
    return (ph->GetGravity() * params.gravityMultiplier) + frictionForce +
           extraForce;
}

Vector3 Particle::Data::GetGravityForce(
    const Particle::Parameters &params) const
{
    Physics *ph = Physics::GetInstance();
    return ph->GetGravity() * params.gravityMultiplier;
}
