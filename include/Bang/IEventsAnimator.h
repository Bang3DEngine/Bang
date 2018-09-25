#ifndef IEVENTSANIMATOR_H
#define IEVENTSANIMATOR_H

#include "Bang/Time.h"
#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

FORWARD class Animator;
FORWARD class Animation;
FORWARD class GameObject;

class IEventsAnimator
{
    IEVENTS(IEventsAnimator)

public:
    virtual void OnAnimationTick(Animator *animator,
                                 Animation *animation,
                                 uint animationIndex,
                                 Time animationTimeWrapped,
                                 Time animationTime)
    {
        BANG_UNUSED_5(animator,
                      animationIndex,
                      animation,
                      animationTimeWrapped,
                      animationTime);
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSANIMATOR_H
