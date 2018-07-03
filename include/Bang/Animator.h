#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Bang/Bang.h"
#include "Bang/Time.h"
#include "Bang/Animation.h"
#include "Bang/Component.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

class Animator : public Component
{
    COMPONENT(Animator);

public:
	Animator();
	virtual ~Animator();

    // Component
    void OnUpdate() override;

    void SetAnimation(Animation *animation);
    void Play();
    void Stop();
    void Pause();

    bool IsPlaying() const;
    Animation* GetAnimation() const;

private:
    bool m_playing = false;
    double m_animationTimeSeconds = 0.0;
    Time::TimeT m_prevFrameTimeMillis = 0;

    RH<Animation> p_animation;
};

NAMESPACE_BANG_END

#endif // ANIMATOR_H

