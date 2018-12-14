#ifndef GL_H
#define GL_H

#include <GL/glew.h>
#include <array>
#include <stack>

#include "Bang/AARect.h"
#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/Matrix4.h"
#include "Bang/StackAndValue.h"
#include "Bang/String.h"
#include "Bang/UMap.h"

struct SDL_Window;

namespace Bang
{
#ifdef DEBUG
#define GL_CALL(CALL) \
    GL_ClearError();  \
    CALL;             \
    GL_CheckError()
#define GL_ClearError() GL::ClearError()
#define GL_CheckError()                                                     \
    ASSERT_SOFT_MSG(                                                        \
        GL::CheckError(                                                     \
            __LINE__, String(SCAST<const char *>(__FUNCTION__)), __FILE__), \
        "There was an OpenGL error, see previous message.");
#else
#define GL_CALL(CALL) CALL
#define GL_ClearError()  // Empty
#define GL_CheckError()  // Empty
#endif

class GLObject;
class GLUniforms;
class IUniformBuffer;
class ShaderProgram;
class VAO;

class GL
{
public:
    enum Enum
    {
        ACTIVE_UNIFORMS = GL_ACTIVE_UNIFORMS,
        COMPILE_STATUS = GL_COMPILE_STATUS,
        VALIDATE_STATUS = GL_VALIDATE_STATUS,
        FILL = GL_FILL,
        INFO_LOG_LENGTH = GL_INFO_LOG_LENGTH,
        LINE = GL_LINE,
        LINK_STATUS = GL_LINK_STATUS,
        MAX_DRAW_BUFFERS = GL_MAX_DRAW_BUFFERS,
        MAX_TEXTURE_IMAGE_UNITS = GL_MAX_TEXTURE_IMAGE_UNITS,
        READ_ONLY = GL_READ_ONLY,
        READ_WRITE = GL_READ_WRITE,
        UNPACK_ALIGNMENT = GL_UNPACK_ALIGNMENT,
        VIEWPORT = GL_VIEWPORT,
        WRITE_ONLY = GL_WRITE_ONLY,
        TEXTURE_BINDING_1D = GL_TEXTURE_BINDING_1D,
        TEXTURE_BINDING_2D = GL_TEXTURE_BINDING_2D,
        TEXTURE_BINDING_3D = GL_TEXTURE_BINDING_3D,
        TEXTURE_BINDING_CUBE_MAP = GL_TEXTURE_BINDING_CUBE_MAP
    };

    enum class Primitive
    {
        POINTS = GL_POINTS,
        LINES = GL_LINES,
        LINE_STRIP = GL_LINE_STRIP,
        LINE_LOOP = GL_LINE_LOOP,
        TRIANGLES = GL_TRIANGLES,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        TRIANGLE_FAN = GL_TRIANGLE_FAN,
        QUADS = GL_QUADS,
        QUAD_STRIP = GL_QUAD_STRIP,
        POLYGON = GL_POLYGON
    };

    enum class Enablable
    {
        BLEND = GL_BLEND,
        DEPTH_TEST = GL_DEPTH_TEST,
        DEPTH_CLAMP = GL_DEPTH_CLAMP,
        STENCIL_TEST = GL_STENCIL_TEST,
        SCISSOR_TEST = GL_SCISSOR_TEST,
        ALPHA_TEST = GL_ALPHA_TEST,
        CULL_FACE = GL_CULL_FACE,
        FRAMEBUFFER_SRGB = GL_FRAMEBUFFER_SRGB,
        MULTISAMPLE = GL_MULTISAMPLE,
        TEXTURE_CUBE_MAP_SEAMLESS = GL_TEXTURE_CUBE_MAP_SEAMLESS
    };

    enum class UsageHint
    {
        STREAM_DRAW = GL_STREAM_DRAW,
        STREAM_READ = GL_STREAM_READ,
        STREAM_COPY = GL_STREAM_COPY,
        STATIC_DRAW = GL_STATIC_DRAW,
        STATIC_READ = GL_STATIC_READ,
        STATIC_COPY = GL_STATIC_COPY,
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
        DYNAMIC_READ = GL_DYNAMIC_READ,
        DYNAMIC_COPY = GL_DYNAMIC_COPY
    };

    enum class CullFaceExt
    {
        NONE = GL_NONE,
        FRONT = GL_FRONT,
        BACK = GL_BACK,
        FRONT_AND_BACK = GL_FRONT_AND_BACK
    };

    enum class Face
    {
        FRONT = GL_FRONT,
        BACK = GL_BACK,
        FRONT_AND_BACK = GL_FRONT_AND_BACK
    };

    enum class BindTarget
    {
        NONE = 0,
        TEXTURE_1D = GL_TEXTURE_1D,
        TEXTURE_2D = GL_TEXTURE_2D,
        TEXTURE_3D = GL_TEXTURE_3D,
        TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
        SHADER_PROGRAM = GL_SHADER,
        FRAMEBUFFER = GL_FRAMEBUFFER,
        DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER,
        READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER,
        VERTEX_ARRAY = GL_VERTEX_ARRAY,
        VAO = GL_VERTEX_ARRAY,
        ARRAY_BUFFER = GL_ARRAY_BUFFER,
        VBO = GL_ARRAY_BUFFER,
        ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
        UNIFORM_BUFFER = GL_UNIFORM_BUFFER
    };

