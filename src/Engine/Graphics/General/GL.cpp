#include "Bang/GL.h"

#include <GL/gl.h>
#include <GL/glew.h>

#include <SDL.h>

#include "Bang/Debug.h"
#include "Bang/GEngine.h"
#include "Bang/GLObject.h"
#include "Bang/GLUniforms.h"
#include "Bang/List.h"
#include "Bang/Matrix3.h"
#include "Bang/Matrix4.h"
#include "Bang/Rect.h"
#include "Bang/ShaderProgram.h"
#include "Bang/StackAndValue.h"
#include "Bang/StreamOperators.h"
#include "Bang/Texture.h"
#include "Bang/TextureUnitManager.h"
#include "Bang/VAO.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"
#include "Bang/Window.h"

using namespace Bang;

// Helper functions
template <class T>
static void SetGLContextValue(StackAndValue<T> *stackAndValue, const T &value)
{
    stackAndValue->currentValue = value;
}
template <class T>
static void SetGLContextValue(StackAndValue<T> GL::*svMPtr, const T &value)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);
    SetGLContextValue(&(gl->*svMPtr), value);
}
template <class T>
static T &GetGLContextValue(StackAndValue<T> &stackAndValue)
{
    return stackAndValue.currentValue;
}
template <class T>
static T &GetGLContextValue(StackAndValue<T> GL::*svMPtr)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);
    return GetGLContextValue(gl->*svMPtr);
}
template <class T>
static const T &GetGLContextValue(const StackAndValue<T> &stackAndValue)
{
    return stackAndValue.currentValue;
}
template <class T>
static const T &GetGLContextValue(const StackAndValue<T> GL::*svMPtr)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);
    return GetGLContextValue(gl->*svMPtr);
}
template <class T>
static void PushGLContextValue(StackAndValue<T> GL::*svMPtr)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);
    StackAndValue<T> *stackAndValue = &(gl->*svMPtr);
    stackAndValue->stack.push(stackAndValue->currentValue);
}
template <class T>
static const T &PopAndGetGLContextValue(StackAndValue<T> GL::*svMPtr)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);
    StackAndValue<T> *stackAndValue = &(gl->*svMPtr);

    ASSERT(stackAndValue->stack.size() >= 1);
    stackAndValue->currentValue = stackAndValue->stack.top();
    stackAndValue->stack.pop();

    return stackAndValue->currentValue;
}

GL::GL()
{
}

GL::~GL()
{
    delete m_glUniforms;

    SDL_DestroyWindow(m_auxiliarWindowToCreateSharedGLContext);
    SDL_GL_DeleteContext(GL::GetSharedGLContext());
}

void GL::Init()
{
    // Create shared context.
    // For this, create a window, get context, and destroy window

    int VersionsMajors[] = {4, 4, 3, 3, 3};
    int VersionsMinors[] = {3, 2, 3, 2, 1};
    constexpr int NumVersionsToTry = sizeof(VersionsMajors) / sizeof(int);
    for (int i = 0; i < NumVersionsToTry; ++i)
    {
        int vMajor = VersionsMajors[i];
        int vMinor = VersionsMinors[i];

#ifdef DEBUG
        SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_FLAGS,
            SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG);
#endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, vMajor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, vMinor);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

        SDL_Window *sdlWindow = SDL_CreateWindow(
            "BangGLContextWindow", 0, 0, 1, 1, SDL_WINDOW_OPENGL);
        m_sharedGLContext = SDL_GL_CreateContext(sdlWindow);

        if (m_sharedGLContext)
        {
            m_auxiliarWindowToCreateSharedGLContext = sdlWindow;
            SDL_HideWindow(sdlWindow);
            Debug_Log("Using OpenGL context with version " << vMajor << "."
                                                           << vMinor);
            break;
        }
        else
        {
            SDL_DestroyWindow(sdlWindow);
        }
    }

    if (!m_sharedGLContext)
    {
        Debug_Error("Could not start an OpenGL context...");
    }

    // Init GLEW
    glewExperimental = true;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        Debug_Error("Glew init error: " << glewGetErrorString(glewError));
    }

    // Init GL state
    m_glUniforms = new GLUniforms();

    GL::Enable(GL::Enablable::DEPTH_TEST);
    GL::Enable(GL::Enablable::STENCIL_TEST);
    GL::Enable(GL::Enablable::CULL_FACE);
    GL::Enable(GL::Enablable::MULTISAMPLE);
    GL::Disable(GL::Enablable::MULTISAMPLE);

    m_maxDrawBuffers = GL::GetInteger(GL::Enum::MAX_DRAW_BUFFERS);
    SetGLContextValue(&GL::m_boundVAOIds, 0u);
    SetGLContextValue(&GL::m_boundVBOArrayBufferIds, 0u);
    SetGLContextValue(&GL::m_boundVBOElementsBufferIds, 0u);
    SetGLContextValue(&GL::m_boundTexture1DIds, 0u);
    SetGLContextValue(&GL::m_boundTexture2DIds, 0u);
    SetGLContextValue(&GL::m_boundTexture3DIds, 0u);
    SetGLContextValue(&GL::m_boundTextureCubeMapIds, 0u);
    SetGLContextValue(&GL::m_boundDrawFramebufferIds, 0u);
    SetGLContextValue(&GL::m_boundReadFramebufferIds, 0u);
    SetGLContextValue(&GL::m_boundShaderProgramIds, 0u);
    SetGLContextValue(&GL::m_boundUniformBufferIds, 0u);
    SetGLContextValue(&GL::m_colorMasks, {{true, true, true, true}});
    SetGLContextValue(&GL::m_lineWidths, 0.0f);
    SetGLContextValue(&GL::m_stencilValues, SCAST<Byte>(0));
    SetGLContextValue(&GL::m_stencilMasks, 0xFFu);
    SetGLContextValue(&GL::m_viewportRects, AARecti::Zero());

    SetGLContextValue(&GL::m_drawBuffers, {GL::Attachment::COLOR0});
    SetGLContextValue(&GL::m_readBuffers, GL::Attachment::COLOR0);

    SetGLContextValue(&GL::m_depthMasks, true);
    SetGLContextValue(&GL::m_depthFuncs, GL::Function::LESS);
    SetGLContextValue(&GL::m_clearColors, Color::Zero());
    SetGLContextValue(&GL::m_cullFaces, GL::Face::BACK);

    SetGLContextValue(&GL::m_scissorRectsPx, AARecti(-1, -1, -1, -1));
    SetGLContextValue(&GL::m_frontPolygonModes, GL::FILL);
    SetGLContextValue(&GL::m_backPolygonModes, GL::FILL);
    SetGLContextValue(&GL::m_frontBackPolygonModes, GL::FILL);
    SetGLContextValue(&GL::m_stencilFuncs, GL::Function::ALWAYS);
    SetGLContextValue(&GL::m_blendColors, Color::Zero());
    SetGLContextValue(&GL::m_blendSrcFactorColors, GL::BlendFactor::ONE);
    SetGLContextValue(&GL::m_blendDstFactorColors, GL::BlendFactor::ZERO);
    SetGLContextValue(&GL::m_blendSrcFactorAlphas, GL::BlendFactor::ONE);
    SetGLContextValue(&GL::m_blendDstFactorAlphas, GL::BlendFactor::ZERO);
    SetGLContextValue(&GL::m_blendEquationColors, GL::BlendEquationE::FUNC_ADD);
    SetGLContextValue(&GL::m_blendEquationAlphas, GL::BlendEquationE::FUNC_ADD);
    SetGLContextValue(&GL::m_stencilOps, GL::StencilOperation::KEEP);

    GL::PrintGLStats();
}

const std::array<GL::CubeMapDir, 6> &GL::GetAllCubeMapDirs()
{
    static bool initialized = false;
    static std::array<GL::CubeMapDir, 6> AllCubeMapDirs;

    if (!initialized)
    {
        AllCubeMapDirs = {{GL::CubeMapDir::POSITIVE_X,
                           GL::CubeMapDir::NEGATIVE_X,
                           GL::CubeMapDir::POSITIVE_Y,
                           GL::CubeMapDir::NEGATIVE_Y,
                           GL::CubeMapDir::POSITIVE_Z,
                           GL::CubeMapDir::NEGATIVE_Z}};
        initialized = true;
    }

    return AllCubeMapDirs;
}

int GL::GetCubeMapDirIndex(GL::CubeMapDir cmDir)
{
    switch (cmDir)
    {
        case GL::CubeMapDir::POSITIVE_X: return 0; break;

        case GL::CubeMapDir::NEGATIVE_X: return 1; break;

        case GL::CubeMapDir::POSITIVE_Y: return 2; break;

        case GL::CubeMapDir::NEGATIVE_Y: return 3; break;

        case GL::CubeMapDir::POSITIVE_Z: return 4; break;

        case GL::CubeMapDir::NEGATIVE_Z: return 5; break;
    }

    ASSERT(false);
    return -1;
}

Vector3 GL::GetCubeMapDirVector(GL::CubeMapDir cmDir)
{
    switch (cmDir)
    {
        case GL::CubeMapDir::TOP: return Vector3::Up(); break;
        case GL::CubeMapDir::BOT: return Vector3::Down(); break;
        case GL::CubeMapDir::LEFT: return Vector3::Left(); break;
        case GL::CubeMapDir::RIGHT: return Vector3::Right(); break;
        case GL::CubeMapDir::FRONT: return Vector3::Forward(); break;
        case GL::CubeMapDir::BACK: return Vector3::Back(); break;
    }
    return Vector3::Zero();
}

void GL::ClearError()
{
    glGetError();
}

bool GL::CheckError(int line, const String &func, const String &file)
{
    bool ok = true;
    while (true)
    {
        GLenum glError = glGetError();
        if (glError == GL_NO_ERROR)
        {
            break;
        }

        const char *err =
            reinterpret_cast<const char *>(gluErrorString(glError));
        Debug_Error("OpenGL error \"" << String(err).ToUpper()
                                      << "\" at function \""
                                      << func
                                      << "\" in "
                                      << file
                                      << ":"
                                      << line);
        // GL::PrintGLContext();
        ok = false;
    }
    return ok;
}

bool GL::CheckFramebufferError()
{
    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    const bool error = (status != GL_FRAMEBUFFER_COMPLETE);
    if (error)
    {
        String errMsg = "There was a framebuffer error: ";
        switch (status)
        {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                errMsg += "FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                errMsg += "FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                errMsg += "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                errMsg += "FRAMEBUFFER_UNSUPPORTED";
                break;
        }
        Debug_Error(errMsg);
    }
    return !error;
}

const Color &GL::GetClearColor()
{
    return GetGLContextValue(&GL::m_clearColors);
}

void GL::Clear(GLbitfield bufferBit)
{
    GL_CALL(glClear(bufferBit));
}
void GL::Clear(GL::BufferBit bufferBit)
{
    GL::Clear(SCAST<GLbitfield>(bufferBit));
}

void GL::SetClearColorBufferValue(const Color &clearColor)
{
    if (GL::GetClearColor() != clearColor)
    {
        SetGLContextValue(&GL::m_clearColors, clearColor);
        GL_CALL(glClearColor(
            clearColor.r, clearColor.g, clearColor.b, clearColor.a));
    }
}

