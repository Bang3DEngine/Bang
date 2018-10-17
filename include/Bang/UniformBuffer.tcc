#pragma once

#include "Bang/UniformBuffer.h"

using namespace Bang;

template <class BufferStruct>
UniformBuffer<BufferStruct>::UniformBuffer()
{
    GL::GenBuffers(1, &m_idGL);

    Bind();
    GL::BufferData(GL::BindTarget::UNIFORM_BUFFER,
                   sizeof(BufferStruct),
                   nullptr,
                   GL::UsageHint::STATIC_DRAW);
    UnBind();
}

template <class BufferStruct>
UniformBuffer<BufferStruct>::~UniformBuffer()
{
    GL::DeleteBuffers(1, &m_idGL);
}

template <class BufferStruct>
void UniformBuffer<BufferStruct>::Set(const BufferStruct &data)
{
    SetSubData(data, 0);
}

template <class BufferStruct>
void UniformBuffer<BufferStruct>::SetSubData(const void *data,
                                             GLuint offset,
                                             GLuint size)
{
    Bind();
    GL::BufferSubData(GetGLBindTarget(), offset, size, data);
    UnBind();
}

template <class BufferStruct>
template <class T>
void UniformBuffer<BufferStruct>::SetSubData(const T &data, GLuint offset)
{
    SetSubData(&data, offset, sizeof(data));
}

template <class BufferStruct>
GL::BindTarget UniformBuffer<BufferStruct>::GetGLBindTarget() const
{
    return GL::BindTarget::UNIFORM_BUFFER;
}
