#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include "Bang/IUniformBuffer.h"

NAMESPACE_BANG_BEGIN

template<class BufferStruct>
class UniformBuffer : public IUniformBuffer
{
public:
	UniformBuffer();
	virtual ~UniformBuffer() override;

    void Set(const BufferStruct &data);

    void SetSubData(const void *data, GLuint offset, GLuint size);

    template <class T>
    void SetSubData(const T &data, GLuint offset);

    virtual GL::BindTarget GetGLBindTarget() const;

private:
    friend class GLUniforms;
};

NAMESPACE_BANG_END

#include "Bang/UniformBuffer.tcc"

#endif // UNIFORMBUFFER_H

