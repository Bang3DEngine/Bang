#include "Bang/GLObject.h"

#include "Bang/GL.h"

USING_NAMESPACE_BANG

GLObject::GLObject() {}

GLId GLObject::GetGLId() const
{
    return m_idGL;
}

void GLObject::Bind() const
{
    GL::Bind(this);
}
void GLObject::UnBind() const
{
    // GL::UnBind(this);
}