void GL::ClearColorBuffer(const Color &clearColor,
                          bool clearR,
                          bool clearG,
                          bool clearB,
                          bool clearA)
{
    std::array<bool, 4> lastColorMask = GL::GetColorMask();
    bool differentColorMask =
        (lastColorMask[0] != clearR) || (lastColorMask[1] != clearG) ||
        (lastColorMask[2] != clearB) || (lastColorMask[3] != clearA);

    if (differentColorMask)
    {
        GL::SetColorMask(clearR, clearG, clearB, clearA);
    }

    GL::SetClearColorBufferValue(clearColor);
    GL::Clear(GL::BufferBit::COLOR);

    if (differentColorMask)
    {
        GL::SetColorMask(lastColorMask[0],
                         lastColorMask[1],
                         lastColorMask[2],
                         lastColorMask[3]);
    }
}

void GL::ClearDepthBuffer()
{
    GL::Clear(GL::BufferBit::DEPTH);
}

void GL::ClearStencilBuffer()
{
    GL::Clear(GL::BufferBit::STENCIL);
}

void GL::ClearDepthBuffer(float clearDepth)
{
    GL::SetClearDepthBufferValue(clearDepth);
    GL::Clear(GL::BufferBit::DEPTH);
}

void GL::ClearStencilBuffer(int stencilValue)
{
    GL::SetClearStencilBufferValue(stencilValue);
    GL::Clear(GL::BufferBit::STENCIL);
}

void GL::SetClearDepthBufferValue(float clearDepthBufferValue)
{
    GL_CALL(glClearDepth(clearDepthBufferValue));
}

void GL::SetClearStencilBufferValue(int clearStencilBufferValue)
{
    GL_CALL(glClearStencil(clearStencilBufferValue));
}

void GL::ClearStencilDepthBuffers(int clearStencilValue, float clearDepthValue)
{
    GL::SetClearStencilBufferValue(clearStencilValue);
    GL::SetClearDepthBufferValue(clearDepthValue);
    GL::Clear(SCAST<GLbitfield>(GL::BufferBit::DEPTH) |
              SCAST<GLbitfield>(GL::BufferBit::STENCIL));
}

void GL::ClearColorStencilDepthBuffers(const Color &clearColorValue,
                                       int clearStencilValue,
                                       float clearDepthValue)
{
    GL::SetClearColorBufferValue(clearColorValue);
    GL::SetClearStencilBufferValue(clearStencilValue);
    GL::SetClearDepthBufferValue(clearDepthValue);
    GL::Clear(SCAST<GLbitfield>(GL::BufferBit::DEPTH) |
              SCAST<GLbitfield>(GL::BufferBit::STENCIL) |
              SCAST<GLbitfield>(GL::BufferBit::COLOR));
}

void GL::EnableVertexAttribArray(int location)
{
    GL_CALL(glEnableVertexAttribArray(location));
}

void GL::DisableVertexAttribArray(int location)
{
    GL_CALL(glDisableVertexAttribArray(location));
}

void GL::VertexAttribPointer(int location,
                             int dataComponentsCount,
                             GL::VertexAttribDataType dataType,
                             bool dataNormalized,
                             int dataStride,
                             int dataOffset)
{
    GL_CALL(glVertexAttribPointer(location,
                                  dataComponentsCount,
                                  GLCAST(dataType),
                                  dataNormalized,
                                  dataStride,
                                  RCAST<void *>(dataOffset)));
}

void GL::PolygonMode(GL::Face face, GL::Enum mode)
{
    if (GL::GetPolygonMode(face) != mode)
    {
        switch (face)
        {
            case GL::Face::FRONT_AND_BACK:
                SetGLContextValue(&GL::m_frontBackPolygonModes, mode);
                break;
            case GL::Face::FRONT:
                SetGLContextValue(&GL::m_frontPolygonModes, mode);
                break;
            case GL::Face::BACK:
                SetGLContextValue(&GL::m_backPolygonModes, mode);
                break;
            default: return;
        }

        glPolygonMode(GLCAST(face), mode);
    }
}

GL::Enum GL::GetPolygonMode(GL::Face face)
{
    switch (face)
    {
        case GL::Face::FRONT_AND_BACK:
            return GetGLContextValue(&GL::m_frontBackPolygonModes);
        case GL::Face::FRONT:
            return GetGLContextValue(&GL::m_frontPolygonModes);
        case GL::Face::BACK: return GetGLContextValue(&GL::m_backPolygonModes);
        default: break;
    }
    return GL::FILL;
}

GLvoid *GL::MapBuffer(GL::BindTarget target, GL::Enum access)
{
    GL_CALL(GLvoid *ret = glMapBuffer(GLCAST(target), access));
    return ret;
}

void GL::UnMapBuffer(GL::BindTarget target)
{
    GL_CALL(glUnmapBuffer(GLCAST(target)));
}

void GL::BlendColor(const Color &blendColor)
{
    if (blendColor != GL::GetBlendColor())
    {
        SetGLContextValue(&GL::m_blendColors, blendColor);
        GL_CALL(glBlendColor(
            blendColor.r, blendColor.g, blendColor.b, blendColor.a));
    }
}

int GL::GetUniformsListSize(GLId shaderProgramId)
{
    return GL::GetProgramInteger(shaderProgramId, GL::ACTIVE_UNIFORMS);
}

GL::UniformType GL::GetUniformTypeAt(GLId shaderProgramId, GLuint uniformIndex)
{
    if (shaderProgramId == 0)
    {
        return GL::UniformType::BYTE;
    }

    GLint size = -1;
    GLenum type = -1;
    GL_CALL(glGetActiveUniform(shaderProgramId,
                               SCAST<GLuint>(uniformIndex),
                               0,
                               NULL,
                               &size,
                               &type,
                               NULL));
    return SCAST<GL::UniformType>(type);
}

void GL::BlendFunc(GL::BlendFactor srcFactor, GL::BlendFactor dstFactor)
{
    GL::BlendFuncSeparate(srcFactor, dstFactor, srcFactor, dstFactor);
}

void GL::BlendFuncSeparate(GL::BlendFactor srcFactorColor,
                           GL::BlendFactor dstFactorColor,
                           GL::BlendFactor srcFactorAlpha,
                           GL::BlendFactor dstFactorAlpha)
{
    if (srcFactorColor != GL::GetBlendSrcFactorColor() ||
        dstFactorColor != GL::GetBlendDstFactorColor() ||
        srcFactorAlpha != GL::GetBlendSrcFactorAlpha() ||
        dstFactorAlpha != GL::GetBlendDstFactorAlpha())
    {
        SetGLContextValue(&GL::m_blendSrcFactorColors, srcFactorColor);
        SetGLContextValue(&GL::m_blendDstFactorColors, dstFactorColor);
        SetGLContextValue(&GL::m_blendSrcFactorAlphas, srcFactorAlpha);
        SetGLContextValue(&GL::m_blendDstFactorAlphas, dstFactorAlpha);
        GL_CALL(glBlendFuncSeparate(GLCAST(srcFactorColor),
                                    GLCAST(dstFactorColor),
                                    GLCAST(srcFactorAlpha),
                                    GLCAST(dstFactorAlpha)));
    }
}

void GL::BlendEquation(GL::BlendEquationE blendEquation)
{
    GL::BlendEquationSeparate(blendEquation, blendEquation);
}

void GL::BlendEquationSeparate(GL::BlendEquationE blendEquationColor,
                               GL::BlendEquationE blendEquationAlpha)
{
    if (blendEquationColor != GL::GetBlendEquationColor() ||
        blendEquationAlpha != GL::GetBlendEquationAlpha())
    {
        SetGLContextValue(&GL::m_blendEquationColors, blendEquationColor);
        SetGLContextValue(&GL::m_blendEquationAlphas, blendEquationAlpha);
        GL_CALL(glBlendEquationSeparate(GLCAST(blendEquationColor),
                                        GLCAST(blendEquationAlpha)));
    }
}

void GL::Enable(GL::Enablable glEnablable)
{
    GL::SetEnabled(glEnablable, true);
}

void GL::Disable(GL::Enablable glEnablable)
{
    GL::SetEnabled(glEnablable, false);
}

void GL::SetEnabled(GL::Enablable glEnablable, bool enabled)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);

    bool canBeIndexed = (GL::CanEnablableBeIndexed(glEnablable));
    if (enabled != GL::IsEnabled(glEnablable) || canBeIndexed)
    {
        if (enabled)
        {
            GL_CALL(glEnable(GLCAST(glEnablable)););
        }
        else
        {
            GL_CALL(glDisable(GLCAST(glEnablable)););
        }

        auto &enabledisStackAndValue = gl->m_enabledVars[glEnablable];
        if (canBeIndexed)
        {
            for (int i = 0; i < GL::GetEnablableIndexMax(glEnablable); ++i)
            {
                enabledisStackAndValue.currentValue[i] = enabled;
            }
        }
        else
        {
            enabledisStackAndValue.currentValue[0] = enabled;
        }
    }
}

void GL::SetEnabledi(GL::Enablable glEnablable, int i, bool enabled)
{
    ASSERT(i >= 0 && i <= GL::GetEnablableIndexMax(glEnablable));

    if (enabled != GL::IsEnabledi(glEnablable, i))
    {
        if (enabled)
        {
            GL_CALL(glEnablei(GLCAST(glEnablable), i));
        }
        else
        {
            GL_CALL(glDisablei(GLCAST(glEnablable), i));
        }

        GL *gl = GL::GetInstance();
        ASSERT(gl);

        if (GL::CanEnablableBeIndexed(glEnablable))
        {
            gl->m_enabledVars[glEnablable].currentValue[i] = enabled;
        }
        else
        {
            gl->m_enabledVars[glEnablable].currentValue[0] = enabled;
        }
    }
}
void GL::Enablei(GL::Enablable glEnablable, int i)
{
    GL::SetEnabledi(glEnablable, i, true);
}
void GL::Disablei(GL::Enablable glEnablable, int i)
{
    GL::SetEnabledi(glEnablable, i, false);
}

bool GL::IsEnabled(GL::Enablable glEnablable)
{
    return GL::IsEnabledi(glEnablable, 0);
}
bool GL::IsEnabledi(GL::Enablable glEnablable, int index)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);

    if (!gl->m_enabledVars.ContainsKey(glEnablable))
    {
        return false;
    }
    const bool isEnabledi = gl->m_enabledVars[glEnablable].currentValue[index];
    return isEnabledi;
}

bool GL::CanEnablableBeIndexed(GL::Enablable enablable)
{
    return (enablable == GL::Enablable::BLEND);
}

int GL::GetEnablableIndexMax(GL::Enablable enablable)
{
    switch (enablable)
    {
        case GL::Enablable::BLEND:
            return GL::GetInstance()->m_maxDrawBuffers;
            break;

        default: break;
    }
    return 1;
}

void GL::BlitFramebuffer(int srcX0,
                         int srcY0,
                         int srcX1,
                         int srcY1,
                         int dstX0,
                         int dstY0,
                         int dstX1,
                         int dstY1,
                         GL::FilterMode filterMode,
                         GL::BufferBit bufferBitMask)
{
    GL_CALL(glBlitFramebuffer(srcX0,
                              srcY0,
                              srcX1,
                              srcY1,
                              dstX0,
                              dstY0,
                              dstX1,
                              dstY1,
                              GLCAST(bufferBitMask),
                              GLCAST(filterMode)));
}

void GL::BlitFramebuffer(const AARecti &srcRect,
                         const AARecti &dstRect,
                         GL::FilterMode filterMode,
                         GL::BufferBit bufferBitMask)
{
    GL::BlitFramebuffer(srcRect.GetMin().x,
                        srcRect.GetMin().y,
                        srcRect.GetMax().x,
                        srcRect.GetMax().y,
                        dstRect.GetMin().x,
                        dstRect.GetMin().y,
                        dstRect.GetMax().x,
                        dstRect.GetMax().y,
                        filterMode,
                        bufferBitMask);
}

