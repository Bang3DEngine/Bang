#ifndef GLOBJECT_H
#define GLOBJECT_H

#include "Bang/BangDefines.h"
#include "Bang/GL.h"

namespace Bang
{
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
}

#endif  // GLOBJECT_H
