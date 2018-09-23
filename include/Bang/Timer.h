#ifndef TIMER_H
#define TIMER_H

#include "Bang/List.h"
#include "Bang/Time.h"
#include "Bang/Component.h"

NAMESPACE_BANG_BEGIN

class Timer : public Component
{
    COMPONENT(Timer)

public:
    void Run();
    void Stop();

    // Component
    void OnUpdate() override;

    void AddCallback(std::function<void()> callback);
    void SetInterval(Time interval);

    bool IsRunning() const;
    Time GetInterval() const;

protected:
	Timer();
	virtual ~Timer();

private:
    bool m_running = true;
    Time m_interval;
    Time m_lastTickTime;

    Array<std::function<void()>> m_callbacks;

    void Tick();
    void ResetTimestamp();
};

NAMESPACE_BANG_END

#endif // TIMER_H

