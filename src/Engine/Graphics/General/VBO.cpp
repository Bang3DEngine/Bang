#include "Bang/VBO.h"

#include "Bang/GL.h"

using namespace Bang;

VBO::VBO()
{
    GL::GenBuffers(1, &m_idGL);
}

VBO::~VBO()
{
    GL::DeleteBuffers(1, &m_idGL);
}

void VBO::Update(const void *data, uint dataSize, uint offset)
{
    GL::Push(GL::Pushable::VBO);

    Bind();
    GL::BufferSubData(GetGLBindTarget(), offset, dataSize, data);

    GL::Pop(GL::Pushable::VBO);
}

void VBO::CreateAndFill(const void *data, uint dataSize, GL::UsageHint usage)
{
    GL::Push(GL::Pushable::VBO);

    Bind();
    GL::BufferData(GetGLBindTarget(), dataSize, data, usage);

    GL::Pop(GL::Pushable::VBO);
}

GL::BindTarget VBO::GetGLBindTarget() const
{
    return GL::BindTarget::ARRAY_BUFFER;
}

void VBO::Bind() const
{
    GL::Bind(this);
}
void VBO::UnBind() const
{
    GL::UnBind(this);
}
