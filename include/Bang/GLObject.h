#ifndef GLOBJECT_H
#define GLOBJECT_H

#include "Bang/BangDefines.h"
#include "Bang/GL.h"

NAMESPACE_BANG_BEGIN

class GLObject
{
public:
    GLId GetGLId() const;

    virtual GL::BindTarget GetGLBindTarget() const = 0;
    virtual void Bind();
    virtual void UnBind();
    virtual void Bind() const;
    virtual void UnBind() const;

protected:
    GLObject();
    virtual ~GLObject() = default;

    GLId m_idGL = 0;
};

NAMESPACE_BANG_END

#endif // GLOBJECT_H
