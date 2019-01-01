#ifndef COMMON_GLSL
#define COMMON_GLSL

const float PI = 3.14159265359;

// Matrices related ///////////////////////////
uniform mat4 B_Model;
uniform mat4 B_ModelInv;
uniform mat4 B_Normal;
uniform mat4 B_PVM;
uniform mat4 B_PVMInv;
//////////////////////////////////////////////

const int CAMERA_CLEARMODE_COLOR  = 0;
const int CAMERA_CLEARMODE_SKYBOX = 1;

layout (std140) uniform B_CameraUniformBuffer
{
    uniform mat4 B_View;
    uniform mat4 B_ViewInv;
    uniform mat4 B_Projection;
    uniform mat4 B_ProjectionInv;
    uniform mat4 B_ProjectionView;

    uniform vec4 B_Camera_WorldPos;
    uniform vec4 B_Camera_ClearColor;

    uniform vec2 B_Viewport_MinPos;
    uniform vec2 B_Viewport_Size;

    uniform int  B_Camera_ClearMode;
};


// Camera related ///////////////////////////
uniform samplerCube B_SkyBox;
uniform samplerCube B_SkyBoxDiffuse;
uniform samplerCube B_SkyBoxSpecular;

uniform bool        B_UseReflectionProbe;
uniform samplerCube B_ReflectionProbeDiffuse;
uniform samplerCube B_ReflectionProbeSpecular;
uniform vec3        B_ReflectionProbeCenter; // If boxed.
uniform vec3        B_ReflectionProbeSize;   // If boxed.
/////////////////////////////////////////////

uniform float B_TimeSeconds;

// Material related /////////////////////////
uniform vec4      B_MaterialAlbedoColor;
uniform sampler2D B_AlbedoTexture;
uniform float     B_AlphaCutoff;
uniform vec2      B_AlbedoUvOffset;
uniform vec2      B_AlbedoUvMultiply;
uniform bool      B_HasAlbedoTexture;
uniform bool      B_MaterialReceivesLighting;
// ///////////////////////////////////////

// Renderer related //////////////////////
uniform bool B_ReceivesShadows;
//////////////////////////////////////////

// GBuffer textures //////////////////////
uniform sampler2D B_GTex_Color;
uniform sampler2D B_GTex_AlbedoColor;
uniform sampler2D B_GTex_Light;
uniform sampler2D B_GTex_Normal;
uniform sampler2D B_GTex_Misc;
uniform sampler2D B_GTex_DepthStencil;
// ///////////////////////////////////////

// Util functions /////////////////
float B_LinearizeDepth(float d, float camZNear, float camZFar)
{
    return (2 * camZNear) / (camZFar + camZNear - d * (camZFar - camZNear));
}

vec3 B_GetCameraPositionWorld()
{
    return B_ViewInv[3].xyz;
}

vec3 B_ComputeWorldPosition(float depth, vec2 uv)
{
    vec4 projectedPos = vec4( (vec3(uv, depth) * 2.0 - 1.0), 1 );
    vec4 worldPos = (B_ProjectionInv * projectedPos);
    worldPos = (B_ViewInv * (worldPos/worldPos.w));
    return worldPos.xyz;
}

vec2 B_GetViewportStep()
{
    return 1.0 / B_Viewport_Size;
}

#ifdef BANG_FRAGMENT
vec2 B_GetViewportPos()
{
    return gl_FragCoord.xy - B_Viewport_MinPos;
}
vec2 B_GetViewportUv()
{
    return B_GetViewportPos() / B_Viewport_Size;
}
vec3 B_ComputeWorldPosition(float depth)
{
    return B_ComputeWorldPosition(depth, B_GetViewportUv());
}