void GL::Scissor(int x, int y, int width, int height)
{
    GL::Scissor(AARecti(x, y, x + width, y + height));
}

void GL::Scissor(const AARecti &scissorRectPx)
{
    if (scissorRectPx != GL::GetScissorRect())
    {
        if (scissorRectPx.IsValid())
        {
            SetGLContextValue(&GL::m_scissorRectsPx, scissorRectPx);
            GL_CALL(glScissor(scissorRectPx.GetMin().x,
                              scissorRectPx.GetMin().y,
                              scissorRectPx.GetWidth(),
                              scissorRectPx.GetHeight()));
        }
        else
        {
            SetGLContextValue(&GL::m_scissorRectsPx, AARecti::Zero());
            GL_CALL(glScissor(0, 0, 0, 0));
        }
    }
}

void GL::ScissorIntersecting(int x, int y, int width, int height)
{
    GL::ScissorIntersecting(AARecti(x, y, x + width, y + height));
}

void GL::ScissorIntersecting(const AARecti &scissorRectPx)
{
    AARecti prevScissor = GL::GetScissorRect();
    AARecti additiveScissor = AARecti::Intersection(prevScissor, scissorRectPx);
    GL::Scissor(additiveScissor);
}

GLId GL::CreateShader(GL::ShaderType shaderType)
{
    GL_CALL(GLId id = glCreateShader(GLCAST(shaderType)));
    return id;
}

void GL::ShaderSource(GLId shaderId, const String &sourceCode)
{
    int sourceSize = SCAST<int>(sourceCode.Size());
    const char *src = sourceCode.ToCString();
    GL_CALL(glShaderSource(shaderId, 1, &src, &sourceSize));
}

bool GL::CompileShader(GLId shaderId)
{
    GL_ClearError();
    glCompileShader(shaderId);
    GL_CALL(bool ok = GL::GetShaderInteger(shaderId, GL::COMPILE_STATUS));
    return ok;
}

int GL::GetShaderInteger(GLId shaderId, GL::Enum glEnum)
{
    int v = false;
    GL_CALL(glGetShaderiv(shaderId, glEnum, &v));
    return v;
}

String GL::GetShaderErrorMsg(GLId shaderId)
{
    int maxLength = GL::GetShaderInteger(shaderId, GL::INFO_LOG_LENGTH);

    Array<char> v(maxLength);
    GL_CALL(glGetShaderInfoLog(shaderId, maxLength, &maxLength, &v[0]));

    return String(v.begin(), v.end());
}

void GL::DeleteShader(GLId shaderId)
{
    GL_CALL(glDeleteShader(shaderId));
}

GLId GL::CreateProgram()
{
    GL_CALL(GLId id = glCreateProgram());
    return id;
}

void GL::AttachShader(GLId programId, GLId shaderId)
{
    GL_CALL(glAttachShader(programId, shaderId));
}

bool GL::LinkProgram(GLId programId)
{
    GL_CALL(glLinkProgram(programId));
    GL_CALL(bool linkedOk = GL::GetProgramInteger(programId, GL::LINK_STATUS));
    return linkedOk;
}

bool GL::ValidateProgram(GLId programId)
{
    GL_CALL(glValidateProgram(programId));
    GL_CALL(bool isValid =
                GL::GetProgramInteger(programId, GL::VALIDATE_STATUS));
    if (!isValid)
    {
        const String errMsg = GL::GetProgramErrorMsg(programId);
        Debug_Error("Invalid shader program in the current state: " << errMsg);
        PrintGLContext();
    }
    return isValid;
}

String GL::GetProgramErrorMsg(GLId programId)
{
    GLint errorLength = GL::GetProgramInteger(programId, GL::INFO_LOG_LENGTH);
    if (errorLength >= 1)
    {
        char *errorLog = new char[errorLength];
        GL_CALL(glGetProgramInfoLog(programId, errorLength, NULL, errorLog));
        String error = String(errorLog);
        delete[] errorLog;
        return error;
    }
    return "";
}

int GL::GetProgramInteger(GLId programId, GL::Enum glEnum)
{
    int result = 0;
    GL::GetProgramIntegers(programId, glEnum, &result);
    return result;
}

void GL::GetProgramIntegers(GLId programId, GL::Enum glEnum, GLint *ints)
{
    GL_CALL(glGetProgramiv(programId, glEnum, ints));
}

void GL::BindAttribLocation(GLId programId,
                            int location,
                            const String &attribName)
{
    GL_CALL(glBindAttribLocation(programId, location, attribName.ToCString()));
}

void GL::BindFragDataLocation(GLId programId,
                              int location,
                              const String &fragDataName)
{
    GL_CALL(
        glBindFragDataLocation(programId, location, fragDataName.ToCString()));
}

int GL::GetUniformLocation(const String &uniformName)
{
    return GL::GetUniformLocation(
        GL::GetBoundId(GL::BindTarget::SHADER_PROGRAM), uniformName);
}
int GL::GetUniformLocation(GLId programId, const String &uniformName)
{
    GL_CALL(int location =
                glGetUniformLocation(programId, uniformName.ToCString()));
    return location;
}

void GL::DeleteProgram(GLId programId)
{
    GL::OnDeletedGLObjects(GL::BindTarget::SHADER_PROGRAM, 1, &programId);
    GL_CALL(glDeleteProgram(programId));
}

void GL::FramebufferTexture(GL::FramebufferTarget target,
                            GL::Attachment attachment,
                            GLId textureId,
                            uint mipMapLevel)
{
    GL_CALL(glFramebufferTexture(
                GLCAST(target), GLCAST(attachment), textureId, mipMapLevel););
}

void GL::FramebufferTexture2D(GL::FramebufferTarget target,
                              GL::Attachment attachment,
                              GL::TextureTarget texTarget,
                              GLId textureId,
                              uint mipMapLevel)
{
    GL_CALL(glFramebufferTexture2D(GLCAST(target),
                                   GLCAST(attachment),
                                   GLCAST(texTarget),
                                   textureId,
                                   mipMapLevel););
}

void GL::BindRenderbuffer(GL::RenderbufferTarget target, GLId renderbufferId)
{
    GL_CALL(glBindRenderbuffer(GLCAST(target), renderbufferId));
}

void GL::RenderbufferStorage(GL::RenderbufferTarget target,
                             GL::RenderbufferFormat format,
                             int width,
                             int height)
{
    GL_CALL(
        glRenderbufferStorage(GLCAST(target), GLCAST(format), width, height));
}

void GL::FramebufferRenderbuffer(GL::FramebufferTarget target,
                                 GL::Attachment attachment,
                                 GL::RenderbufferTarget rbTarget,
                                 GLId renderbufferId)
{
    GL_CALL(glFramebufferRenderbuffer(
        GLCAST(target), GLCAST(attachment), GLCAST(rbTarget), renderbufferId));
}

void GL::DrawBuffers(const Array<GL::Attachment> &drawAttachments)
{
    // if (!Containers::Equal(gl->m_drawBuffers.Begin(),
    // gl->m_drawBuffers.End(),
    //                        drawAttachments.Begin()))
    {
        SetGLContextValue(&GL::m_drawBuffers, drawAttachments);
        if (drawAttachments.Size() >= 1)
        {
            GL_CALL(glDrawBuffers(drawAttachments.Size(),
                                  (const GLenum *)(&drawAttachments[0])));
        }
        else
        {
            GL::DrawBuffers({GL::Attachment::NONE});
        }
    }
}

void GL::ReadBuffer(GL::Attachment readAttachment)
{
    // if (gl->m_readBuffer != readAttachment)
    {
        SetGLContextValue(&GL::m_readBuffers, readAttachment);
        GL_CALL(glReadBuffer(GLCAST(readAttachment)));
    }
}

void GL::ReadPixels(int x,
                    int y,
                    int width,
                    int height,
                    GL::ColorComp inputComp,
                    GL::DataType outputDataType,
                    void *pixels)
{
    GL_CALL(glReadPixels(x,
                         y,
                         width,
                         height,
                         GLCAST(inputComp),
                         GLCAST(outputDataType),
                         pixels););
}

void GL::ReadPixels(const AARecti &readRect,
                    GL::ColorComp inputComp,
                    GL::DataType outputDataType,
                    void *pixels)
{
    ASSERT(readRect.IsValid());
    GL::ReadPixels(readRect.GetMin().x,
                   readRect.GetMin().y,
                   readRect.GetWidth(),
                   readRect.GetHeight(),
                   inputComp,
                   outputDataType,
                   pixels);
}

void GL::Finish()
{
    glFinish();
}
void GL::Flush()
{
    glFlush();
}

void GL::Uniform(const String &name, int value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(const String &name, float value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(const String &name, double value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(const String &name, bool value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(const String &name, const Matrix3f &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(const String &name, const Matrix4f &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(const String &name, const Color &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(const String &name, const Vector2 &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(const String &name, const Vector3 &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(const String &name, const Vector4 &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<int> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<bool> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<float> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<double> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<Color> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<Vector2> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<Vector3> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<Vector4> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<Matrix3> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}
void GL::Uniform(const String &name, const Array<Matrix4> &value)
{
    GL::Uniform(GL::GetUniformLocation(name), value);
}

void GL::Uniform(int location, int value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform1i(location, value));
    }
}
void GL::Uniform(int location, float value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform1f(location, value));
    }
}
void GL::Uniform(int location, double value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform1d(location, value));
    }
}
void GL::Uniform(int location, bool value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform1i(location, value ? 1 : 0));
    }
}
void GL::Uniform(int location, const Matrix3f &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniformMatrix3fv(location, 1, false, value.Data()));
    }
}
void GL::Uniform(int location, const Matrix4f &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniformMatrix4fv(location, 1, false, value.Data()));
    }
}
void GL::Uniform(int location, const Color &value)
{
    GL::Uniform(location, value.ToVector4());
}
void GL::Uniform(int location, const Vector2 &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform2fv(location, 1, value.Data()));
    }
}
void GL::Uniform(int location, const Vector3 &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform3fv(location, 1, value.Data()));
    }
}
void GL::Uniform(int location, const Vector4 &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform4fv(location, 1, value.Data()));
    }
}
void GL::Uniform(int location, const Array<int> &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform1iv(location, value.Size(), value.Data()));
    }
}
void GL::Uniform(int location, const Array<bool> &value)
{
    Array<int> valueInts;
    for (bool b : value)
    {
        valueInts.PushBack(b ? 1 : 0);
    }

    if (location >= 0)
    {
        GL_CALL(glUniform1iv(location, value.Size(), valueInts.Data()));
    }
}
void GL::Uniform(int location, const Array<float> &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform1fv(location, value.Size(), value.Data()));
    }
}

void GL::Uniform(int location, const Array<double> &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform1dv(
            location, value.Size(), RCAST<const GLdouble *>(value.Data())));
    }
}

