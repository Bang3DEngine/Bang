#pragma once

#include "Bang/GLUniforms.h"

namespace Bang
{
class Color;
}

using namespace Bang;

template <class T>
T GLUniforms::GetUniform(GLId programId, const String &uniformName)
{
    return GLUniforms::GetUniform<T>(
        programId, GL::GetUniformLocation(programId, uniformName));
}

template <class T>
T GLUniforms::GetUniform(const String &uniformName)
{
    return GLUniforms::GetUniform<T>(
        GL::GetBoundId(GL::BindTarget::SHADER_PROGRAM), uniformName);
}

template <class T>
Array<T> GLUniforms::GetUniformArray(GLId programId,
                                     const String &uniformName,
                                     int numElements)
{
    return GLUniforms::GetUniformArray<T>(
        GL::GetBoundId(GL::BindTarget::SHADER_PROGRAM),
        GL::GetUniformLocation(programId, uniformName),
        numElements);
}

template <class T>
GLUniforms::GLSLVar<T> GLUniforms::GetUniformAt(GLId shaderProgramId,
                                                GLuint uniformIndex)
{
    if (shaderProgramId == 0)
    {
        return GLUniforms::GLSLVar<T>();
    }

    GLint size;
    GLenum type;
    GLsizei length;
    constexpr GLsizei bufSize = 128;
    GLchar cname[bufSize];

    GL_CALL(glGetActiveUniform(shaderProgramId,
                               SCAST<GLuint>(uniformIndex),
                               bufSize,
                               &length,
                               &size,
                               &type,
                               cname););

    String name(cname);
    T uniformValue = GLUniforms::GetUniform<T>(shaderProgramId, name);
    return GLUniforms::GLSLVar<T>(name, uniformValue, size);
}

namespace Bang
{
template <>
inline Array<int> GLUniforms::GetUniformArray(GLId program,
                                              int uniformLocation,
                                              int numElements)
{
    ASSERT(numElements >= 1);
    Array<int> res;
    res.Resize(numElements);
    GL_CALL(glGetnUniformiv(
        program, uniformLocation, numElements * sizeof(int), res.Data()));
    return res;
}
template <>
inline int GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<int>(program, uniformLocation, 1)[0];
}
template <>
inline Array<short> GLUniforms::GetUniformArray(GLId program,
                                                int uniformLocation,
                                                int numElements)
{
    return GLUniforms::GetUniformArray<int>(
               program, uniformLocation, numElements)
        .To<Array, short>();
}
template <>
inline short GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<short>(program, uniformLocation, 1)[0];
}
template <>
inline Array<Byte> GLUniforms::GetUniformArray(GLId program,
                                               int uniformLocation,
                                               int numElements)
{
    return GLUniforms::GetUniformArray<int>(
               program, uniformLocation, numElements)
        .To<Array, Byte>();
}
template <>
inline Byte GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<Byte>(program, uniformLocation, 1)[0];
}
template <>
inline Array<bool> GLUniforms::GetUniformArray(GLId program,
                                               int uniformLocation,
                                               int numElements)
{
    ASSERT(numElements >= 1);
    Array<int> aux =
        GLUniforms::GetUniformArray<int>(program, uniformLocation, numElements);
    Array<bool> res;
    for (uint i = 0; i < aux.Size(); ++i)
    {
        res.PushBack(aux[i] != 0);
    }
    return res;
}
template <>
inline bool GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<bool>(program, uniformLocation, 1)[0];
}
template <>
inline Array<float> GLUniforms::GetUniformArray(GLId program,
                                                int uniformLocation,
                                                int numElements)
{
    ASSERT(numElements >= 1);
    Array<float> res;
    res.Resize(numElements);
    GL_CALL(glGetnUniformfv(
        program, uniformLocation, numElements * sizeof(float), res.Data()));
    return res;
}
template <>
inline float GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<float>(program, uniformLocation, 1)[0];
}
template <>
inline Array<Vector2> GLUniforms::GetUniformArray(GLId program,
                                                  int uniformLocation,
                                                  int numElements)
{
    ASSERT(numElements >= 1);
    Array<Vector2> res;
    res.Resize(numElements);
    GL_CALL(glGetnUniformfv(
        program, uniformLocation, numElements * sizeof(Vector2), &res[0][0]));
    return res;
}
template <>
inline Vector2 GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<Vector2>(program, uniformLocation, 1)[0];
}
template <>
inline Array<Vector3> GLUniforms::GetUniformArray(GLId program,
                                                  int uniformLocation,
                                                  int numElements)
{
    ASSERT(numElements >= 1);
    Array<Vector3> res;
    res.Resize(numElements);
    GL_CALL(glGetnUniformfv(
        program, uniformLocation, numElements * sizeof(Vector3), &res[0][0]));
    return res;
}
template <>
inline Vector3 GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<Vector3>(program, uniformLocation, 1)[0];
}
template <>
inline Array<Vector4> GLUniforms::GetUniformArray(GLId program,
                                                  int uniformLocation,
                                                  int numElements)
{
    ASSERT(numElements >= 1);
    Array<Vector4> res;
    res.Resize(numElements);
    GL_CALL(glGetnUniformfv(
        program, uniformLocation, numElements * sizeof(Vector4), &res[0][0]));
    return res;
}
template <>
inline Vector4 GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<Vector4>(program, uniformLocation, 1)[0];
}
template <>
inline Array<Color> GLUniforms::GetUniformArray(GLId program,
                                                int uniformLocation,
                                                int numElements)
{
    ASSERT(numElements >= 1);
    Array<Color> res;
    res.Resize(numElements);
    GL_CALL(glGetnUniformfv(
        program, uniformLocation, numElements * sizeof(Color), &res[0].r));
    return res;
}
template <>
inline Color GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<Color>(program, uniformLocation, 1)[0];
}
template <>
inline Array<Matrix3> GLUniforms::GetUniformArray(GLId program,
                                                  int uniformLocation,
                                                  int numElements)
{
    ASSERT(numElements >= 1);
    Array<Matrix3> res;
    res.Resize(numElements);
    GL_CALL(glGetnUniformfv(program,
                            uniformLocation,
                            numElements * sizeof(Matrix3),
                            &res[0][0][0]));
    return res;
}
template <>
inline Matrix3 GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<Matrix3>(program, uniformLocation, 1)[0];
}
template <>
inline Array<Matrix4> GLUniforms::GetUniformArray(GLId program,
                                                  int uniformLocation,
                                                  int numElements)
{
    ASSERT(numElements >= 1);
    Array<Matrix4> res;
    res.Resize(numElements);
    GL_CALL(glGetnUniformfv(program,
                            uniformLocation,
                            numElements * sizeof(Matrix4),
                            &res[0][0][0]));
    return res;
}
template <>
inline Matrix4 GLUniforms::GetUniform(GLId program, int uniformLocation)
{
    return GLUniforms::GetUniformArray<Matrix4>(program, uniformLocation, 1)[0];
}
}  // namespace Bang
