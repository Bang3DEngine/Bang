#ifndef PARTICLE_H
#define PARTICLE_H

#include "Bang/Bang.h"
#include "Bang/Color.h"
#include "Bang/Array.h"
#include "Bang/Vector3.h"
#include "Bang/Collider.h"

NAMESPACE_BANG_BEGIN

class Particle
{
public:
    enum class PhysicsStepMode
    {
        EULER,
        EULER_SEMI,
        VERLET
    };

    struct Data
    {
        Vector3 prevPosition;
        Color startColor;
        Color endColor;
        float totalLifeTime;
        float remainingLifeTime;
        float remainingStartTime;
        float prevDeltaTimeSecs;
        float size;

        Vector3 position;
        Vector3 velocity;
        Vector3 force;
        uint currentFrame;
        Color currentColor;
    };

    struct Parameters
    {
        float bounciness = 1.0f;
        Array<Collider*> colliders;
        bool computeCollisions = false;
        Vector3 gravity = Vector3::Zero;
        PhysicsStepMode physicsStepMode = PhysicsStepMode::EULER;

        float animationSpeed = 0.0f;
        Vector2i animationSheetSize  = Vector2i::One;
    };

    static void Step(Particle::Data *pData, float dt, const Parameters &params);

    Particle() = delete;
    virtual ~Particle() = delete;

private:
    static void StepPositionAndVelocity(Particle::Data *pData,
                                        float dt,
                                        const Parameters &params);
    static void CollideParticle(Collider *collider,
                                const Parameters &params,
                                const Vector3 &prevPositionNoInt,
                                const Vector3 &newPositionNoInt,
                                const Vector3 &newVelocityNoInt,
                                Vector3 *newPositionAfterInt,
                                Vector3 *newVelocityAfterInt);
};

NAMESPACE_BANG_END

#endif // PARTICLE_H

