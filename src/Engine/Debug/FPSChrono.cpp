#include "Bang/FPSChrono.h"

#include "Bang/List.tcc"
#include "Bang/Math.h"
#include "Bang/Time.h"

using namespace Bang;

void FPSChrono::MarkBegin()
{
    m_beginTime = Time::GetNow();
}

void FPSChrono::MarkEnd()
{
    while (SCAST<int>(m_latestDeltaTimes.Size()) > GetMeanSamples() - 1)
    {
        m_latestDeltaTimes.PopBack();
    }

    m_latestDeltaTimes.PushFront(Time::GetPassedTimeSince(m_beginTime));
}

void FPSChrono::SetMeanSamples(int meanSamples)
{
    m_meanSamples = Math::Max(meanSamples, 1);
}

double FPSChrono::GetLastTimeSeconds() const
{
    return m_latestDeltaTimes.Size() >= 1
               ? m_latestDeltaTimes.Front().GetSeconds()
               : 0.0;
}

double FPSChrono::GetMeanFPS() const
{
    if (m_latestDeltaTimes.IsEmpty())
    {
        return 0.0;
    }

    return (1.0 / Math::Max(0.00001, GetMeanSeconds()));
}

double FPSChrono::GetMeanSeconds() const
{
    if (m_latestDeltaTimes.IsEmpty())
    {
        return 0.0;
    }

    Time meanDeltas;
    for (Time delta : m_latestDeltaTimes)
    {
        meanDeltas += delta;
    }
    meanDeltas /= m_latestDeltaTimes.Size();
    return meanDeltas.GetSeconds();
}

int FPSChrono::GetMeanSamples() const
{
    return m_meanSamples;
}
