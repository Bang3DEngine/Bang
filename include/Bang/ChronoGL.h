#ifndef CHRONOGL_H
#define CHRONOGL_H

#include <GL/glew.h>

#include "Bang/BangDefines.h"

namespace Bang
{
class ChronoGL
{
public:
    ChronoGL();
    ~ChronoGL();

    void MarkBegin();
    void MarkEnd();

    double GetEllapsedSeconds() const;

private:
    GLuint m_queryId = 0;
    mutable GLuint m_prevTimeNanos = 0;

    bool IsQueryResultAvailable() const;
    GLuint GetQueryResultNanos() const;
};
}

#endif  // CHRONOGL_H
