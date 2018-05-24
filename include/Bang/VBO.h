#ifndef VBO_H
#define VBO_H

#include "Bang/Bang.h"
#include "Bang/GLObject.h"

NAMESPACE_BANG_BEGIN

class VBO : public GLObject
{
public:
    VBO();
    virtual ~VBO();

    void Fill(const void *data, int dataSize,
              GL::UsageHint usage = GL::UsageHint::STATIC_DRAW);

    GL::BindTarget GetGLBindTarget() const override;
    void Bind() const override;
    void UnBind() const override;
};

NAMESPACE_BANG_END

#endif // VBO_H
