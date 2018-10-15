#include "Bang/Particle.h"

#include "Bang/Box.h"
#include "Bang/Ray.h"
#include "Bang/Mesh.h"
#include "Bang/Plane.h"
#include "Bang/Sphere.h"
#include "Bang/Segment.h"
#include "Bang/Geometry.h"
#include "Bang/Triangle.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/BoxCollider.h"
#include "Bang/MeshCollider.h"
#include "Bang/SphereCollider.h"

USING_NAMESPACE_BANG

void Particle::Step(Particle::Data *pData_,
                    float dt,
                    const Parameters &params)
{
    Particle::Data &pData = *pData_;
    if (pData.remainingStartTime <= 0.0f)
    {
        pData.remainingLifeTime -= dt;
        if (pData.remainingLifeTime > 0.0f)
        {
            float lifeTimePercent = 1.0f - (pData.remainingLifeTime /
                                            pData.totalLifeTime);

            // Render related
            pData.currentColor = Color::Lerp(pData.startColor,
                                             pData.endColor,
                                             lifeTimePercent);

            {
                float passedLifeTime = (pData.totalLifeTime -
                                        pData.remainingLifeTime);
                const Vector2i &sheetSize = params.animationSheetSize;
                uint animationFrame = SCAST<uint>(passedLifeTime *
                                                  params.animationSpeed);
                animationFrame = animationFrame % (sheetSize.x * sheetSize.y);
                pData.currentFrame = animationFrame;
            }

            const Vector3 pPrevPos = pData.position;
            Particle::StepPositionAndVelocity(&pData, dt, params);

            if (params.computeCollisions && params.colliders.Size() >= 1)
            {
                Vector3 posAfterCollision             = pData.position;
                Vector3 velocityAfterCollision        = pData.velocity;
                const Vector3 pPositionWithoutCollide = pData.position;
                const Vector3 pVelocityWithoutCollide = pData.velocity;
                for (Collider *collider : params.colliders)
                {
                    if (collider->IsEnabledRecursively())
                    {
                        CollideParticle(collider,
                                        params,
                                        pPrevPos,
                                        pPositionWithoutCollide,
                                        pVelocityWithoutCollide,
                                        &posAfterCollision,
                                        &velocityAfterCollision);
                    }
                }

                if (posAfterCollision != pPositionWithoutCollide)
                {
                    pData.prevPosition = posAfterCollision -
                                                (velocityAfterCollision * dt);
                }

                pData.position = posAfterCollision;
                pData.velocity = velocityAfterCollision;
            }
            pData.prevDeltaTimeSecs = dt;
        }
    }
    else
    {
        pData.remainingStartTime -= dt;
    }
}

void Particle::StepPositionAndVelocity(Particle::Data *pData,
                                       float dt,
                                       const Parameters &params)
{
    constexpr float pMass        = 1.0f;
    const Vector3  pPrevPos      = pData->position;
    const Vector3 &pPrevPrevPos  = pData->prevPosition;
    const Vector3 &pPrevVelocity = pData->velocity;
    const Vector3 &pForce        = pData->force;
    const Vector3  pAcc          = (pForce / pMass);

    Vector3 pNewPosition;
    Vector3 pNewVelocity;
    switch (params.physicsStepMode)
    {
        case Particle::PhysicsStepMode::EULER:
            pNewPosition = pPrevPos + (pPrevVelocity * dt);
            pNewVelocity = pPrevVelocity + (pAcc * dt);
        break;

        case Particle::PhysicsStepMode::EULER_SEMI:
            pNewVelocity = pPrevVelocity + (pAcc * dt);
            pNewPosition = pPrevPos + (pNewVelocity * dt);
        break;

        case Particle::PhysicsStepMode::VERLET:
        {
            float timeStepRatio = (dt / pData->prevDeltaTimeSecs);
            pNewPosition = pPrevPos +
                           timeStepRatio * (pPrevPos - pPrevPrevPos) +
                           (pAcc * (dt*dt));
            pNewVelocity = (pNewPosition - pPrevPos) / dt;
        }
        break;
    }

    pData->position     = pNewPosition;
    pData->velocity     = pNewVelocity;
    pData->prevPosition = pPrevPos;
}

void Particle::CollideParticle(Collider *collider,
                               const Parameters &params,
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

        case PhysicsObject::Type::MESH_COLLIDER:
        {
            MeshCollider *meshCol = SCAST<MeshCollider*>(collider);
            if (Mesh *mesh = meshCol->GetMesh())
            {
                for (int triIdx = 0; triIdx < mesh->GetNumTriangles(); ++triIdx)
                {
                    Triangle tri = mesh->GetTriangle(triIdx);
                    Transform *tr = collider->GetGameObject()->GetTransform();
                    tri = tr->GetLocalToWorldMatrix() * tri;

                    Geometry::IntersectSegmentTriangle(dispSegment,
                                                       tri,
                                                       &collided,
                                                       &collisionPoint);
                    if (collided)
                    {
                        collisionNormal = tri.GetNormal();
                    }
                }
            }
        }
        break;

        default:
        break;
    }

    if (collided)
    {
        const Vector3 cpos = collisionPoint;
        const Vector3 cnorm = collisionNormal;
        const float bouncinessEpsilon = (1.0f + params.bounciness);
        Plane collisionPlane(collisionPoint, collisionNormal);
        Vector3 newPos = newPositionNoInt - bouncinessEpsilon *
                         collisionPlane.GetDistanceTo(newPositionNoInt) * cnorm;
        newPositionAfterInt = newPos;

        Vector3 newVel = newVelocityNoInt -
                         bouncinessEpsilon * cnorm *
                         collisionPlane.GetDistanceTo(cpos + newVelocityNoInt);
        newVelocityAfterInt = newVel;
    }
}
