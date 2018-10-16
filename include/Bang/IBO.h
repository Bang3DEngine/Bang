#ifndef IBO_H
#define IBO_H

#include "Bang/Bang.h"
#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/GLObject.h"

NAMESPACE_BANG_BEGIN

class IBO : public GLObject
{
public:
    IBO();
    virtual ~IBO() override;

    void Fill(const void *data, int dataSize,
              GL::UsageHint usage = GL::UsageHint::STATIC_DRAW);

    GL::BindTarget GetGLBindTarget() const override;
    void Bind() const override;
    void UnBind() const override;
};

NAMESPACE_BANG_END

#endif // IBO_H

