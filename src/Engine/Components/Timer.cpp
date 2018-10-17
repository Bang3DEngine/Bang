#include "Bang/Timer.h"

#include "Bang/Array.tcc"

using namespace Bang;

void Timer::Run()
{
    ResetTimestamp();
    m_running = true;
    m_interval.SetSeconds(1);
}

void Timer::Stop()
{
    m_running = false;
}

void Timer::OnUpdate()
{
    Component::OnUpdate();

    if (IsRunning())
    {
        Time timeSinceLastTick = (Time::GetPassedTimeSince(m_lastTickTime));
        if (timeSinceLastTick > GetInterval())
        {
            Tick();
        }
    }
}

void Timer::Tick()
{
    for (auto callback : m_callbacks)
    {
        callback();
    }
    ResetTimestamp();
}

void Timer::ResetTimestamp()
{
    m_lastTickTime = Time::GetNow();
}

void Timer::AddCallback(std::function<void()> callback)
{
    m_callbacks.PushBack(callback);
}

void Timer::SetInterval(Time interval)
{
    m_interval = interval;
}

bool Timer::IsRunning() const
{
    return m_running;
}

Time Timer::GetInterval() const
{
    return m_interval;
}

Timer::Timer()
{
}

Timer::~Timer()
{
}
