#include "Bang/Debug.h"

#include "Bang/Path.h"
#include "Bang/Shader.h"
#include "Bang/String.h"
#include "Bang/Matrix3.h"
#include "Bang/Matrix4.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"
#include "Bang/GLUniforms.h"
#include "Bang/Application.h"

USING_NAMESPACE_BANG

const String Debug::c_logPrefix    = "[   LOG   ]: ";
const String Debug::c_warnPrefix   = "[ WARNING ]: ";
const String Debug::c_errorPrefix  = "[  ERROR  ]: ";

Debug::Debug() { }
Debug::~Debug() { }

void Debug::Message(DebugMessageType msgType,
                    const String &str, int line,
                    const String &fileName)
{
    String prefix = "";
    switch (msgType)
    {
        case DebugMessageType::LOG:   prefix = c_logPrefix;   break;
        case DebugMessageType::WARN:  prefix = c_warnPrefix;  break;
        case DebugMessageType::ERROR: prefix = c_errorPrefix; break;
    }

    std::cerr << prefix << str << " | " << fileName <<
                 "(" << line << ")" <<  std::endl;
    std::cerr.flush();

    Debug *dbg = Debug::GetInstance();
    if (dbg)
    {
        dbg->EventEmitter<IDebugListener>::PropagateToListeners(
                    &IDebugListener::OnMessage,
                    msgType, str, line, fileName);
    }

}

void Debug::Log(const String &str, int line, const String &fileName)
{
    Debug::Message(DebugMessageType::LOG, str, line, fileName);
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

void Debug::PrintUniforms(uint programId)
{
    int uniformsCount = GL::GetUniformsListSize(programId);
    for (int i = 0; i < uniformsCount; ++i)
    {
        GL::UniformType type = GL::GetUniformTypeAt(programId, i);
        switch (type)
        {
            case GL::UniformType::Float:
            case GL::UniformType::Double:
            {
                GLUniforms::GLSLVar<float> var = GLUniforms::GetUniformAt<float>(programId, i);
                Debug_Log(var.name << ": " << var.value);
                break;
            }
            case GL::UniformType::Byte:
            case GL::UniformType::UnsignedByte:
            {
                GLUniforms::GLSLVar<Byte> var = GLUniforms::GetUniformAt<Byte>(programId, i);
                Debug_Log(var.name << ": " << var.value);
                break;
            }
            case GL::UniformType::Short:
            case GL::UniformType::UnsignedShort:
            {
                GLUniforms::GLSLVar<short> var = GLUniforms::GetUniformAt<short>(programId, i);
                Debug_Log(var.name << ": " << var.value);
                break;
            }
            case GL::UniformType::Vector2:
            {
                GLUniforms::GLSLVar<Vector2> var = GLUniforms::GetUniformAt<Vector2>(programId, i);
                Debug_Log(var.name << ": " << var.value);
                break;
            }
            case GL::UniformType::Vector3:
            {
                GLUniforms::GLSLVar<Vector3> var = GLUniforms::GetUniformAt<Vector3>(programId, i);
                Debug_Log(var.name << ": " << var.value);
                break;
            }
            case GL::UniformType::Vector4:
            {
                GLUniforms::GLSLVar<Vector4> var = GLUniforms::GetUniformAt<Vector4>(programId, i);
                Debug_Log(var.name << ": " << var.value);
                break;
            }
            case GL::UniformType::Matrix3:
            {
                GLUniforms::GLSLVar<Matrix3> var = GLUniforms::GetUniformAt<Matrix3>(programId, i);
                Debug_Log(var.name << ": " << var.value);
                break;
            }
            case GL::UniformType::Matrix4:
            {
                GLUniforms::GLSLVar<Matrix4> var = GLUniforms::GetUniformAt<Matrix4>(programId, i);
                Debug_Log(var.name << ": " << var.value);
                break;
            }
            case GL::UniformType::Int:
            case GL::UniformType::UnsignedInt:
            case GL::UniformType::UnsignedInt_24_8:
            case GL::UniformType::Sampler1D:
            case GL::UniformType::Sampler2D:
            case GL::UniformType::Sampler3D:
            case GL::UniformType::SamplerCube:
            case GL::UniformType::Sampler1DShadow:
            case GL::UniformType::Sampler2DShadow:
            case GL::UniformType::SamplerCubeShadow:
            case GL::UniformType::Sampler1DArrayShadow:
            case GL::UniformType::Sampler2DArrayShadow:
            {
                GLUniforms::GLSLVar<int> var = GLUniforms::GetUniformAt<int>(programId, i);
                Debug_Log(var.name << ": " << var.value);
                break;
            }
        }
    }
}

void Debug::PrintAllUniforms()
{
    PrintUniforms();
    // Debug_Peek( GLUniforms::GetModelMatrix() );
    // Debug_Peek( GLUniforms::GetViewMatrix() );
    // Debug_Peek( GLUniforms::GetProjectionMatrix() );
    // Debug_Peek( GLUniforms::GetActive()->GetViewProjMode() );
}

void Debug::PrintUniforms()
{
    Debug::PrintUniforms(GL::GetBoundId(GL::BindTarget::ShaderProgram));
}

Debug *Debug::GetInstance()
{
    Application *app = Application::GetInstance();
    return app ? app->GetDebug() : nullptr;
}
