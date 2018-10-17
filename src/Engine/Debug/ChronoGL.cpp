#include "Bang/ChronoGL.h"

using namespace Bang;

ChronoGL::ChronoGL()
{
    glGenQueries(1, &m_queryId);
}

ChronoGL::~ChronoGL()
{
    glDeleteQueries(1, &m_queryId);
}

void ChronoGL::MarkBegin()
{
    if (IsQueryResultAvailable())
    {
        m_prevTimeNanos = GetQueryResultNanos();
    }
    glBeginQuery(GL_TIME_ELAPSED, m_queryId);
}

void ChronoGL::MarkEnd()
{
    glEndQuery(GL_TIME_ELAPSED);
}

double ChronoGL::GetEllapsedSeconds() const
{
    if (IsQueryResultAvailable())
    {
        m_prevTimeNanos = GetQueryResultNanos();
    }
    return m_prevTimeNanos;
}

bool ChronoGL::IsQueryResultAvailable() const
{
    GLint available = 0;
    glGetQueryObjectiv(m_queryId, GL_QUERY_RESULT_AVAILABLE, &available);
    return (available != 0);
}

GLuint ChronoGL::GetQueryResultNanos() const
{
    GLuint resultNanos;
    glGetQueryObjectuiv(m_queryId, GL_QUERY_RESULT, &resultNanos);
    return resultNanos;
}
