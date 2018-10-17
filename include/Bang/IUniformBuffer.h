#ifndef IUNIFORMBUFFER_H
#define IUNIFORMBUFFER_H

#include <GL/glew.h>

#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/GLObject.h"

namespace Bang
{
class IUniformBuffer : public GLObject
{
public:
    virtual ~IUniformBuffer() override;

    void SetBindingPoint(int bindingPoint);
    GLuint GetBindingPoint() const;

    GL::BindTarget GetGLBindTarget() const override;

protected:
    IUniformBuffer();
    mutable void *m_mappedMemory = nullptr;

private:
    GLuint m_bindingPoint = -1;
};
}

#endif  // IUNIFORMBUFFER_H
