#include "Bang/Debug.h"

#include "Bang/Application.h"
#include "Bang/Array.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/GL.h"
#include "Bang/GLUniforms.h"
#include "Bang/GLUniforms.tcc"
#include "Bang/IEventsDebug.h"
#include "Bang/Math.h"
#include "Bang/Matrix3.h"
#include "Bang/Matrix3.tcc"
#include "Bang/Matrix4.h"
#include "Bang/Mutex.h"
#include "Bang/MutexLocker.h"
#include "Bang/Shader.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

using namespace Bang;

Debug::Debug()
{
}
Debug::~Debug()
{
}

void Debug::Message(DebugMessageType msgType,
                    const String &str,
                    int line,
                    const String &fileName)
{
    String prefix = "";
    switch (msgType)
    {
        case DebugMessageType::LOG: prefix = "[   LOG   ]: "; break;
        case DebugMessageType::DLOG: prefix = "[  DLOG   ]: "; break;
        case DebugMessageType::WARN: prefix = "[ WARNING ]: "; break;
        case DebugMessageType::ERROR: prefix = "[  ERROR  ]: "; break;
    }

    std::ostream &os =
        (msgType == DebugMessageType::ERROR ? std::cerr : std::cout);

    static Mutex outputMutex;
    {
        MutexLocker ml(&outputMutex);
        os << prefix << str << " | " << fileName << "(" << line << ")"
           << std::endl;
    }
    os.flush();

    if (Debug *dbg = Debug::GetInstance())
    {
        dbg->EventEmitter<IEventsDebug>::PropagateToListeners(
            &IEventsDebug::OnMessage, msgType, str, line, fileName);
    }
}

void Debug::Log(const String &str, int line, const String &fileName)
{
    Debug::Message(DebugMessageType::LOG, str, line, fileName);
}

void Debug::DLog(const String &str, int line, const String &fileName)
{
    Debug::Message(DebugMessageType::DLOG, str, line, fileName);
}

void Debug::Warn(const String &str, int line, const String &fileName)
{
    Debug::Message(DebugMessageType::WARN, str, line, fileName);
}

void Debug::Error(const String &str, int line, const String &fileName)
{
    Debug::Message(DebugMessageType::ERROR, str, line, fileName);
}

void Debug::PrintUniforms(Shader *shader)
{
    Debug::PrintUniforms(shader->GetGLId());
}

void PrintUniform(const String &varName,
                  int uniformIdx,
                  const String &varValueString)
{
    Debug_DLog(varName << " (" << uniformIdx << "): " << varValueString);
}

template <class T>
void PrintUniformsAllElements(GLId programId,
                              int uniformBeginIndex,
                              const GLUniforms::GLSLVar<T> &var)
{
    Array<T> allElements =
        GLUniforms::GetUniformArray<T>(programId, var.name, var.size);
    for (uint i = 0; i < allElements.Size(); ++i)
    {
        const T &value = allElements[i];
        String idxStr = String::ToString(i);
        String varName = var.name.Replace("[0]", "[" + idxStr + "]");
        PrintUniform(varName, uniformBeginIndex, String::ToString(value));
    }
}