void GL::Uniform(int location, const Array<Color> &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform4fv(
            location, value.Size(), RCAST<const GLfloat *>(value.Data())));
    }
}
void GL::Uniform(int location, const Array<Vector2> &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform2fv(
            location, value.Size(), RCAST<const GLfloat *>(value.Data())));
    }
}
void GL::Uniform(int location, const Array<Vector3> &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform3fv(
            location, value.Size(), RCAST<const GLfloat *>(value.Data())));
    }
}
void GL::Uniform(int location, const Array<Vector4> &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniform4fv(
            location, value.Size(), RCAST<const GLfloat *>(value.Data())));
    }
}
void GL::Uniform(int location, const Array<Matrix3> &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniformMatrix3fv(location,
                                   value.Size(),
                                   false,
                                   RCAST<const GLfloat *>(value.Data())));
    }
}
void GL::Uniform(int location, const Array<Matrix4> &value)
{
    if (location >= 0)
    {
        GL_CALL(glUniformMatrix4fv(location,
                                   value.Size(),
                                   false,
                                   RCAST<const GLfloat *>(value.Data())));
    }
}

void GL::PixelStore(GL::Enum pixelStoreEnum, int n)
{
    GL_CALL(glPixelStorei(pixelStoreEnum, n));
}

void GL::GenerateMipMap(GL::TextureTarget textureTarget)
{
    GL_CALL(glGenerateMipmap(GLCAST(textureTarget)));
}

void GL::TexImage2D(GL::TextureTarget textureTarget,
                    uint textureWidth,
                    uint textureHeight,
                    GL::ColorFormat textureColorFormat,
                    GL::ColorComp inputDataColorComp,
                    GL::DataType inputDataType,
                    const void *data)
{
    GL_CALL(glTexImage2D(GLCAST(textureTarget),
                         0,
                         GLCAST(textureColorFormat),
                         textureWidth,
                         textureHeight,
                         0,
                         GLCAST(inputDataColorComp),
                         GLCAST(inputDataType),
                         data));
}

void GL::TexImage3D(GL::TextureTarget textureTarget,
                    uint textureWidth,
                    uint textureHeight,
                    uint textureDepth,
                    GL::ColorFormat textureColorFormat,
                    GL::ColorComp inputDataColorComp,
                    GL::DataType inputDataType,
                    const void *data)
{
    // typedef void(GLAPIENTRY * PFNGLTEXIMAGE3DPROC)(GLenum target,
    //                                                GLint level,
    //                                                GLint internalFormat,
    //                                                GLsizei width,
    //                                                GLsizei height,
    //                                                GLsizei depth,
    //                                                GLint border,
    //                                                GLenum format,
    //                                                GLenum type,
    //                                                const void *pixels);
    GL_CALL(glTexImage3D(GLCAST(textureTarget),
                         0,
                         GLCAST(textureColorFormat),
                         textureWidth,
                         textureHeight,
                         textureDepth,
                         0,
                         GLCAST(inputDataColorComp),
                         GLCAST(inputDataType),
                         data));
}

void GL::TexSubImage3D(GL::TextureTarget textureTarget,
                       uint offsetX,
                       uint offsetY,
                       uint offsetZ,
                       uint textureWidth,
                       uint textureHeight,
                       uint textureDepth,
                       GL::ColorComp inputDataColorComp,
                       GL::DataType inputDataType,
                       const void *data)
{
    GL_CALL(glTexSubImage3D(GLCAST(textureTarget),
                            0,
                            offsetX,
                            offsetY,
                            offsetZ,
                            textureWidth,
                            textureHeight,
                            textureDepth,
                            GLCAST(inputDataColorComp),
                            GLCAST(inputDataType),
                            data));
}

void GL::TexParameteri(GL::TextureTarget textureTarget,
                       GL::TexParameter textureParameter,
                       GLint value)
{
    GL_CALL(glTexParameteri(
                GLCAST(textureTarget), GLCAST(textureParameter), value););
}

void GL::TexParameterf(GL::TextureTarget textureTarget,
                       GL::TexParameter textureParameter,
                       GLfloat value)
{
    GL_CALL(glTexParameterf(
                GLCAST(textureTarget), GLCAST(textureParameter), value););
}

void GL::TexParameterFilter(GL::TextureTarget textureTarget,
                            GL::FilterMagMin filterMagMin,
                            GL::FilterMode filterMode)
{
    GL_CALL(GL::TexParameteri(textureTarget,
                              SCAST<GL::TexParameter>(filterMagMin),
                              GLCAST(filterMode)));
}

void GL::TexParameterWrap(GL::TextureTarget textureTarget,
                          GL::WrapCoord wrapCoord,
                          GL::WrapMode wrapMode)
{
    GL_CALL(GL::TexParameteri(
        textureTarget, SCAST<GL::TexParameter>(wrapCoord), GLCAST(wrapMode)));
}

void GL::GetTexImage(GL::TextureTarget textureTarget,
                     GL::ColorComp colorComp,
                     Byte *pixels)
{
    GL::GetTexImage(
        textureTarget, colorComp, GL::DataType::UNSIGNED_BYTE, pixels);
}

void GL::GetTexImage(GL::TextureTarget textureTarget,
                     GL::ColorComp colorComp,
                     float *pixels)
{
    GL::GetTexImage(textureTarget, colorComp, GL::DataType::FLOAT, pixels);
}

void GL::GetTexImage(GL::TextureTarget textureTarget,
                     GL::ColorComp colorComp,
                     GL::DataType dataType,
                     void *pixels)
{
    GL_CALL(glGetTexImage(GLCAST(textureTarget),
                          0,
                          GLCAST(colorComp),
                          GLCAST(dataType),
                          SCAST<void *>(pixels)));
}

bool GL::GetBoolean(GL::Enum glEnum)
{
    bool result;
    GL::GetBoolean(glEnum, &result);
    return result;
}
void GL::GetBoolean(GL::Enum glEnum, bool *values)
{
    GLboolean result;
    GL_CALL(glGetBooleanv(GLCAST(glEnum), &result));
    *values = result;
}

void GL::BindUniformBlock(GLId programId,
                          const String &uniformBlockName,
                          GLuint bindingPoint)
{
    GLuint ubLocation = GL::GetUniformBlockIndex(programId, uniformBlockName);
    if (ubLocation != SCAST<GLuint>(-1))
    {
        GL::UniformBlockBinding(programId, ubLocation, bindingPoint);
    }
}

void GL::UniformBlockBinding(GLId programId,
                             GLuint uniformBlockLocation,
                             GLuint bindingPoint)
{
    GL_CALL(
        glUniformBlockBinding(programId, uniformBlockLocation, bindingPoint));
}

GLuint GL::GetUniformBlockIndex(GLId programId, const String &uniformBlockName)
{
    GL_CALL(GLuint index = glGetUniformBlockIndex(
                programId, uniformBlockName.ToCString()));
    return index;
}

int GL::GetInteger(GL::Enum glEnum)
{
    GLint result;
    GL::GetInteger(glEnum, &result);
    return result;
}
void GL::GetInteger(GL::Enum glEnum, int *values)
{
    GL_CALL(glGetIntegerv(glEnum, values));
}

void GL::ActiveTexture(int activeTexture)
{
    ASSERT(activeTexture >= GL_TEXTURE0);
    GL_CALL(glActiveTexture(activeTexture));
}

void GL::LineWidth(float lineWidth)
{
    if (GL::GetLineWidth() != lineWidth)
    {
        SetGLContextValue(&GL::m_lineWidths, lineWidth);
        GL_CALL(glLineWidth(lineWidth));
    }
}

void GL::PointSize(float pointSize)
{
    GL_CALL(glPointSize(pointSize));
}

void GL::GenFramebuffers(int n, GLId *glIds)
{
    GL_CALL(glGenFramebuffers(n, glIds));
}

void GL::GenRenderBuffers(int n, GLId *glIds)
{
    GL_CALL(glGenRenderbuffers(n, glIds));
}

void GL::GenTextures(int n, GLId *glIds)
{
    GL_CALL(glGenTextures(n, glIds));
}

void GL::GenVertexArrays(int n, GLId *glIds)
{
    GL_CALL(glGenVertexArrays(n, glIds));
}

void GL::GenBuffers(int n, GLId *glIds)
{
    GL_CALL(glGenBuffers(n, glIds));
}

void GL::DeleteFramebuffers(int n, const GLId *glIds)
{
    GL::OnDeletedGLObjects(GL::BindTarget::FRAMEBUFFER, n, glIds);
    GL_CALL(glDeleteFramebuffers(n, glIds));
}

void GL::DeleteRenderBuffers(int n, const GLId *glIds)
{
    GL_CALL(glDeleteRenderbuffers(n, glIds));
}

void GL::DeleteTextures(int n, const GLId *glIds)
{
    GL::OnDeletedGLObjects(GL::BindTarget::TEXTURE_1D, n, glIds);
    GL::OnDeletedGLObjects(GL::BindTarget::TEXTURE_2D, n, glIds);
    GL::OnDeletedGLObjects(GL::BindTarget::TEXTURE_3D, n, glIds);
    GL::OnDeletedGLObjects(GL::BindTarget::TEXTURE_CUBE_MAP, n, glIds);
    GL_CALL(glDeleteTextures(n, glIds));
}

void GL::DeleteVertexArrays(int n, const GLId *glIds)
{
    GL::OnDeletedGLObjects(GL::BindTarget::VAO, n, glIds);
    GL_CALL(glDeleteVertexArrays(n, glIds));
}

void GL::DeleteBuffers(int n, const GLId *glIds)
{
    GL_CALL(glDeleteBuffers(n, glIds));
}

void GL::OnDeletedGLObjects(GL::BindTarget bindTarget, int n, const GLId *glIds)
{
    // Unbind objects if they were bound
    for (int i = 0; i < n; ++i)
    {
        if (GL::GetBoundId(bindTarget) == glIds[i])
        {
            GL::UnBind(bindTarget);
        }
    }
}

void GL::SetViewport(const AARect &viewportNDC)
{
    if (Window *window = Window::GetActive())
    {
        Vector2 minPx((viewportNDC.GetMin() * 0.5f + 0.5f) *
                      Vector2(window->GetSize()));
        Vector2 maxPx((viewportNDC.GetMax() * 0.5f + 0.5f) *
                      Vector2(window->GetSize()));
        GL::SetViewport(AARecti(minPx.x, minPx.y, maxPx.x, maxPx.y));
    }
}

void GL::SetViewport(const AARecti &viewport)
{
    GL::SetViewport(viewport.GetMin().x,
                    viewport.GetMin().y,
                    viewport.GetWidth(),
                    viewport.GetHeight());
}

void GL::SetViewport(int x, int y, int width, int height)
{
    AARecti vpRect(Vector2i(x, y), Vector2i(x + width, y + height));
    if (GL::GetViewportRect() != vpRect)
    {
        SetGLContextValue(&GL::m_viewportRects, vpRect);
        GL_CALL(glViewport(x, y, width, height));

        GLUniforms::OnViewportChanged(GL::GetViewportRect());
    }
}

AARecti GL::GetViewportRect()
{
    return GetGLContextValue(&GL::m_viewportRects);
}

Vector2i GL::GetViewportSize()
{
    return GL::GetViewportRect().GetSize();
}

float GL::GetViewportAspectRatio()
{
    Vector2i vpSize = GL::GetViewportSize();
    return SCAST<float>(vpSize.x) / Math::Max(vpSize.y, 1);
}

Vector2 GL::GetViewportPixelSize()
{
    return 1.0f / Vector2(GL::GetViewportSize());
}

const Array<GL::Attachment> &GL::GetDrawBuffers()
{
    return GetGLContextValue(&GL::m_drawBuffers);
}

const GL::Attachment &GL::GetReadBuffer()
{
    return GetGLContextValue(&GL::m_readBuffers);
}

const Color &GL::GetBlendColor()
{
    return GetGLContextValue(&GL::m_blendColors);
}

GL::BlendFactor GL::GetBlendSrcFactorColor()
{
    return GetGLContextValue(&GL::m_blendSrcFactorColors);
}

