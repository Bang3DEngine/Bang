#ifndef TIMER_H
#define TIMER_H

#include <functional>

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/Time.h"

namespace Bang
{
class Timer : public Component
{
    COMPONENT_WITHOUT_CLASS_ID(Timer)

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
    virtual ~Timer() override;

private:
    bool m_running = true;
    Time m_interval;
    Time m_lastTickTime;

    Array<std::function<void()>> m_callbacks;

    void Tick();
    void ResetTimestamp();
};
}  // namespace Bang

#endif  // TIMER_H