void Debug::PrintUniforms(uint programId,
                          uint fromUniformIdx,
                          uint toUniformIdx)
{
    bool wasProgramBound =
        GL::IsBound(GL::BindTarget::SHADER_PROGRAM, programId);
    if (!wasProgramBound)
    {
        GL::Push(GL::Pushable::SHADER_PROGRAM);
        GL::Bind(GL::BindTarget::SHADER_PROGRAM, programId);
    }

    uint uniformsCount = GL::GetUniformsListSize(programId);
    uint maxIndex = Math::Min(uniformsCount, SCAST<uint>(toUniformIdx + 1));
    for (uint i = fromUniformIdx; i < maxIndex; ++i)
    {
        GL::UniformType type = GL::GetUniformTypeAt(programId, i);
        switch (type)
        {
            case GL::UniformType::FLOAT:
            case GL::UniformType::DOUBLE:
            {
                GLUniforms::GLSLVar<float> var =
                    GLUniforms::GetUniformAt<float>(programId, i);
                if (fromUniformIdx < toUniformIdx)
                {
                    PrintUniformsAllElements(programId, i, var);
                }
                else
                {
                    PrintUniform(var.name, i, String::ToString(var.value));
                }
                break;
            }
            case GL::UniformType::BYTE:
            case GL::UniformType::UNSIGNED_BYTE:
            {
                GLUniforms::GLSLVar<Byte> var =
                    GLUniforms::GetUniformAt<Byte>(programId, i);
                if (fromUniformIdx < toUniformIdx)
                {
                    PrintUniformsAllElements(programId, i, var);
                }
                else
                {
                    PrintUniform(var.name, i, String::ToString(var.value));
                }
                break;
            }
            case GL::UniformType::SHORT:
            case GL::UniformType::UNSIGNED_SHORT:
            {
                GLUniforms::GLSLVar<short> var =
                    GLUniforms::GetUniformAt<short>(programId, i);
                if (fromUniformIdx < toUniformIdx)
                {
                    PrintUniformsAllElements(programId, i, var);
                }
                else
                {
                    PrintUniform(var.name, i, String::ToString(var.value));
                }
                break;
            }
            case GL::UniformType::VEC2:
            {
                GLUniforms::GLSLVar<Vector2> var =
                    GLUniforms::GetUniformAt<Vector2>(programId, i);
                if (fromUniformIdx < toUniformIdx)
                {
                    PrintUniformsAllElements(programId, i, var);
                }
                else
                {
                    PrintUniform(var.name, i, String::ToString(var.value));
                }
                break;
            }
            case GL::UniformType::VEC3:
            {
                GLUniforms::GLSLVar<Vector3> var =
                    GLUniforms::GetUniformAt<Vector3>(programId, i);
                if (fromUniformIdx < toUniformIdx)
                {
                    PrintUniformsAllElements(programId, i, var);
                }
                else
                {
                    PrintUniform(var.name, i, String::ToString(var.value));
                }
                break;
            }
            case GL::UniformType::VEC4:
            {
                GLUniforms::GLSLVar<Vector4> var =
                    GLUniforms::GetUniformAt<Vector4>(programId, i);
                if (fromUniformIdx < toUniformIdx)
                {
                    PrintUniformsAllElements(programId, i, var);
                }
                else
                {
                    PrintUniform(var.name, i, String::ToString(var.value));
                }
                break;
            }
            case GL::UniformType::MAT3:
            {
                GLUniforms::GLSLVar<Matrix3> var =
                    GLUniforms::GetUniformAt<Matrix3>(programId, i);
                if (fromUniformIdx < toUniformIdx)
                {
                    PrintUniformsAllElements(programId, i, var);
                }
                else
                {
                    PrintUniform(var.name, i, String::ToString(var.value));
                }
                break;
            }
            case GL::UniformType::MAT4:
            {
                GLUniforms::GLSLVar<Matrix4> var =
                    GLUniforms::GetUniformAt<Matrix4>(programId, i);
                if (fromUniformIdx < toUniformIdx)
                {
                    PrintUniformsAllElements(programId, i, var);
                }
                else
                {
                    PrintUniform(var.name, i, String::ToString(var.value));
                }
                break;
            }
            case GL::UniformType::INT:
            case GL::UniformType::UNSIGNED_INT:
            case GL::UniformType::UNSIGNED_INT_24_8:
            case GL::UniformType::SAMPLER_1D:
            case GL::UniformType::SAMPLER_2D:
            case GL::UniformType::SAMPLER_3D:
            case GL::UniformType::SAMPLER_CUBE:
            case GL::UniformType::SAMPLER_1D_SHADOW:
            case GL::UniformType::SAMPLER_2D_SHADOW:
            case GL::UniformType::SAMPLER_CUBE_SHADOW:
            case GL::UniformType::SAMPLER_1D_ARRAY_SHADOW:
            case GL::UniformType::SAMPLER_2D_ARRAY_SHADOW:
            {
                GLUniforms::GLSLVar<int> var =
                    GLUniforms::GetUniformAt<int>(programId, i);
                if (fromUniformIdx < toUniformIdx)
                {
                    PrintUniformsAllElements(programId, i, var);
                }
                else
                {
                    PrintUniform(var.name, i, String::ToString(var.value));
                }
                break;
            }

            default: break;
        }
    }

    if (!wasProgramBound)
    {
        GL::Pop(GL::Pushable::SHADER_PROGRAM);
    }
}

void Debug::PrintAllUniforms()
{
    PrintUniforms();
}

void Debug::PrintUniforms()
{
    Debug::PrintUniforms(GL::GetBoundId(GL::BindTarget::SHADER_PROGRAM));
}

Debug *Debug::GetInstance()
{
    Application *app = Application::GetInstance();
    return app ? app->GetDebug() : nullptr;
}
