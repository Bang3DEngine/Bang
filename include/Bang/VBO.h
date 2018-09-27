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

    void CreateAndFill(const void *data,
                       uint dataSize,
                       GL::UsageHint usage = GL::UsageHint::STATIC_DRAW);
    void Update(const void *data,
                uint dataSize,
                uint offset = 0);

    GL::BindTarget GetGLBindTarget() const override;
    void Bind() const override;
    void UnBind() const override;
};

NAMESPACE_BANG_END

#endif // VBO_H