GL::BlendFactor GL::GetBlendDstFactorColor()
{
    return GetGLContextValue(&GL::m_blendDstFactorColors);
}

GL::BlendFactor GL::GetBlendSrcFactorAlpha()
{
    return GetGLContextValue(&GL::m_blendSrcFactorAlphas);
}

GL::BlendFactor GL::GetBlendDstFactorAlpha()
{
    return GetGLContextValue(&GL::m_blendDstFactorAlphas);
}

GL::BlendEquationE GL::GetBlendEquationColor()
{
    return GetGLContextValue(&GL::m_blendEquationColors);
}

GL::BlendEquationE GL::GetBlendEquationAlpha()
{
    return GetGLContextValue(&GL::m_blendEquationAlphas);
}

const AARecti &GL::GetScissorRect()
{
    GL *gl = GL::GetInstance();
    if (gl &&
        (!GL::IsEnabled(GL::Enablable::SCISSOR_TEST) ||
         GetGLContextValue(&GL::m_scissorRectsPx) == AARecti(-1, -1, -1, -1)))
    {
        SetGLContextValue(&GL::m_scissorRectsPx, GL::GetViewportRect());
    }
    return GetGLContextValue(&GL::m_scissorRectsPx);
}

void GL::BindBuffer(GL::BindTarget target, GLId bufferId)
{
    GL_CALL(glBindBuffer(GLCAST(target), bufferId));
}

void GL::BufferData(GL::BindTarget target,
                    GLuint dataSize,
                    const void *data,
                    GL::UsageHint usageHint)
{
    GL_CALL(glBufferData(GLCAST(target), dataSize, data, GLCAST(usageHint)));
}

void GL::BufferSubData(GL::BindTarget target,
                       GLuint offset,
                       GLuint dataSize,
                       const void *data)
{
    GL_CALL(glBufferSubData(GLCAST(target), offset, dataSize, data));
}

void GL::SetColorMask(const std::array<bool, 4> &colorMask)
{
    GL::SetColorMask(colorMask[0], colorMask[1], colorMask[2], colorMask[3]);
}

void GL::Render(const VAO *vao,
                GL::Primitive renderMode,
                int elementsCount,
                int startElementIndex,
                bool validateShader)
{
#ifdef DEBUG
    if (validateShader)
    {
        GLId boundShaderProgram =
            GL::GetBoundId(GL::BindTarget::SHADER_PROGRAM);
        if (boundShaderProgram > 0)
        {
            ASSERT(GL::ValidateProgram(boundShaderProgram));
        }
    }
#endif

    if (vao->IsIndexed())
    {
        GL::DrawElements(vao, renderMode, elementsCount, startElementIndex);
    }
    else
    {
        GL::DrawArrays(vao, renderMode, elementsCount, startElementIndex);
    }
}

void GL::DrawArrays(const VAO *vao,
                    GL::Primitive primitivesMode,
                    int verticesCount,
                    int startVertexIndex)
{
    vao->Bind();
    GL_CALL(
        glDrawArrays(GLCAST(primitivesMode), startVertexIndex, verticesCount));
    vao->UnBind();
}

void GL::DrawElements(const VAO *vao,
                      GL::Primitive primitivesMode,
                      int elementsCount,
                      int startElementIndex)
{
    vao->Bind();
    GL_CALL(glDrawElements(GLCAST(primitivesMode),
                           elementsCount,
                           GLCAST(GL::DataType::UNSIGNED_INT),
                           RCAST<const void *>(startElementIndex)));
    vao->UnBind();
}

void GL::RenderInstanced(const VAO *vao,
                         GL::Primitive primitivesMode,
                         int elementsCount,
                         int instanceAmount)
{
#ifdef DEBUG
    GLId boundShaderProgram = GL::GetBoundId(GL::BindTarget::SHADER_PROGRAM);
    ASSERT(boundShaderProgram > 0);
    ASSERT(GL::ValidateProgram(boundShaderProgram));
#endif

    if (vao->IsIndexed())
    {
        GL::DrawElementsInstanced(
            vao, primitivesMode, elementsCount, instanceAmount);
    }
    else
    {
        GL::DrawArraysInstanced(
            vao, primitivesMode, elementsCount, instanceAmount, 0);
    }
}

void GL::DrawArraysInstanced(const VAO *vao,
                             GL::Primitive primitivesMode,
                             int verticesCount,
                             int instanceAmount,
                             int instanceStartIndex)
{
    vao->Bind();
    GL_CALL(glDrawArraysInstanced(GLCAST(primitivesMode),
                                  instanceStartIndex,
                                  verticesCount,
                                  instanceAmount));
    vao->UnBind();
}

void GL::DrawElementsInstanced(const VAO *vao,
                               GL::Primitive primitivesMode,
                               int elementsCount,
                               int instanceAmount)
{
    vao->Bind();
    GL_CALL(glDrawElementsInstanced(GLCAST(primitivesMode),
                                    elementsCount,
                                    GLCAST(GL::DataType::UNSIGNED_INT),
                                    RCAST<void *>(0),
                                    instanceAmount));
    vao->UnBind();
}

void GL::VertexAttribDivisor(uint location, uint divisor)
{
    GL_CALL(glVertexAttribDivisor(location, divisor));
}

uint GL::GetLineWidth()
{
    return SCAST<uint>(GetGLContextValue(&GL::m_lineWidths));
}

uint GL::GetStencilMask()
{
    return GetGLContextValue(&GL::m_stencilMasks);
}

GL::Function GL::GetStencilFunc()
{
    return GetGLContextValue(&GL::m_stencilFuncs);
}

void GL::Bind(const GLObject *bindable)
{
    GL::Bind(bindable->GetGLBindTarget(), bindable->GetGLId());
}

void GL::Bind(GL::BindTarget bindTarget, GLId glId)
{
    switch (bindTarget)
    {
        case GL::BindTarget::TEXTURE_1D:
            SetGLContextValue(&GL::m_boundTexture1DIds, glId);
            GL_CALL(glBindTexture(GLCAST(bindTarget), glId));
            break;
        case GL::BindTarget::TEXTURE_2D:
            SetGLContextValue(&GL::m_boundTexture2DIds, glId);
            GL_CALL(glBindTexture(GLCAST(bindTarget), glId));
            break;
        case GL::BindTarget::TEXTURE_3D:
            SetGLContextValue(&GL::m_boundTexture3DIds, glId);
            GL_CALL(glBindTexture(GLCAST(bindTarget), glId));
            break;
        case GL::BindTarget::TEXTURE_CUBE_MAP:
            SetGLContextValue(&GL::m_boundTextureCubeMapIds, glId);
            GL_CALL(glBindTexture(GLCAST(bindTarget), glId));
            break;
        case GL::BindTarget::SHADER_PROGRAM:
            if (GL::IsBound(bindTarget, glId))
            {
                return;
            }
            SetGLContextValue(&GL::m_boundShaderProgramIds, glId);
            GL_CALL(glUseProgram(glId));
            break;
        case GL::BindTarget::FRAMEBUFFER:
            SetGLContextValue(&GL::m_boundDrawFramebufferIds, glId);
            SetGLContextValue(&GL::m_boundReadFramebufferIds, glId);
            GL_CALL(glBindFramebuffer(GLCAST(bindTarget), glId));
            break;
        case GL::BindTarget::DRAW_FRAMEBUFFER:
            SetGLContextValue(&GL::m_boundDrawFramebufferIds, glId);
            GL_CALL(glBindFramebuffer(GLCAST(bindTarget), glId));
            break;
        case GL::BindTarget::READ_FRAMEBUFFER:
            SetGLContextValue(&GL::m_boundReadFramebufferIds, glId);
            GL_CALL(glBindFramebuffer(GLCAST(bindTarget), glId));
            break;
        case GL::BindTarget::VAO:
            if (GL::IsBound(bindTarget, glId))
            {
                return;
            }
            SetGLContextValue(&GL::m_boundVAOIds, glId);
            GL_CALL(glBindVertexArray(glId));
            break;
        case GL::BindTarget::ELEMENT_ARRAY_BUFFER:
            SetGLContextValue(&GL::m_boundVBOElementsBufferIds, glId);
            GL_CALL(GL::BindBuffer(bindTarget, glId));
            break;
        case GL::BindTarget::ARRAY_BUFFER:
            SetGLContextValue(&GL::m_boundVBOArrayBufferIds, glId);
            GL_CALL(GL::BindBuffer(bindTarget, glId));
            break;
        case GL::BindTarget::UNIFORM_BUFFER:
            SetGLContextValue(&GL::m_boundUniformBufferIds, glId);
            GL_CALL(GL::BindBuffer(bindTarget, glId));
            break;

        default: ASSERT(false); break;
    }
}

void GL::UnBind(const GLObject *bindable)
{
    GL::UnBind(bindable->GetGLBindTarget());
}

void GL::UnBind(GL::BindTarget bindTarget)
{
    GL::Bind(bindTarget, 0);
}

bool GL::IsBound(const GLObject *bindable)
{
    return bindable
               ? GL::IsBound(bindable->GetGLBindTarget(), bindable->GetGLId())
               : false;
}

void GL::SetColorMask(bool maskR, bool maskG, bool maskB, bool maskA)
{
    GL *gl = GL::GetInstance();
    std::array<bool, 4> newColorMask = {{maskR, maskG, maskB, maskA}};
    if (!gl || (GL::GetColorMask() != newColorMask))
    {
        SetGLContextValue(&GL::m_colorMasks, newColorMask);
        GL_CALL(glColorMask(maskR, maskG, maskB, maskA));
    }
}

void GL::SetViewProjMode(GL::ViewProjMode mode)
{
    GL::GetInstance()->GetGLUniforms()->SetViewProjMode(mode);
}

void GL::SetStencilOp(GL::StencilOperation fail,
                      GL::StencilOperation zFail,
                      GL::StencilOperation zPass)
{
    SetGLContextValue(&GL::m_stencilOps, zPass);
    GL_CALL(glStencilOp(GLCAST(fail), GLCAST(zFail), GLCAST(zPass)));
}

void GL::SetStencilFunc(GL::Function stencilFunction, uint mask)
{
    GL::SetStencilFunc(stencilFunction, GL::GetStencilValue(), mask);
}

void GL::SetStencilFunc(GL::Function stencilFunction,
                        Byte stencilValue,
                        uint mask)
{
    if (stencilFunction != GL::GetStencilFunc() ||
        stencilValue != GL::GetStencilValue() || mask != GL::GetStencilMask())
    {
        SetGLContextValue(&GL::m_stencilFuncs, stencilFunction);
        SetGLContextValue(&GL::m_stencilValues, stencilValue);
        SetGLContextValue(&GL::m_stencilMasks, mask);
        GL_CALL(glStencilFunc(GLCAST(stencilFunction), stencilValue, mask));
    }
}

void GL::SetStencilOp(GL::StencilOperation zPass)
{
    if (GL::GetStencilOp() != zPass)
    {
        GL::SetStencilOp(
            GL::StencilOperation::KEEP, GL::StencilOperation::KEEP, zPass);
    }
}

void GL::SetStencilValue(Byte value)
{
    GL::SetStencilFunc(GetStencilFunc(), value, GetStencilMask());
}

void GL::SetDepthMask(bool writeDepth)
{
    if (GL::GetInstance()->GetDepthMask() != writeDepth)
    {
        SetGLContextValue(&GL::m_depthMasks, writeDepth);
        GL_CALL(glDepthMask(writeDepth));
    }
}

