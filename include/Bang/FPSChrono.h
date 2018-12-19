#ifndef FPSCHRONO_H
#define FPSCHRONO_H

#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/Time.h"

namespace Bang
{
class FPSChrono
{
public:
    FPSChrono() = default;
    ~FPSChrono() = default;

    void MarkBegin();
    void MarkEnd();

    void SetMeanSamples(int meanSamples);

    double GetLastTimeSeconds() const;
    double GetMeanFPS() const;
    double GetMeanSeconds() const;
    int GetMeanSamples() const;

private:
    List<Time> m_latestDeltaTimes;
    Time m_beginTime;
    int m_meanSamples = 100;
};
}

#endif  // FPSCHRONO_H
