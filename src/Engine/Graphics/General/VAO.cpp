#include "Bang/VAO.h"

#include "Bang/GL.h"
#include "Bang/IBO.h"
#include "Bang/VBO.h"
#include "Bang/ShaderProgram.h"

USING_NAMESPACE_BANG

VAO::VAO()
{
    GL::GenVertexArrays(1, &m_idGL);
}

VAO::~VAO()
{
    GL::DeleteVertexArrays(1, &m_idGL);
}

void VAO::Bind() const
{
    GL::Bind(this);
}
void VAO::UnBind() const
{
    GL::UnBind(this);
}

void VAO::SetVBO(const VBO *vbo,
                 int location,
                 int dataComponentsCount,
                 GL::VertexAttribDataType dataType,
                 bool dataNormalized,
                 int dataStride,
                 int dataOffset)
{
    GL::Push(GL::Pushable::VAO);
    GL::Push(GL::Pushable::VBO);
    RemoveVBO(location);

    Bind();
    vbo->Bind();
    GL::EnableVertexAttribArray(location);
    GL::VertexAttribPointer(location,
                            dataComponentsCount,
                            dataType,
                            dataNormalized,
                            dataStride,
                            dataOffset);
    GL::Pop(GL::Pushable::VBO);
    GL::Pop(GL::Pushable::VAO);

    while (p_vbos.Size() <= location)
    {
        p_vbos.PushBack(nullptr);
    }
    p_vbos[location] = vbo;
}

void VAO::SetVertexAttribDivisor(uint location, uint divisor)
{
    GL::Push(GL::Pushable::VAO);

    Bind();
    GL::VertexAttribDivisor(location, divisor);

    GL::Pop(GL::Pushable::VAO);
}

void VAO::SetIBO(IBO *ibo)
{
    p_ibo = ibo;

    GL::Push(GL::Pushable::VAO);
    Bind();
    ibo->Bind();
    GL::Pop(GL::Pushable::VAO);
}

void VAO::RemoveVBO(GLint location)
{
    if (location >= 0 && location < p_vbos.Size())
    {
        GL::Push(GL::Pushable::VAO);

        Bind();
        GL::DisableVertexAttribArray(location);

        GL::Pop(GL::Pushable::VAO);

        p_vbos[location] = nullptr;
    }
}

IBO *VAO::GetIBO() const
{
    return p_ibo;
}

GL::BindTarget VAO::GetGLBindTarget() const
{
    return GL::BindTarget::VAO;
}

bool VAO::IsIndexed() const
{
    return (GetIBO() != nullptr);
}

const VBO* VAO::GetVBOByLocation(GLint location) const
{
    if (location >= p_vbos.Size())
    {
        return nullptr;
    }
    else
    {
        return p_vbos[location];
    }
}

int VAO::GetVBOCount() const
{
    return p_vbos.Size();
}