    enum class DataType
    {
        BYTE = GL_BYTE,
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
        SHORT = GL_SHORT,
        UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
        INT = GL_INT,
        UNSIGNED_INT = GL_UNSIGNED_INT,
        FLOAT = GL_FLOAT,
        DOUBLE = GL_DOUBLE
    };

    enum class VertexAttribDataType
    {
        BYTE = GL_BYTE,
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
        SHORT = GL_SHORT,
        UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
        INT = GL_INT,
        UNSIGNED_INT = GL_UNSIGNED_INT,
        FLOAT = GL_FLOAT,
        DOUBLE = GL_DOUBLE,
        HALF_FLOAT = GL_HALF_FLOAT,
        FIXED = GL_FIXED,
        INT_2_10_10_10_REV = GL_INT_2_10_10_10_REV,
        UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV,
        UNSIGNED_INT_10F_11F_11F_REV = GL_UNSIGNED_INT_10F_11F_11F_REV
    };

    enum class UniformType
    {
        BYTE = GL_BYTE,
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
        SHORT = GL_SHORT,
        UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
        INT = GL_INT,
        UNSIGNED_INT = GL_UNSIGNED_INT,
        FLOAT = GL_FLOAT,
        DOUBLE = GL_DOUBLE,
        UNSIGNED_INT_24_8 = GL_UNSIGNED_INT_24_8,
        VEC2 = GL_FLOAT_VEC2,
        VEC3 = GL_FLOAT_VEC3,
        VEC4 = GL_FLOAT_VEC4,
        MAT3 = GL_FLOAT_MAT3,
        MAT4 = GL_FLOAT_MAT4,
        SAMPLER_1D = GL_SAMPLER_1D,
        SAMPLER_2D = GL_SAMPLER_2D,
        SAMPLER_3D = GL_SAMPLER_3D,
        SAMPLER_CUBE = GL_SAMPLER_CUBE,
        SAMPLER_1D_SHADOW = GL_SAMPLER_1D_SHADOW,
        SAMPLER_2D_SHADOW = GL_SAMPLER_2D_SHADOW,
        SAMPLER_CUBE_SHADOW = GL_SAMPLER_CUBE_SHADOW,
        SAMPLER_1D_ARRAY_SHADOW = GL_SAMPLER_1D_ARRAY_SHADOW,
        SAMPLER_2D_ARRAY_SHADOW = GL_SAMPLER_2D_ARRAY_SHADOW
    };

    enum class ShaderType
    {
        VERTEX = GL_VERTEX_SHADER,
        GEOMETRY = GL_GEOMETRY_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER
    };

    enum class ColorComp
    {
        RED = GL_RED,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        DEPTH = GL_DEPTH_COMPONENT,
        DEPTH_STENCIL = GL_DEPTH_STENCIL,
        STENCIL_INDEX = GL_STENCIL_INDEX
    };

    enum class ColorFormat
    {
        SRGB = GL_SRGB,
        SRGBA = GL_SRGB_ALPHA,
        RGBA8 = GL_RGBA8,
        RGBA16F = GL_RGBA16F,
        RGB10_A2 = GL_RGB10_A2,
        RGBA32F = GL_RGBA32F,
        R8 = GL_R8,
        DEPTH = GL_DEPTH_COMPONENT,
        DEPTH16 = GL_DEPTH_COMPONENT16,
        DEPTH24 = GL_DEPTH_COMPONENT24,
        DEPTH32 = GL_DEPTH_COMPONENT32,
        DEPTH32F = GL_DEPTH_COMPONENT32F,
        DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8
    };

    enum class ViewProjMode
    {
        WORLD = 0,
        CANVAS = 1
    };