void GL::SetDepthFunc(GL::Function depthFunc)
{
    if (GL::GetDepthFunc() != depthFunc)
    {
        SetGLContextValue(&GL::m_depthFuncs, depthFunc);
        GL_CALL(glDepthFunc(GLCAST(depthFunc)));
    }
}

void GL::SetCullFace(GL::Face cullFace)
{
    if (GL::GetCullFace() != cullFace)
    {
        SetGLContextValue(&GL::m_cullFaces, cullFace);
        GL_CALL(glCullFace(GLCAST(cullFace)));
    }
}

void GL::SetWireframe(bool wireframe)
{
    if (GL::IsWireframe() != wireframe)
    {
        GL::PolygonMode(GL::Face::BACK, wireframe ? GL::LINE : GL::FILL);
        GL::PolygonMode(GL::Face::FRONT, wireframe ? GL::LINE : GL::FILL);
        GL::PolygonMode(GL::Face::FRONT_AND_BACK,
                        wireframe ? GL::LINE : GL::FILL);
    }
}

Vector2 GL::FromPixelsPointToPixelPerfect(const Vector2 &winPoint)
{
    return Vector2::Floor(winPoint);
}

Vector2 GL::FromPointToPointNDC(const Vector2 &point, const Vector2 &rectSize)
{
    return (point / rectSize) * 2.0f - 1.0f;
}

Vector2 GL::FromPointNDCToPoint(const Vector2 &pointNDC,
                                const Vector2 &rectSize)
{
    return (pointNDC * 0.5f + 0.5f) * rectSize;
}

Vector2 GL::FromAmountToAmountNDC(const Vector2 &amount,
                                  const Vector2 &rectSize)
{
    return ((amount / rectSize) * 2.0f);
}

Vector2 GL::FromAmountNDCToAmount(const Vector2 &amountNDC,
                                  const Vector2 &rectSize)
{
    return (amountNDC * rectSize * 0.5f);
}

Vector2 GL::FromWindowPointToViewportPoint(const Vector2 &winPoint,
                                           const AARecti &viewport)
{
    return Vector2(winPoint - Vector2(viewport.GetMin()));
}

Vector2 GL::FromViewportPointToWindowPoint(const Vector2 &vpPoint)
{
    Vector2 winPoint = vpPoint + Vector2(GL::GetViewportRect().GetMin());
    return winPoint;
}
Vector2 GL::FromViewportPointToWindowPoint(const Vector2i &vpPoint)
{
    return GL::FromViewportPointToWindowPoint(Vector2(vpPoint));
}

Vector2 GL::FromWindowPointToViewportPoint(const Vector2 &winPoint)
{
    return GL::FromWindowPointToViewportPoint(winPoint, GL::GetViewportRect());
}
Vector2 GL::FromWindowPointToViewportPoint(const Vector2i &winPoint)
{
    return GL::FromWindowPointToViewportPoint(Vector2(winPoint));
}

RectPoints GL::FromViewportRectToViewportRectNDCPoints(const Rect &vpRect)
{
    RectPoints points = vpRect.GetPoints();
    return {{GL::FromViewportPointToViewportPointNDC(points[0]),
             GL::FromViewportPointToViewportPointNDC(points[1]),
             GL::FromViewportPointToViewportPointNDC(points[2]),
             GL::FromViewportPointToViewportPointNDC(points[3])}};
}

Rect GL::FromViewportRectNDCToViewportRect(const Rect &vpRectNDC)
{
    return Rect(FromViewportPointNDCToViewportPoint(vpRectNDC.GetCenter()),
                vpRectNDC.GetAxis(0),
                FromViewportAmountNDCToViewportAmount(vpRectNDC.GetHalfSize()));
}

Rect GL::FromWindowRectToWindowRectNDC(const Rect &winRect)
{
    return Rect(FromWindowPointToWindowPointNDC(winRect.GetCenter()),
                winRect.GetAxis(0),
                FromWindowAmountToWindowAmountNDC(winRect.GetHalfSize()));
}

Rect GL::FromWindowRectNDCToWindowRect(const Rect &winRectNDC)
{
    return Rect(FromWindowPointNDCToWindowPoint(winRectNDC.GetCenter()),
                winRectNDC.GetAxis(0),
                FromWindowAmountNDCToWindowAmount(winRectNDC.GetHalfSize()));
}

AARect GL::FromViewportRectToViewportRectNDC(const AARect &vpRect)
{
    Vector2 min = GL::FromViewportPointToViewportPointNDC(vpRect.GetMin());
    Vector2 max = GL::FromViewportPointToViewportPointNDC(vpRect.GetMax());
    return AARect(min, max);
}

AARect GL::FromViewportRectNDCToViewportRect(const AARect &vpRectNDC)
{
    Vector2 min = GL::FromViewportPointNDCToViewportPoint(vpRectNDC.GetMin());
    Vector2 max = GL::FromViewportPointNDCToViewportPoint(vpRectNDC.GetMax());
    return AARect(min, max);
}

AARect GL::FromWindowRectToWindowRectNDC(const AARect &winRect)
{
    Vector2 min = GL::FromWindowPointToWindowPointNDC(winRect.GetMin());
    Vector2 max = GL::FromWindowPointToWindowPointNDC(winRect.GetMax());
    return AARect(min, max);
}

AARect GL::FromWindowRectNDCToWindowRect(const AARect &winRectNDC)
{
    Vector2 min = GL::FromWindowPointNDCToWindowPoint(winRectNDC.GetMin());
    Vector2 max = GL::FromWindowPointNDCToWindowPoint(winRectNDC.GetMax());
    return AARect(min, max);
}

Vector2 GL::FromWindowAmountToWindowAmountNDC(const Vector2 &winAmount)
{
    return (winAmount / Vector2(Window::GetActive()->GetSize())) * 2.0f;
}

Vector2 GL::FromWindowAmountNDCToWindowAmount(const Vector2 &winAmountNDC)
{
    return ((winAmountNDC * Vector2(Window::GetActive()->GetSize())) * 0.5f);
}

Vector2 GL::FromViewportAmountToViewportAmountNDC(const Vector2 &vpAmount)
{
    Vector2 vpSize(Vector2i::Max(GL::GetViewportSize(), Vector2i::One()));
    return (vpAmount / vpSize) * 2.0f;
}

Vector2 GL::FromViewportAmountNDCToViewportAmount(const Vector2 &vpAmountNDC)
{
    return ((vpAmountNDC * Vector2(GL::GetViewportSize())) * 0.5f);
}

Vector2 GL::FromWindowPointNDCToWindowPoint(const Vector2 &winPointNDC)
{
    Vector2i winSize = Window::GetActive()->GetSize();
    return GL::FromPointNDCToPoint(winPointNDC, Vector2(winSize));
}

Vector2 GL::FromWindowPointToWindowPointNDC(const Vector2 &winPoint)
{
    Vector2i winSize(Window::GetActive()->GetSize());
    return GL::FromPointToPointNDC(winPoint, Vector2(winSize));
}
Vector2 GL::FromWindowPointToWindowPointNDC(const Vector2i &winPoint)
{
    return GL::FromWindowPointToWindowPointNDC(Vector2(winPoint));
}

Vector2 GL::FromViewportPointToViewportPointNDC(const Vector2 &vpPoint)
{
    Vector2i vpSize = Vector2i::Max(Vector2i::One(), GL::GetViewportSize());
    Vector2 res = GL::FromPointToPointNDC(vpPoint, Vector2(vpSize));
    return res;
}

Vector2 GL::FromViewportPointToViewportPointNDC(const Vector2i &vpPoint)
{
    return GL::FromViewportPointToViewportPointNDC(Vector2(vpPoint));
}

Vector2 GL::FromViewportPointNDCToViewportPoint(const Vector2 &vpPointNDC)
{
    Vector2 vpSize(GL::GetViewportSize());
    return GL::FromPointNDCToPoint(vpPointNDC, vpSize);
}

GL::StencilOperation GL::GetStencilOp()
{
    return GetGLContextValue(&GL::m_stencilOps);
}
Byte GL::GetStencilValue()
{
    return GetGLContextValue(&GL::m_stencilValues);
}

std::array<bool, 4> GL::GetColorMask()
{
    return GetGLContextValue(&GL::m_colorMasks);
}
bool GL::IsColorMaskR()
{
    return GL::GetColorMask()[0];
}
bool GL::IsColorMaskG()
{
    return GL::GetColorMask()[1];
}
bool GL::IsColorMaskB()
{
    return GL::GetColorMask()[2];
}
bool GL::IsColorMaskA()
{
    return GL::GetColorMask()[3];
}

bool GL::GetDepthMask()
{
    return GetGLContextValue(&GL::m_depthMasks);
}
GL::Function GL::GetDepthFunc()
{
    return GetGLContextValue(&GL::m_depthFuncs);
}

bool GL::IsWireframe()
{
    return GL::GetPolygonMode(GL::Face::FRONT_AND_BACK) == GL_LINE;
}
GL::Face GL::GetCullFace()
{
    return GetGLContextValue(&GL::m_cullFaces);
}

SDL_GLContext GL::GetSharedGLContext()
{
    return GL::GetInstance()->m_sharedGLContext;
}

GLId GL::GetBoundId(GL::BindTarget bindTarget)
{
    switch (bindTarget)
    {
        case GL::BindTarget::TEXTURE_1D:
            return GetGLContextValue(&GL::m_boundTexture1DIds);
        case GL::BindTarget::TEXTURE_2D:
            return GetGLContextValue(&GL::m_boundTexture2DIds);
        case GL::BindTarget::TEXTURE_3D:
            return GetGLContextValue(&GL::m_boundTexture3DIds);
        case GL::BindTarget::TEXTURE_CUBE_MAP:
            return GetGLContextValue(&GL::m_boundTextureCubeMapIds);
        case GL::BindTarget::FRAMEBUFFER:
            return (GL::GetBoundId(GL::BindTarget::DRAW_FRAMEBUFFER) ==
                    GL::GetBoundId(GL::BindTarget::READ_FRAMEBUFFER))
                       ? GL::GetBoundId(GL::BindTarget::DRAW_FRAMEBUFFER)
                       : 0;
        case GL::BindTarget::DRAW_FRAMEBUFFER:
            return GetGLContextValue(&GL::m_boundDrawFramebufferIds);
        case GL::BindTarget::READ_FRAMEBUFFER:
            return GetGLContextValue(&GL::m_boundReadFramebufferIds);
        case GL::BindTarget::VAO: return GetGLContextValue(&GL::m_boundVAOIds);
        case GL::BindTarget::ARRAY_BUFFER:
            return GetGLContextValue(&GL::m_boundVBOArrayBufferIds);
        case GL::BindTarget::ELEMENT_ARRAY_BUFFER:
            return GetGLContextValue(&GL::m_boundVBOElementsBufferIds);
        case GL::BindTarget::SHADER_PROGRAM:
            return GetGLContextValue(&GL::m_boundShaderProgramIds);
        default: ASSERT(false);
    }
    ASSERT(false);
    return -1;
}

bool GL::IsBound(GL::BindTarget bindTarget, GLId glId)
{
    return GL::GetBoundId(bindTarget) == glId;
}

