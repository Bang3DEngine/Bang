#include "Bang/IBO.h"

#include "Bang/GL.h"

using namespace Bang;

IBO::IBO()
{
    GL::GenBuffers(1, &m_idGL);
}

IBO::~IBO()
{
    GL::DeleteBuffers(1, &m_idGL);
}

void IBO::Fill(const void *data, int dataSize, GL::UsageHint usage)
{
    Bind();
    GL::BufferData(GetGLBindTarget(), dataSize, data, usage);
    UnBind();
}

GL::BindTarget IBO::GetGLBindTarget() const
{
    return GL::BindTarget::ELEMENT_ARRAY_BUFFER;
}

void IBO::Bind() const
{
    GL::Bind(this);
}
void IBO::UnBind() const
{
    GL::UnBind(this);
}