    enum class TextureTarget
    {
        TEXTURE_1D = GL_TEXTURE_1D,
        TEXTURE_2D = GL_TEXTURE_2D,
        TEXTURE_3D = GL_TEXTURE_3D,
        TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
        TEXTURE_CUBE_MAP_POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        TEXTURE_CUBE_MAP_NEGATIVE_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        TEXTURE_CUBE_MAP_POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        TEXTURE_CUBE_MAP_NEGATIVE_Y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        TEXTURE_CUBE_MAP_POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        TEXTURE_CUBE_MAP_NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    enum class CubeMapDir
    {
        POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        RIGHT = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        NEGATIVE_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        LEFT = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        TOP = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        NEGATIVE_Y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        BOT = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        BACK = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        FRONT = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    static const std::array<GL::CubeMapDir, 6> &GetAllCubeMapDirs();
    static int GetCubeMapDirIndex(GL::CubeMapDir cmDir);
    static Vector3 GetCubeMapDirVector(GL::CubeMapDir cmDir);

    enum class FramebufferTarget
    {
        DRAW = GL_DRAW_FRAMEBUFFER,
        READ = GL_READ_FRAMEBUFFER,
        READ_DRAW = GL_FRAMEBUFFER
    };
    enum class RenderbufferTarget
    {
        RENDERBUFFER = GL_RENDERBUFFER
    };

    enum class RenderbufferFormat
    {
        DEPTH = GL_DEPTH,
        STENCIL = GL_STENCIL,
        DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8
    };

    enum class BufferBit
    {
        COLOR = GL_COLOR_BUFFER_BIT,
        DEPTH = GL_DEPTH_BUFFER_BIT,
        STENCIL = GL_STENCIL_BUFFER_BIT,
        SCISSOR = GL_SCISSOR_BIT
    };

    enum class WrapMode
    {
        REPEAT = GL_REPEAT,
        CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
        CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
    };
    enum class WrapCoord
    {
        WRAP_S = GL_TEXTURE_WRAP_S,
        WRAP_T = GL_TEXTURE_WRAP_T,
        WRAP_R = GL_TEXTURE_WRAP_R
    };

    enum class TexParameter
    {
        DEPTH_STENCIL_TEXTURE_MODE = GL_DEPTH_STENCIL_TEXTURE_MODE,
        TEXTURE_BORDER_COLOR = GL_TEXTURE_BORDER_COLOR,
        TEXTURE_COMPARE_FUNC = GL_TEXTURE_COMPARE_FUNC,
        TEXTURE_COMPARE_MODE = GL_TEXTURE_COMPARE_MODE,
        TEXTURE_LOD_BIAS = GL_TEXTURE_LOD_BIAS,
        TEXTURE_MIN_FILTER = GL_TEXTURE_MIN_FILTER,
        TEXTURE_MAG_FILTER = GL_TEXTURE_MAG_FILTER,
        TEXTURE_MIN_LOD = GL_TEXTURE_MIN_LOD,
        TEXTURE_MAX_LOD = GL_TEXTURE_MAX_LOD,
        TEXTURE_BASE_LEVEL = GL_TEXTURE_BASE_LEVEL,
        TEXTURE_MAX_LEVEL = GL_TEXTURE_MAX_LEVEL,
        TEXTURE_WRAP_S = GL_TEXTURE_WRAP_S,
        TEXTURE_WRAP_T = GL_TEXTURE_WRAP_T,
        TEXTURE_WRAP_R = GL_TEXTURE_WRAP_R,
        TEXTURE_PRIORITY = GL_TEXTURE_PRIORITY,
        DEPTH_TEXTURE_MODE = GL_DEPTH_TEXTURE_MODE,
        GENERATE_MIPMAP = GL_GENERATE_MIPMAP
    };

    enum class FilterMode
    {
        NEAREST = GL_NEAREST,
        BILINEAR = GL_LINEAR,
        TRILINEAR_NN = GL_NEAREST_MIPMAP_NEAREST,
        TRILINEAR_NL = GL_NEAREST_MIPMAP_LINEAR,
        TRILINEAR_LN = GL_LINEAR_MIPMAP_NEAREST,
        TRILINEAR_LL = GL_LINEAR_MIPMAP_LINEAR
    };
    enum class FilterMagMin
    {
        MAG = GL_TEXTURE_MAG_FILTER,
        MIN = GL_TEXTURE_MIN_FILTER
    };

    enum class StencilOperation
    {
        ZERO = GL_ZERO,
        KEEP = GL_KEEP,
        REPLACE = GL_REPLACE,
        INCR = GL_INCR,
        INCR_WRAP = GL_INCR_WRAP,
        DECR = GL_DECR,
        DECR_WRAP = GL_DECR_WRAP,
        INVERT = GL_INVERT
    };

    enum class BlendFactor
    {
        ZERO = GL_ZERO,
        ONE = GL_ONE,
        SRC_COLOR = GL_SRC_COLOR,
        ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
        DST_COLOR = GL_DST_COLOR,
        ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
        SRC_ALPHA = GL_SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
        DST_ALPHA = GL_DST_ALPHA,
        ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
        CONSTANT_COLOR = GL_CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
        CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
        ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
        SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE,
        SRC1_COLOR = GL_SRC1_COLOR,
        ONE_MINUS_SRC1_COLOR = GL_ONE_MINUS_SRC1_COLOR,
        SRC1_ALPHA = GL_SRC1_ALPHA,
        ONE_MINUS_SRC1_ALPHA = GL_ONE_MINUS_SRC1_ALPHA
    };

    enum class BlendEquationE
    {
        FUNC_ADD = GL_FUNC_ADD,
        FUNC_SUBTRACT = GL_FUNC_SUBTRACT,
        FUNC_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT,
        MIN = GL_MIN,
        MAX = GL_MAX
    };

    enum class Function
    {
        NEVER = GL_NEVER,
        LESS = GL_LESS,
        LEQUAL = GL_LEQUAL,
        GREATER = GL_GREATER,
        GEQUAL = GL_GEQUAL,
        EQUAL = GL_EQUAL,
        NOT_EQUAL = GL_NOTEQUAL,
        ALWAYS = GL_ALWAYS
    };

    enum class Attachment
    {
        NONE = GL_NONE,
        COLOR0 = GL_COLOR_ATTACHMENT0,
        COLOR1 = GL_COLOR_ATTACHMENT1,
        COLOR2 = GL_COLOR_ATTACHMENT2,
        COLOR3 = GL_COLOR_ATTACHMENT3,
        COLOR4 = GL_COLOR_ATTACHMENT4,
        COLOR5 = GL_COLOR_ATTACHMENT5,
        COLOR6 = GL_COLOR_ATTACHMENT6,
        COLOR7 = GL_COLOR_ATTACHMENT7,
        DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT,
        STENCIL = GL_STENCIL_ATTACHMENT,
        DEPTH = GL_DEPTH_ATTACHMENT,
        BACK = GL_BACK,
        FRONT = GL_FRONT
    };

    enum class Pushable
    {
        FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS,
        SHADER_PROGRAM,
        BLEND_STATES,
        DEPTH_STATES,
        STENCIL_STATES,
        VAO,
        VBO,
        VIEWPORT,
        CULL_FACE,
        COLOR_MASK,
        MODEL_MATRIX,
        VIEW_MATRIX,
        PROJECTION_MATRIX,
        ALL_MATRICES,
        VIEWPROJ_MODE
    };

    static void ClearError();
    static bool CheckError(int line, const String &func, const String &file);
    static bool CheckFramebufferError();

    static void Clear(GLbitfield bufferBit);
    static void Clear(GL::BufferBit bufferBit);
    static void SetClearColorBufferValue(const Color &clearColor);
    static void ClearColorBuffer(const Color &clearColor = Color::Zero(),
                                 bool clearR = true,
                                 bool clearG = true,
                                 bool clearB = true,
                                 bool clearA = true);
    static void ClearDepthBuffer();
    static void ClearStencilBuffer();
    static void ClearDepthBuffer(float clearDepth);
    static void ClearStencilBuffer(int stencilValue);
    static void SetClearDepthBufferValue(float clearDepthBufferValue);
    static void SetClearStencilBufferValue(int clearStencilBufferValue);
    static void ClearStencilDepthBuffers(int clearStencilValue = 0,
                                         float clearDepthValue = 1.0f);
    static void ClearColorStencilDepthBuffers(
        const Color &clearColorValue = Color::Zero(),
        int clearStencilValue = 0,
        float clearDepthValue = 1.0f);

    static void Enablei(GL::Enablable glTest, int index);
    static void Disablei(GL::Enablable glTest, int index);
    static void Enable(GL::Enablable glTest);
    static void Disable(GL::Enablable glTest);
    static void SetEnabled(GL::Enablable glTest, bool enabled);
    static void SetEnabledi(GL::Enablable glTest, int index, bool enabled);
    static bool IsEnabled(GL::Enablable glTest);
    static bool IsEnabledi(GL::Enablable glTest, int index);
    static bool CanEnablableBeIndexed(GL::Enablable enablable);
    static int GetEnablableIndexMax(GL::Enablable enablable);

    static void EnableVertexAttribArray(int location);
    static void DisableVertexAttribArray(int location);
    static void VertexAttribPointer(int location,
                                    int dataComponentsCount,
                                    GL::VertexAttribDataType dataType,
                                    bool dataNormalized,
                                    int dataStride,
                                    int dataOffset);

    static void PolygonMode(GL::Face face, GL::Enum mode);

    static GLvoid *MapBuffer(GL::BindTarget target, GL::Enum access);
    static void UnMapBuffer(GL::BindTarget target);

    static void BlendColor(const Color &blendColor);
    static void BlendFunc(GL::BlendFactor srcFactor, GL::BlendFactor dstFactor);
    static void BlendFuncSeparate(GL::BlendFactor srcFactorColor,
                                  GL::BlendFactor dstFactorColor,
                                  GL::BlendFactor srcFactorAlpha,
                                  GL::BlendFactor dstFactorAlpha);
    static void BlendEquation(GL::BlendEquationE blendEquation);
    static void BlendEquationSeparate(GL::BlendEquationE blendEquationColor,
                                      GL::BlendEquationE blendEquationAlpha);

    static void BlitFramebuffer(int srcX0,
                                int srcY0,
                                int srcX1,
                                int srcY1,
                                int dstX0,
                                int dstY0,
                                int dstX1,
                                int dstY1,
                                GL::FilterMode filterMode,
                                GL::BufferBit bufferBitMask);
    static void BlitFramebuffer(const AARecti &srcRect,
                                const AARecti &dstRect,
                                GL::FilterMode filterMode,
                                GL::BufferBit bufferBitMask);

    static void Scissor(int x, int y, int width, int height);
    static void Scissor(const AARecti &scissorRectPx);
    static void ScissorIntersecting(int x, int y, int width, int height);
    static void ScissorIntersecting(const AARecti &scissorRectPx);

    static GLId CreateShader(GL::ShaderType shaderType);
    static void ShaderSource(GLId shaderId, const String &sourceCode);
    static bool CompileShader(GLId shaderId);
    static void DeleteShader(GLId shaderId);

    static GLId CreateProgram();
    static void AttachShader(GLId programId, GLId shaderId);
    static bool LinkProgram(GLId programId);
    static void BindAttribLocation(GLId programId,
                                   int location,
                                   const String &attribName);
    static void BindFragDataLocation(GLId programId,
                                     int location,
                                     const String &fragDataName);
    static void DeleteProgram(GLId programId);

    static void FramebufferTexture(GL::FramebufferTarget target,
                                   GL::Attachment attachment,
                                   GLId textureId,
                                   uint mipMapLevel = 0);
    static void FramebufferTexture2D(GL::FramebufferTarget target,
                                     GL::Attachment attachment,
                                     GL::TextureTarget texTarget,
                                     GLId textureId,
                                     uint mipMapLevel = 0);
    static void BindRenderbuffer(GL::RenderbufferTarget target,
                                 GLId renderbufferId);
    static void RenderbufferStorage(GL::RenderbufferTarget target,
                                    GL::RenderbufferFormat format,
                                    int width,
                                    int height);
    static void FramebufferRenderbuffer(GL::FramebufferTarget target,
                                        GL::Attachment attachment,
                                        GL::RenderbufferTarget rbTarget,
                                        GLId renderbufferId);
    static void DrawBuffers(const Array<GL::Attachment> &drawAttachments);
    static void ReadBuffer(GL::Attachment readAttachment);

    static void ReadPixels(int x,
                           int y,
                           int width,
                           int height,
                           GL::ColorComp inputComp,
                           GL::DataType outputDataType,
                           void *pixels);
    static void ReadPixels(const AARecti &readRect,
                           GL::ColorComp inputComp,
                           GL::DataType outputDataType,
                           void *pixels);

    static void Finish();
    static void Flush();

    static void Uniform(const String &name, int value);
    static void Uniform(const String &name, float value);
    static void Uniform(const String &name, double value);
    static void Uniform(const String &name, bool value);
    static void Uniform(const String &name, const Matrix3f &value);
    static void Uniform(const String &name, const Matrix4f &value);
    static void Uniform(const String &name, const Color &value);
    static void Uniform(const String &name, const Vector2 &value);
    static void Uniform(const String &name, const Vector3 &value);
    static void Uniform(const String &name, const Vector4 &value);
    static void Uniform(const String &name, const Array<int> &value);
    static void Uniform(const String &name, const Array<bool> &value);
    static void Uniform(const String &name, const Array<float> &value);
    static void Uniform(const String &name, const Array<double> &value);
    static void Uniform(const String &name, const Array<Color> &value);
    static void Uniform(const String &name, const Array<Vector2> &value);
    static void Uniform(const String &name, const Array<Vector3> &value);
    static void Uniform(const String &name, const Array<Vector4> &value);
    static void Uniform(const String &name, const Array<Matrix3> &value);
    static void Uniform(const String &name, const Array<Matrix4> &value);
    static void Uniform(int location, int value);
    static void Uniform(int location, float value);
    static void Uniform(int location, double value);
    static void Uniform(int location, bool value);
    static void Uniform(int location, const Matrix3f &value);
    static void Uniform(int location, const Matrix4f &value);
    static void Uniform(int location, const Color &value);
    static void Uniform(int location, const Vector2 &value);
    static void Uniform(int location, const Vector3 &value);
    static void Uniform(int location, const Vector4 &value);
    static void Uniform(int location, const Array<int> &value);
    static void Uniform(int location, const Array<bool> &value);
    static void Uniform(int location, const Array<float> &value);
    static void Uniform(int location, const Array<double> &value);
    static void Uniform(int location, const Array<Color> &value);
    static void Uniform(int location, const Array<Vector2> &value);
    static void Uniform(int location, const Array<Vector3> &value);
    static void Uniform(int location, const Array<Vector4> &value);
    static void Uniform(int location, const Array<Matrix3> &value);
    static void Uniform(int location, const Array<Matrix4> &value);

    static void PixelStore(GL::Enum pixelStoreEnum, int n);

    static void GenerateMipMap(GL::TextureTarget textureTarget);
    static void TexImage2D(GL::TextureTarget textureTarget,
                           uint textureWidth,
                           uint textureHeight,
                           GL::ColorFormat textureColorFormat,
                           GL::ColorComp inputDataColorComp,
                           GL::DataType inputDataType,
                           const void *data);
    static void TexImage3D(GL::TextureTarget textureTarget,
                           uint textureWidth,
                           uint textureHeight,
                           uint textureDepth,
                           GL::ColorFormat textureColorFormat,
                           GL::ColorComp inputDataColorComp,
                           GL::DataType inputDataType,
                           const void *data);
    static void TexSubImage3D(GL::TextureTarget textureTarget,
                              uint offsetX,
                              uint offsetY,
                              uint offsetZ,
                              uint textureWidth,
                              uint textureHeight,
                              uint textureDepth,
                              GL::ColorComp inputDataColorComp,
                              GL::DataType inputDataType,
                              const void *data);

    static void TexParameteri(GL::TextureTarget textureTarget,
                              GL::TexParameter textureParameter,
                              GLint value);
    static void TexParameterf(GL::TextureTarget textureTarget,
                              GL::TexParameter textureParameter,
                              GLfloat value);
    static void TexParameterFilter(GL::TextureTarget textureTarget,
                                   GL::FilterMagMin filterMagMin,
                                   GL::FilterMode filterMode);
    static void TexParameterWrap(GL::TextureTarget textureTarget,
                                 GL::WrapCoord wrapCoord,
                                 GL::WrapMode wrapMode);
    static void ActiveTexture(int activeTexture);
    static void LineWidth(float lineWidth);
    static void PointSize(float pointSize);

    static void GenFramebuffers(int n, GLId *glIds);
    static void GenRenderBuffers(int n, GLId *glIds);
    static void GenTextures(int n, GLId *glIds);
    static void GenVertexArrays(int n, GLId *glIds);
    static void GenBuffers(int n, GLId *glIds);
    static void DeleteFramebuffers(int n, const GLId *glIds);
    static void DeleteRenderBuffers(int n, const GLId *glIds);
    static void DeleteTextures(int n, const GLId *glIds);
    static void DeleteVertexArrays(int n, const GLId *glIds);
    static void DeleteBuffers(int n, const GLId *glIds);
    static void OnDeletedGLObjects(GL::BindTarget bindTarget,
                                   int n,
                                   const GLId *glIds);

    static void SetViewport(const AARect &viewportNDC);
    static void SetViewport(const AARecti &viewport);
    static void SetViewport(int x, int y, int width, int height);

    static void BindBuffer(GL::BindTarget target, GLId bufferId);
    static void BufferData(GL::BindTarget target,
                           GLuint dataSize,
                           const void *data,
                           GL::UsageHint usageHint);
    static void BufferSubData(GL::BindTarget target,
                              GLuint offset,
                              GLuint dataSize,
                              const void *data);
    static void SetColorMask(const std::array<bool, 4> &colorMask);
    static void SetColorMask(bool maskR, bool maskG, bool maskB, bool maskA);
    static void SetViewProjMode(GL::ViewProjMode mode);
    static void SetStencilOp(GL::StencilOperation zPass);
    static void SetStencilOp(GL::StencilOperation fail,
                             GL::StencilOperation zFail,
                             GL::StencilOperation zPass);
    static void SetStencilFunc(GL::Function stencilFunction, uint mask = 0xFF);
    static void SetStencilFunc(GL::Function stencilFunction,
                               Byte stencilValue,
                               uint mask = 0xFF);
    static void SetStencilValue(Byte value);
    static void SetDepthMask(bool writeDepth);
    static void SetDepthFunc(GL::Function testDepth);
    static void SetWireframe(bool wireframe);
    static void SetCullFace(const GL::Face cullFace);
    static void SetZNearFar(float zNear, float zFar);

    static Vector2 FromPixelsPointToPixelPerfect(const Vector2 &winPoint);
    static Vector2 FromPointToPointNDC(const Vector2 &point,
                                       const Vector2 &rectSize);
    static Vector2 FromPointNDCToPoint(const Vector2 &pointNDC,
                                       const Vector2 &rectSize);
    static Vector2 FromAmountToAmountNDC(const Vector2 &amount,
                                         const Vector2 &rectSize);
    static Vector2 FromAmountNDCToAmount(const Vector2 &amountNDC,
                                         const Vector2 &rectSize);
    static Vector2 FromWindowPointToViewportPoint(const Vector2 &winPoint,
                                                  const AARecti &viewport);
    static Vector2 FromViewportPointToWindowPoint(const Vector2 &vpPoint);
    static Vector2 FromViewportPointToWindowPoint(const Vector2i &vpPoint);
    static Vector2 FromWindowPointToViewportPoint(const Vector2 &winPoint);
    static Vector2 FromWindowPointToViewportPoint(const Vector2i &winPoint);
    static RectPoints FromViewportRectToViewportRectNDCPoints(
        const Rect &vpRect);
    static Rect FromViewportRectNDCToViewportRect(const Rect &vpRectNDC);
    static Rect FromWindowRectToWindowRectNDC(const Rect &winRect);
    static Rect FromWindowRectNDCToWindowRect(const Rect &winRectNDC);
    static AARect FromViewportRectToViewportRectNDC(const AARect &vpRect);
    static AARect FromViewportRectNDCToViewportRect(const AARect &vpRectNDC);
    static AARect FromWindowRectToWindowRectNDC(const AARect &winRect);
    static AARect FromWindowRectNDCToWindowRect(const AARect &winRectNDC);
    static Vector2 FromWindowAmountToWindowAmountNDC(const Vector2 &winAmount);
    static Vector2 FromWindowAmountNDCToWindowAmount(
        const Vector2 &winAmountNDC);
    static Vector2 FromViewportAmountToViewportAmountNDC(
        const Vector2 &vpAmount);
    static Vector2 FromViewportAmountNDCToViewportAmount(
        const Vector2 &vpAmountNDC);
    static Vector2 FromWindowPointNDCToWindowPoint(const Vector2 &winPointNDC);
    static Vector2 FromWindowPointToWindowPointNDC(const Vector2 &winPoint);
    static Vector2 FromWindowPointToWindowPointNDC(const Vector2i &winPoint);
    static Vector2 FromViewportPointToViewportPointNDC(const Vector2 &vpPoint);
    static Vector2 FromViewportPointToViewportPointNDC(const Vector2i &vpPoint);
    static Vector2 FromViewportPointNDCToViewportPoint(const Vector2 &vpPoint);

    static void Render(const VAO *vao,
                       GL::Primitive primitivesMode,
                       int elementsCount,
                       int startElementIndex = 0,
                       bool validateShader = true);
    static void DrawArrays(const VAO *vao,
                           GL::Primitive primitivesMode,
                           int verticesCount,
                           int startVertexIndex = 0);
    static void DrawElements(const VAO *vao,
                             GL::Primitive primitivesMode,
                             int elementsCount,
                             int startElementIndex = 0);
    static void RenderInstanced(const VAO *vao,
                                GL::Primitive primitivesMode,
                                int elementsCount,
                                int instanceAmount);
    static void DrawArraysInstanced(const VAO *vao,
                                    GL::Primitive primitivesMode,
                                    int verticesCount,
                                    int instanceAmount,
                                    int instanceStartIndex);
    static void DrawElementsInstanced(const VAO *vao,
                                      GL::Primitive primitivesMode,
                                      int elementsCount,
                                      int instanceAmount);

    static void VertexAttribDivisor(uint location, uint divisor);

    static int GetInteger(GL::Enum glEnum);
    static void GetInteger(GL::Enum glEnum, int *values);
    static bool GetBoolean(GL::Enum glEnum);
    static void GetBoolean(GL::Enum glEnum, bool *values);

    static void BindUniformBlock(GLId programId,
                                 const String &uniformBlockName,
                                 GLuint bindingPoint);
    static void UniformBlockBinding(GLId programId,
                                    GLuint uniformBlockLocation,
                                    GLuint bindingPoint);
    static GLuint GetUniformBlockIndex(GLId programId,
                                       const String &uniformBlockName);
    static int GetUniformLocation(const String &uniformName);
    static int GetUniformLocation(GLId programId, const String &uniformName);
    static int GetUniformsListSize(GLId shaderProgramId);
    static GL::UniformType GetUniformTypeAt(GLId shaderProgramId,
                                            GLuint uniformIndex);

    static bool ValidateProgram(GLId programId);
    static String GetProgramErrorMsg(GLId programId);
    static int GetProgramInteger(GLId programId, GL::Enum glEnum);
    static void GetProgramIntegers(GLId programId,
                                   GL::Enum glEnum,
                                   GLint *ints);

    static int GetShaderInteger(GLId shaderId, GL::Enum glEnum);
    static String GetShaderErrorMsg(GLId shaderId);

    static AARecti GetViewportRect();
    static Vector2i GetViewportSize();
    static float GetViewportAspectRatio();
    static Vector2 GetViewportPixelSize();

    static const Array<GL::Attachment> &GetDrawBuffers();
    static const GL::Attachment &GetReadBuffer();

    static const Color &GetBlendColor();
    static GL::BlendFactor GetBlendSrcFactorColor();
    static GL::BlendFactor GetBlendDstFactorColor();
    static GL::BlendFactor GetBlendSrcFactorAlpha();
    static GL::BlendFactor GetBlendDstFactorAlpha();
    static GL::BlendEquationE GetBlendEquationColor();
    static GL::BlendEquationE GetBlendEquationAlpha();
    static const AARecti &GetScissorRect();
    static const Color &GetClearColor();
    static GL::Enum GetPolygonMode(GL::Face face);
    static uint GetLineWidth();
    static uint GetStencilMask();
    static GL::Function GetStencilFunc();
    static GL::StencilOperation GetStencilOp();
    static Byte GetStencilValue();
    static bool IsColorMaskR();
    static bool IsColorMaskG();
    static bool IsColorMaskB();
    static bool IsColorMaskA();
    static std::array<bool, 4> GetColorMask();
    static bool GetDepthMask();
    static GL::Function GetDepthFunc();
    static bool IsWireframe();
    static GL::Face GetCullFace();
    static SDL_GLContext GetSharedGLContext();

    static void GetTexImage(GL::TextureTarget textureTarget,
                            GL::ColorComp colorComp,
                            Byte *pixels);
    static void GetTexImage(GL::TextureTarget textureTarget,
                            GL::ColorComp colorComp,
                            float *pixels);
    static void GetTexImage(GL::TextureTarget textureTarget,
                            GL::ColorComp colorComp,
                            GL::DataType dataType,
                            void *pixels);

    static void Bind(const GLObject *bindable);
    static void Bind(GL::BindTarget bindTarget, GLId glId);
    static void UnBind(const GLObject *bindable);
    static void UnBind(GL::BindTarget bindTarget);
    static GLId GetBoundId(GL::BindTarget bindTarget);
    static bool IsBound(const GLObject *bindable);
    static bool IsBound(GL::BindTarget bindTarget, GLId glId);

    static uint GetPixelBytesSize(GL::ColorFormat format);
    static uint GetPixelBytesSize(GL::ColorComp colorComp,
                                  GL::DataType dataType);
    static uint GetBytesSize(GL::DataType dataType);
    static uint GetNumComponents(GL::ColorComp colorComp);
    static uint GetNumComponents(GL::ColorFormat colorFormat);
    static GL::DataType GetDataTypeFrom(GL::ColorFormat format);
    static GL::ColorComp GetColorCompFrom(GL::ColorFormat format);
    static bool IsDepthFormat(GL::ColorFormat format);

    static void BindUniformBufferToShader(const String &uniformBlockName,
                                          const ShaderProgram *sp,
                                          const IUniformBuffer *buffer);

    static void Push(GL::Pushable pushable);
    static void Push(GL::Enablable enablable);
    static void Push(GL::BindTarget bindTarget);
    static void Pop(GL::Pushable pushable);
    static void Pop(GL::Enablable enablable);
    static void Pop(GL::BindTarget bindTarget);
    static void PushOrPop(GL::Pushable pushable, bool push);
    static void PushOrPop(GL::Enablable enablable, bool push);
    static void PushOrPop(GL::BindTarget bindTarget, bool push);

    static void PrintGLStats();
    static void PrintGLContext();

    static GL::ViewProjMode GetViewProjMode();

    static GL *GetInstance();
    GLUniforms *GetGLUniforms() const;

    GL();
    virtual ~GL();

private:
    void Init();

    // Context
    SDL_Window *m_auxiliarWindowToCreateSharedGLContext = nullptr;
    SDL_GLContext m_sharedGLContext = nullptr;

    StackAndValue<GLId> m_boundVAOIds;
    StackAndValue<GLId> m_boundVBOArrayBufferIds;
    StackAndValue<GLId> m_boundVBOElementsBufferIds;
    StackAndValue<GLId> m_boundTexture1DIds;
    StackAndValue<GLId> m_boundTexture2DIds;
    StackAndValue<GLId> m_boundTexture3DIds;
    StackAndValue<GLId> m_boundTextureCubeMapIds;
    StackAndValue<GLId> m_boundDrawFramebufferIds;
    StackAndValue<GLId> m_boundReadFramebufferIds;
    StackAndValue<GLId> m_boundShaderProgramIds;
    StackAndValue<GLId> m_boundUniformBufferIds;
    StackAndValue<float> m_lineWidths;
    StackAndValue<Byte> m_stencilValues;
    StackAndValue<uint> m_stencilMasks;
    StackAndValue<AARecti> m_viewportRects;
    StackAndValue<std::array<bool, 4>> m_colorMasks;
    std::stack<Matrix4> m_modelMatrices;
    std::stack<Matrix4> m_viewMatrices;
    std::stack<Matrix4> m_projectionMatrices;
    std::stack<GL::ViewProjMode> m_viewProjModes;

    UMap<GL::Enablable, StackAndValue<std::array<bool, 16>>, EnumClassHash>
        m_enabledVars;

    uint m_maxDrawBuffers = 0;
    StackAndValue<Array<GL::Attachment>> m_drawBuffers;
    StackAndValue<GL::Attachment> m_readBuffers;

    StackAndValue<bool> m_depthMasks;
    StackAndValue<GL::Function> m_depthFuncs;
    StackAndValue<Color> m_clearColors;
    StackAndValue<GL::Face> m_cullFaces;

    StackAndValue<AARecti> m_scissorRectsPx;
    StackAndValue<GL::Enum> m_frontPolygonModes;
    StackAndValue<GL::Enum> m_backPolygonModes;
    StackAndValue<GL::Enum> m_frontBackPolygonModes;
    StackAndValue<GL::Function> m_stencilFuncs;
    StackAndValue<GL::BlendFactor> m_blendSrcFactorColors;
    StackAndValue<GL::BlendFactor> m_blendDstFactorColors;
    StackAndValue<GL::BlendFactor> m_blendSrcFactorAlphas;
    StackAndValue<GL::BlendFactor> m_blendDstFactorAlphas;
    StackAndValue<Color> m_blendColors;
    StackAndValue<GL::BlendEquationE> m_blendEquationColors;
    StackAndValue<GL::BlendEquationE> m_blendEquationAlphas;
    StackAndValue<GL::StencilOperation> m_stencilOps;

    GLUniforms *m_glUniforms = nullptr;

    friend class GEngine;
};
}  // namespace Bang

#endif  // GL_H
