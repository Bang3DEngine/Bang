#include "Bang/VAO.h"

#include <GL/glew.h>

#include "Bang/Array.tcc"
#include "Bang/GL.h"
#include "Bang/IBO.h"
#include "Bang/VBO.h"

using namespace Bang;

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
                 uint location,
                 uint dataComponentsCount,
                 GL::VertexAttribDataType dataType,
                 bool dataNormalized,
                 uint dataStride,
                 uint dataOffset)
{
    GL::Push(GL::Pushable::VAO);
    GL::Push(GL::Pushable::VBO);
    RemoveVBO(location);

    Bind();
    vbo->Bind();
    GL::EnableVertexAttribArray(SCAST<GLint>(location));
    GL::VertexAttribPointer(SCAST<GLint>(location),
                            SCAST<GLint>(dataComponentsCount),
                            dataType,
                            SCAST<GLint>(dataNormalized),
                            SCAST<GLint>(dataStride),
                            SCAST<GLint>(dataOffset));
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

void VAO::RemoveVBO(uint location)
{
    if (location < p_vbos.Size())
    {
        GL::Push(GL::Pushable::VAO);

        Bind();
        GL::DisableVertexAttribArray(SCAST<GLint>(location));

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

const VBO *VAO::GetVBOByLocation(uint location) const
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
