#include "Bang/GLObject.h"

#include "Bang/GL.h"

using namespace Bang;

GLObject::GLObject()
{
}

GLId GLObject::GetGLId() const
{
    return m_idGL;
}

void GLObject::Bind()
{
    const GLObject *constThis = this;
    constThis->Bind();
}
void GLObject::UnBind()
{
    const GLObject *constThis = this;
    constThis->UnBind();
}

void GLObject::Bind() const
{
    GL::Bind(this);
}
void GLObject::UnBind() const
{
    // GL::UnBind(this);
}
