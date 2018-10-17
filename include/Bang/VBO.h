#ifndef VBO_H
#define VBO_H

#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/GLObject.h"

namespace Bang
{
class VBO : public GLObject
{
public:
    VBO();
    virtual ~VBO() override;

    void CreateAndFill(const void *data,
                       uint dataSize,
                       GL::UsageHint usage = GL::UsageHint::STATIC_DRAW);
    void Update(const void *data, uint dataSize, uint offset = 0);

    GL::BindTarget GetGLBindTarget() const override;
    void Bind() const override;
    void UnBind() const override;
};
}

#endif  // VBO_H