uint GL::GetPixelBytesSize(GL::ColorFormat texFormat)
{
    switch (texFormat)
    {
        case GL::ColorFormat::R8: return 1;
        case GL::ColorFormat::SRGB: return 3;
        case GL::ColorFormat::SRGBA: return 4;
        case GL::ColorFormat::RGBA8: return 4;
        case GL::ColorFormat::RGBA16F: return 8;
        case GL::ColorFormat::DEPTH: return 4;
        case GL::ColorFormat::DEPTH16: return 2;
        case GL::ColorFormat::DEPTH24: return 3;
        case GL::ColorFormat::DEPTH32: return 4;
        case GL::ColorFormat::DEPTH32F: return 16;
        case GL::ColorFormat::DEPTH24_STENCIL8: return 4;
        case GL::ColorFormat::RGB10_A2: return 4;
        case GL::ColorFormat::RGBA32F: return 16;
    }
    return 0;
}

uint GL::GetPixelBytesSize(GL::ColorComp colorComp, GL::DataType dataType)
{
    return GL::GetNumComponents(colorComp) * GL::GetBytesSize(dataType);
}

uint GL::GetBytesSize(GL::DataType dataType)
{
    switch (dataType)
    {
        case GL::DataType::BYTE:
        case GL::DataType::UNSIGNED_BYTE: return sizeof(Byte);

        case GL::DataType::SHORT:
        case GL::DataType::UNSIGNED_SHORT: return sizeof(short);

        case GL::DataType::INT:
        case GL::DataType::UNSIGNED_INT: return sizeof(int);

        case GL::DataType::FLOAT: return sizeof(float);

        case GL::DataType::DOUBLE: return sizeof(double);

        default: return -1;
    }
    ASSERT(false);
    return 0;
}

uint GL::GetNumComponents(GL::ColorComp colorComp)
{
    switch (colorComp)
    {
        case GL::ColorComp::RED:
        case GL::ColorComp::DEPTH:
        case GL::ColorComp::DEPTH_STENCIL: return 1;

        case GL::ColorComp::RGB: return 3;
        case GL::ColorComp::RGBA: return 4;

        default: break;
    }
    ASSERT(false);
    return 0;
}

uint GL::GetNumComponents(GL::ColorFormat colorFormat)
{
    return GL::GetNumComponents(GL::GetColorCompFrom(colorFormat));
}

GL::DataType GL::GetDataTypeFrom(GL::ColorFormat format)
{
    switch (format)
    {
        case GL::ColorFormat::R8:
        case GL::ColorFormat::SRGB:
        case GL::ColorFormat::SRGBA:
        case GL::ColorFormat::RGBA8:
        case GL::ColorFormat::RGB10_A2: return GL::DataType::UNSIGNED_BYTE;

        case GL::ColorFormat::RGBA16F:
        case GL::ColorFormat::RGBA32F:
        case GL::ColorFormat::DEPTH24_STENCIL8:
        case GL::ColorFormat::DEPTH:
        case GL::ColorFormat::DEPTH16:
        case GL::ColorFormat::DEPTH24:
        case GL::ColorFormat::DEPTH32:
        case GL::ColorFormat::DEPTH32F: return GL::DataType::FLOAT;

        default: break;
    }
    ASSERT(false);
    return GL::DataType::FLOAT;
}

GL::ColorComp GL::GetColorCompFrom(GL::ColorFormat format)
{
    switch (format)
    {
        case GL::ColorFormat::R8: return GL::ColorComp::RED;

        case GL::ColorFormat::SRGBA:
        case GL::ColorFormat::RGBA8:
        case GL::ColorFormat::RGBA16F:
        case GL::ColorFormat::RGBA32F:
        case GL::ColorFormat::RGB10_A2: return GL::ColorComp::RGBA;

        case GL::ColorFormat::DEPTH:
        case GL::ColorFormat::DEPTH16:
        case GL::ColorFormat::DEPTH24:
        case GL::ColorFormat::DEPTH32:
        case GL::ColorFormat::DEPTH32F:
        case GL::ColorFormat::DEPTH24_STENCIL8: return GL::ColorComp::DEPTH;

        default: break;
    }
    ASSERT(false);
    return GL::ColorComp::RGB;
}

bool GL::IsDepthFormat(GL::ColorFormat format)
{
    return (format == GL::ColorFormat::DEPTH ||
            format == GL::ColorFormat::DEPTH16 ||
            format == GL::ColorFormat::DEPTH24 ||
            format == GL::ColorFormat::DEPTH32 ||
            format == GL::ColorFormat::DEPTH32F ||
            format == GL::ColorFormat::DEPTH24_STENCIL8);
}

void GL::BindUniformBufferToShader(const String &uniformBlockName,
                                   const ShaderProgram *sp,
                                   const IUniformBuffer *buffer)
{
    GL_CALL(GLuint blockIndex = glGetUniformBlockIndex(
                sp->GetGLId(), uniformBlockName.ToCString()));
    GL_CALL(glUniformBlockBinding(
        sp->GetGLId(), blockIndex, buffer->GetBindingPoint()));
}

template <class T>
void Push_(StackAndValue<T> *stackAndValue)
{
    stackAndValue->stack.push(stackAndValue->currentValue);
}
template <class T>
void Pop_(StackAndValue<T> *stackAndValue)
{
    ASSERT(stackAndValue->stack.size() >= 1);
    ASSERT(stackAndValue->stack.top() == stackAndValue->currentValue);
    stackAndValue->stack.pop();
}
template <class T>
void PushOrPop_(StackAndValue<T> *stackAndValue, bool push)
{
    if (push)
    {
        Push_(stackAndValue);
    }
    else
    {
        Pop_(stackAndValue);
    }
}
template <class T>
void Push_(StackAndValue<T> GL::*stackAndValueMemberPtr)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);
    Push_(&(gl->*stackAndValueMemberPtr));
}
template <class T>
void Pop_(StackAndValue<T> GL::*stackAndValueMemberPtr)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);
    Pop_(&(gl->*stackAndValueMemberPtr));
}
template <class T>
void PushOrPop_(StackAndValue<T> GL::*stackAndValueMemberPtr, bool push)
{
    if (push)
    {
        Push_(stackAndValueMemberPtr);
    }
    else
    {
        Pop_(stackAndValueMemberPtr);
    }
}

void GL::Push(GL::Pushable pushable)
{
    PushOrPop(pushable, true);
}

void GL::Push(GL::Enablable enablable)
{
    PushOrPop(enablable, true);
}

void GL::Push(GL::BindTarget bindTarget)
{
    PushOrPop(bindTarget, true);
}

void GL::Pop(GL::Pushable pushable)
{
    PushOrPop(pushable, false);
}

void GL::Pop(GL::Enablable enablable)
{
    PushOrPop(enablable, false);
}

void GL::Pop(GL::BindTarget bindTarget)
{
    PushOrPop(bindTarget, false);
}

void GL::PushOrPop(GL::Pushable pushable, bool push)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);
    switch (pushable)
    {
        case GL::Pushable::BLEND_STATES:
            if (!push)
            {
                ASSERT(gl->m_blendColors.stack.size() >= 1);
                ASSERT(gl->m_blendSrcFactorColors.stack.size() >= 1);
                ASSERT(gl->m_blendDstFactorColors.stack.size() >= 1);
                ASSERT(gl->m_blendSrcFactorAlphas.stack.size() >= 1);
                ASSERT(gl->m_blendDstFactorAlphas.stack.size() >= 1);
                ASSERT(gl->m_blendEquationColors.stack.size() >= 1);
                ASSERT(gl->m_blendEquationAlphas.stack.size() >= 1);
                GL::BlendColor(gl->m_blendColors.stack.top());
                GL::BlendFuncSeparate(gl->m_blendSrcFactorColors.stack.top(),
                                      gl->m_blendDstFactorColors.stack.top(),
                                      gl->m_blendSrcFactorAlphas.stack.top(),
                                      gl->m_blendDstFactorAlphas.stack.top());
                GL::BlendEquationSeparate(
                    gl->m_blendEquationColors.stack.top(),
                    gl->m_blendEquationAlphas.stack.top());
            }

            PushOrPop(GL::Enablable::BLEND, push);
            PushOrPop_(&gl->m_blendColors, push);
            PushOrPop_(&gl->m_blendEquationAlphas, push);
            PushOrPop_(&gl->m_blendEquationColors, push);
            PushOrPop_(&gl->m_blendDstFactorAlphas, push);
            PushOrPop_(&gl->m_blendDstFactorColors, push);
            PushOrPop_(&gl->m_blendSrcFactorAlphas, push);
            PushOrPop_(&gl->m_blendSrcFactorColors, push);

            break;

        case GL::Pushable::COLOR_MASK:
            if (!push)
            {
                GL::SetColorMask(gl->m_colorMasks.stack.top());
            }
            PushOrPop_(&gl->m_colorMasks, push);
            break;

        case GL::Pushable::CULL_FACE:
            if (!push)
            {
                GL::SetCullFace(gl->m_cullFaces.stack.top());
            }
            PushOrPop(GL::Enablable::CULL_FACE, push);
            PushOrPop_(&gl->m_cullFaces, push);
            break;

        case GL::Pushable::DEPTH_STATES:
            PushOrPop(GL::Enablable::DEPTH_TEST, push);
            PushOrPop(GL::Enablable::DEPTH_CLAMP, push);
            if (!push)
            {
                GL::SetDepthFunc(gl->m_depthFuncs.stack.top());
                GL::SetDepthMask(gl->m_depthMasks.stack.top());
            }
            PushOrPop_(&gl->m_depthFuncs, push);
            PushOrPop_(&gl->m_depthMasks, push);
            break;

        case GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS:
            if (!push)
            {
                PushOrPop(GL::BindTarget::FRAMEBUFFER, push);
                if (GL::GetBoundId(GL::BindTarget::FRAMEBUFFER) > 0)
                {
                    GL::DrawBuffers(gl->m_drawBuffers.stack.top());
                    GL::ReadBuffer(gl->m_readBuffers.stack.top());
                }
                else
                {
                    // Simply update to mantain coherency (and so assert in
                    // pop does not bother)
                    gl->m_drawBuffers.currentValue =
                        gl->m_drawBuffers.stack.top();
                    gl->m_readBuffers.currentValue =
                        gl->m_readBuffers.stack.top();
                }
            }

            PushOrPop_(&gl->m_drawBuffers, push);
            PushOrPop_(&gl->m_readBuffers, push);

            if (push)
            {
                PushOrPop(GL::BindTarget::FRAMEBUFFER, push);
            }
            break;

        case GL::Pushable::SHADER_PROGRAM:
            PushOrPop(GL::BindTarget::SHADER_PROGRAM, push);
            break;

        case GL::Pushable::STENCIL_STATES:
            PushOrPop(GL::Enablable::STENCIL_TEST, push);
            if (!push)
            {
                GL::SetStencilFunc(gl->m_stencilFuncs.stack.top(),
                                   gl->m_stencilMasks.stack.top());
                GL::SetStencilOp(gl->m_stencilOps.stack.top());
                GL::SetStencilValue(gl->m_stencilValues.stack.top());
            }
            PushOrPop_(&gl->m_stencilFuncs, push);
            PushOrPop_(&gl->m_stencilMasks, push);
            PushOrPop_(&gl->m_stencilOps, push);
            PushOrPop_(&gl->m_stencilValues, push);
            break;

        case GL::Pushable::VAO: GL::PushOrPop(GL::BindTarget::VAO, push); break;

        case GL::Pushable::VBO:
            GL::PushOrPop(GL::BindTarget::ARRAY_BUFFER, push);
            break;

        case GL::Pushable::VIEWPORT:
            if (!push)
            {
                GL::SetViewport(gl->m_viewportRects.stack.top());
            }
            PushOrPop_(&gl->m_viewportRects, push);
            break;

        case GL::Pushable::MODEL_MATRIX:
            if (push)
            {
                gl->m_modelMatrices.push(
                    GLUniforms::GetActive()->GetModelMatrix());
            }
            else
            {
                ASSERT(gl->m_modelMatrices.size() >= 1);
                GLUniforms::GetActive()->SetModelMatrix(
                    gl->m_modelMatrices.top());
                gl->m_modelMatrices.pop();
            }
            break;

        case GL::Pushable::VIEW_MATRIX:
            if (push)
            {
                gl->m_viewMatrices.push(
                    GLUniforms::GetActive()->GetViewMatrix());
            }
            else
            {
                ASSERT(gl->m_viewMatrices.size() >= 1);
                GLUniforms::GetActive()->SetViewMatrix(
                    gl->m_viewMatrices.top());
                gl->m_viewMatrices.pop();
            }
            break;

        case GL::Pushable::PROJECTION_MATRIX:
            if (push)
            {
                gl->m_projectionMatrices.push(
                    GLUniforms::GetActive()->GetProjectionMatrix(
                        GL::ViewProjMode::WORLD));
            }
            else
            {
                ASSERT(gl->m_projectionMatrices.size() >= 1);
                GLUniforms::GetActive()->SetProjectionMatrix(
                    gl->m_projectionMatrices.top());
                gl->m_projectionMatrices.pop();
            }
            break;

        case GL::Pushable::ALL_MATRICES:
            PushOrPop(GL::Pushable::MODEL_MATRIX, push);
            PushOrPop(GL::Pushable::VIEW_MATRIX, push);
            PushOrPop(GL::Pushable::PROJECTION_MATRIX, push);
            break;

        case GL::Pushable::VIEWPROJ_MODE:
            if (push)
            {
                gl->m_viewProjModes.push(
                    GLUniforms::GetActive()->GetViewProjMode());
            }
            else
            {
                ASSERT(gl->m_viewProjModes.size() >= 1);
                GLUniforms::GetActive()->SetViewProjMode(
                    gl->m_viewProjModes.top());
                gl->m_viewProjModes.pop();
            }
            break;

        default: ASSERT(false); break;
    }
}