// GBuffer Samplers //////////////////////
vec4  B_SampleColor(vec2 uv)
{
    return texture(B_GTex_Color, uv);
}
vec3  B_SampleNormal(vec2 uv)
{
    return normalize( texture(B_GTex_Normal, uv).xyz * 2.0f - 1.0f );
}
vec4  B_SampleAlbedoColor(vec2 uv)
{
    return texture(B_GTex_AlbedoColor, uv);
}
vec4  B_SampleLight(vec2 uv)
{
    return texture(B_GTex_Light, uv);
}
bool  B_SampleReceivesLight (vec2 uv)
{
    return texture(B_GTex_Misc, uv).r > 0.1;
}
bool  B_SampleReceivesShadows (vec2 uv)
{
    return texture(B_GTex_Misc, uv).r > 0.5;
}
float B_SampleRoughness (vec2 uv)
{
    return texture(B_GTex_Misc, uv).g;
}
float B_SampleMetalness (vec2 uv)
{
    return texture(B_GTex_Misc, uv).b;
}
float B_SampleDepth(vec2 uv)
{
    return texture(B_GTex_DepthStencil, uv).r;
}
float B_SampleStencil(vec2 uv)
{
    return texture(B_GTex_DepthStencil, uv).a;
}
float B_SampleFlags(vec2 uv)
{
    return texture(B_GTex_Misc, uv).z;
}
vec4 B_SampleColor()
{
    return B_SampleColor(B_GetViewportUv());
}
vec3 B_SampleNormal()
{
    return B_SampleNormal(B_GetViewportUv());
}
vec4 B_SampleAlbedoColor()
{
    return B_SampleAlbedoColor(B_GetViewportUv());
}
bool B_SampleReceivesLight()
{
    return B_SampleReceivesLight(B_GetViewportUv());
}
bool B_SampleReceivesShadows()
{
    return B_SampleReceivesShadows(B_GetViewportUv());
}
float B_SampleRoughness()
{
    return B_SampleRoughness(B_GetViewportUv());
}
float B_SampleMetalness()
{
    return B_SampleMetalness(B_GetViewportUv());
}
float B_SampleDepth()
{
    return B_SampleDepth(B_GetViewportUv());
}
float B_SampleStencil()
{
    return B_SampleStencil(B_GetViewportUv());
}
float B_SampleFlags()
{
    return B_SampleFlags(B_GetViewportUv());
}
vec4 B_SampleColorOffset(vec2 pixOffset)
{
    return B_SampleColor(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
vec3 B_SampleNormalOffset(vec2 pixOffset)
{
    return B_SampleNormal(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
vec4 B_SampleAlbedoColorOffset(vec2 pixOffset)
{
    return B_SampleAlbedoColor(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
bool B_SampleReceivesLightOffset(vec2 pixOffset)
{
    return B_SampleReceivesLight(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
bool B_SampleReceivesShadowsOffset(vec2 pixOffset)
{
    return B_SampleReceivesShadows(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
float B_SampleRoughnessOffset(vec2 pixOffset)
{
    return B_SampleRoughness(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
float B_SampleMetalnessOffset(vec2 pixOffset)
{
    return B_SampleMetalness(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
float B_SampleDepthOffset(vec2 pixOffset)
{
    return B_SampleDepth(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
float B_SampleStencilOffset(vec2 pixOffset)
{
    return B_SampleStencil(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
float B_SampleFlagsOffset(vec2 pixOffset)
{
    return B_SampleFlags(B_GetViewportUv() + B_GetViewportStep() * pixOffset);
}
vec3 B_ComputeWorldPosition()
{
    return B_ComputeWorldPosition( B_SampleDepth() );
}
#endif

float Map01(float x, float minX, float maxX)
{
    return (x-minX) / (maxX-minX);
}
float Brightness(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}
float Luma(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

float B_GetDepthWorld(float depth01)
{
    float projectedDepth =  ((depth01) * 2 - 1);
    vec4 unprojectedDepth = B_ProjectionInv *
                            vec4(0, 0, projectedDepth, 1);
    unprojectedDepth.z /= unprojectedDepth.w;
    float unprojectedDepthZ = unprojectedDepth.z;
    vec3 worldPos = B_ComputeWorldPosition(depth01, vec2(0));
    vec3 viewPos = (B_View * vec4(worldPos, 1)).xyz;
    return -viewPos.z;
}

#endif


