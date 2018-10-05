#ifndef VAO_H
#define VAO_H

#include <GL/glew.h>

#include "Bang/Array.h"
#include "Bang/GLObject.h"

NAMESPACE_BANG_BEGIN

FORWARD class VBO;
FORWARD class IBO;

class VAO : public GLObject
{
public:
    VAO();
    virtual ~VAO();

    void Bind() const override;
    void UnBind() const override;

    void SetVBO(const VBO *vbo,
                uint location,
                uint dataComponentsCount,
                GL::VertexAttribDataType dataType,
                bool dataNormalized   = false,
                uint dataStride        = 0,
                uint dataOffset        = 0);
    void SetVertexAttribDivisor(uint location,
                                uint divisor);
    void SetIBO(IBO *ibo);

    void RemoveVBO(uint location);

    IBO *GetIBO() const;
    bool IsIndexed() const;
    int GetVBOCount() const;
    GL::BindTarget GetGLBindTarget() const override;
    const VBO *GetVBOByLocation(uint location) const;

private:
    Array<const VBO*> p_vbos;
    IBO* p_ibo = nullptr;
};

NAMESPACE_BANG_END

#endif // VAO_H