void GL::PushOrPop(GL::Enablable enablable, bool push)
{
    GL *gl = GL::GetInstance();
    ASSERT(gl);

    if (!push)
    {
        ASSERT(gl->m_enabledVars.ContainsKey(enablable));
        ASSERT(gl->m_enabledVars.Get(enablable).stack.size() >= 1);
        GL::SetEnabled(enablable,
                       gl->m_enabledVars.Get(enablable).stack.top()[0]);

        auto &enabledIStackAndValue = gl->m_enabledVars.Get(enablable);
        if (GL::CanEnablableBeIndexed(enablable))
        {
            for (int i = 0; i < GL::GetEnablableIndexMax(enablable); ++i)
            {
                GL::SetEnabledi(
                    enablable, i, enabledIStackAndValue.stack.top()[i]);
            }
        }
        else
        {
            GL::SetEnabled(enablable, enabledIStackAndValue.stack.top()[0]);
        }
    }
    PushOrPop_(&gl->m_enabledVars.Get(enablable), push);
}

void GL::PushOrPop(GL::BindTarget bindTarget, bool push)
{
    GL *gl = GL::GetInstance();
    switch (bindTarget)
    {
        case GL::BindTarget::NONE:
        case GL::BindTarget::TEXTURE_1D:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundTexture1DIds.stack.top());
            }
            PushOrPop_(&gl->m_boundTexture1DIds, push);
            break;

        case GL::BindTarget::TEXTURE_2D:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundTexture2DIds.stack.top());
            }
            PushOrPop_(&gl->m_boundTexture2DIds, push);
            break;

        case GL::BindTarget::TEXTURE_3D:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundTexture3DIds.stack.top());
            }
            PushOrPop_(&gl->m_boundTexture3DIds, push);
            break;

        case GL::BindTarget::TEXTURE_CUBE_MAP:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundTextureCubeMapIds.stack.top());
            }
            PushOrPop_(&gl->m_boundTextureCubeMapIds, push);
            break;
        case GL::BindTarget::SHADER_PROGRAM:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundShaderProgramIds.stack.top());
            }
            PushOrPop_(&gl->m_boundShaderProgramIds, push);
            break;
        case GL::BindTarget::FRAMEBUFFER:
            GL::PushOrPop(GL::BindTarget::DRAW_FRAMEBUFFER, push);
            GL::PushOrPop(GL::BindTarget::READ_FRAMEBUFFER, push);
            break;
        case GL::BindTarget::DRAW_FRAMEBUFFER:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundDrawFramebufferIds.stack.top());
            }
            PushOrPop_(&gl->m_boundDrawFramebufferIds, push);
            break;
        case GL::BindTarget::READ_FRAMEBUFFER:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundReadFramebufferIds.stack.top());
            }
            PushOrPop_(&gl->m_boundReadFramebufferIds, push);
            break;
        case GL::BindTarget::VAO:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundVAOIds.stack.top());
            }
            PushOrPop_(&gl->m_boundVAOIds, push);
            break;
        case GL::BindTarget::ARRAY_BUFFER:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundVBOArrayBufferIds.stack.top());
            }
            PushOrPop_(&gl->m_boundVBOArrayBufferIds, push);
            break;
        case GL::BindTarget::ELEMENT_ARRAY_BUFFER:
            if (!push)
            {
                GL::Bind(bindTarget,
                         gl->m_boundVBOElementsBufferIds.stack.top());
            }
            PushOrPop_(&gl->m_boundVBOElementsBufferIds, push);
            break;
        case GL::BindTarget::UNIFORM_BUFFER:
            if (!push)
            {
                GL::Bind(bindTarget, gl->m_boundUniformBufferIds.stack.top());
            }
            PushOrPop_(&gl->m_boundUniformBufferIds, push);
            break;

        default: ASSERT(false); break;
    }
}

void GL::PrintGLStats()
{
    const int maxUniformLocations =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_UNIFORM_LOCATIONS));
    const int maxUniformBufferBindings =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_UNIFORM_BUFFER_BINDINGS));
    const int maxVaryingComponents =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_VARYING_COMPONENTS));
    const int maxVertexUniformsComponents =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_VERTEX_UNIFORM_COMPONENTS));
    const int maxGeomUniformsComponents =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS));
    const int maxTessControlUniformsComponents =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS));
    const int maxTessEvalUniformsComponents = GL::GetInteger(
        SCAST<GL::Enum>(GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS));
    const int maxFragUniformsComponents =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS));
    const int maxTexUnits =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_TEXTURE_IMAGE_UNITS));
    const int maxTexSize = GL::GetInteger(SCAST<GL::Enum>(GL_MAX_TEXTURE_SIZE));
    const int maxTexSize3D =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_3D_TEXTURE_SIZE));
    const int maxFBWidth =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_FRAMEBUFFER_WIDTH));
    const int maxFBHeight =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_FRAMEBUFFER_HEIGHT));
    const int maxColorAttachments =
        GL::GetInteger(SCAST<GL::Enum>(GL_MAX_COLOR_ATTACHMENTS));

    Debug_DLog("GL Stats: ================");
    Debug_DPeek(maxUniformLocations);
    Debug_DPeek(maxUniformBufferBindings);
    Debug_DPeek(maxVaryingComponents);
    Debug_DPeek(maxVertexUniformsComponents);
    Debug_DPeek(maxGeomUniformsComponents);
    Debug_DPeek(maxTessControlUniformsComponents);
    Debug_DPeek(maxTessEvalUniformsComponents);
    Debug_DPeek(maxFragUniformsComponents);
    Debug_DPeek(maxTexUnits);
    Debug_DPeek(maxTexSize);
    Debug_DPeek(maxTexSize3D);
    Debug_DPeek(maxFBWidth);
    Debug_DPeek(maxFBHeight);
    Debug_DPeek(maxColorAttachments);
    Debug_DLog("==========================");
}

void GL::PrintGLContext()
{
    Debug_DLog("GL Context: ================");
    Debug_DPeek(GL::GetViewportRect());
    Debug_DPeek(GL::GetScissorRect());
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::VAO)));
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::ARRAY_BUFFER)));
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::FRAMEBUFFER)));
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::DRAW_FRAMEBUFFER)));
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::READ_FRAMEBUFFER)));
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::SHADER_PROGRAM)));
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::TEXTURE_1D)));
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::TEXTURE_2D)));
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::TEXTURE_3D)));
    Debug_DPeek(SCAST<int>(GL::GetBoundId(GL::BindTarget::TEXTURE_CUBE_MAP)));
    // Debug_DPeek( SCAST<int>(GL::GetBoundId(GL::BindTarget::UniformBuffer)) );
    Debug_DPeek(GL::GetColorMask());
    Debug_DPeek(GL::GetLineWidth());
    Debug_DPeek(GL::IsEnabled(GL::Enablable::ALPHA_TEST));
    Debug_DPeek(GL::IsEnabled(GL::Enablable::BLEND));
    Debug_DPeek(GL::IsEnabled(GL::Enablable::CULL_FACE));
    Debug_DPeek(GL::IsEnabled(GL::Enablable::DEPTH_TEST));
    Debug_DPeek(GL::IsEnabled(GL::Enablable::DEPTH_CLAMP));
    Debug_DPeek(GL::IsEnabled(GL::Enablable::SCISSOR_TEST));
    Debug_DPeek(GL::IsEnabled(GL::Enablable::STENCIL_TEST));
    Debug_DPeek(GL::GetDrawBuffers());
    Debug_DPeek(GL::GetReadBuffer());
    Debug_DPeek(GL::GetDepthMask());
    Debug_DPeek(GL::GetDepthFunc());
    Debug_DPeek(GL::GetClearColor());
    Debug_DPeek(GL::GetCullFace());
    Debug_DPeek(GL::GetPolygonMode(GL::Face::BACK));
    Debug_DPeek(GL::GetPolygonMode(GL::Face::FRONT));
    Debug_DPeek(GL::GetPolygonMode(GL::Face::FRONT_AND_BACK));
    Debug_DPeek(GL::GetStencilOp());
    Debug_DPeek(GL::GetStencilFunc());
    Debug_DPeek(SCAST<int>(GL::GetStencilValue()));
    Debug_DPeek(SCAST<int>(GL::GetStencilMask()));
    Debug_DPeek(GL::GetBlendColor());
    Debug_DPeek(GL::GetBlendDstFactorAlpha());
    Debug_DPeek(GL::GetBlendDstFactorColor());
    Debug_DPeek(GL::GetBlendEquationAlpha());
    Debug_DPeek(GL::GetBlendEquationColor());
    Debug_DPeek(GL::GetBlendSrcFactorAlpha());
    Debug_DPeek(GL::GetBlendSrcFactorColor());
    Debug_DPeek(GLUniforms::GetModelMatrix());
    Debug_DPeek(GLUniforms::GetViewMatrix());
    Debug_DPeek(GLUniforms::GetProjectionMatrix());
    Debug_DPeek(GLUniforms::GetActive()->GetViewProjMode());

    Debug::PrintAllUniforms();

    Debug_DLog("==========================");
}

GL::ViewProjMode GL::GetViewProjMode()
{
    return GL::GetInstance()->GetGLUniforms()->GetViewProjMode();
}

GL *GL::GetInstance()
{
    GEngine *ge = GEngine::GetInstance();
    return ge ? ge->GetGL() : nullptr;
}

GLUniforms *GL::GetGLUniforms() const
{
    return m_glUniforms;
}
