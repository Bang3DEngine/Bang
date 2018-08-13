#ifndef COMMON_GLSL
#define COMMON_GLSL

const float PI = 3.14159265359;

// Matrices related ///////////////////////////
uniform mat4 B_Model;
uniform mat4 B_ModelInv;
uniform mat4 B_Normal;
uniform mat4 B_View;
uniform mat4 B_ViewInv;
uniform mat4 B_Projection;
uniform mat4 B_ProjectionInv;
uniform mat4 B_ProjectionView;
uniform mat4 B_PVM;
uniform mat4 B_PVMInv;
//////////////////////////////////////////////

// Camera related ///////////////////////////
const int CAMERA_CLEARMODE_COLOR  = 0;
const int CAMERA_CLEARMODE_SKYBOX = 1;
uniform vec3        B_Camera_WorldPos;
uniform int         B_Camera_ClearMode;
uniform vec4        B_Camera_ClearColor;
uniform vec3        B_Camera_WorldForward;
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
uniform vec2  B_Viewport_MinPos;
uniform vec2  B_Viewport_Size;

// Material related /////////////////////////
uniform bool      B_MaterialReceivesLighting;
uniform vec4      B_MaterialAlbedoColor;
uniform float     B_MaterialRoughness;
uniform float     B_MaterialMetalness;
uniform sampler2D B_AlbedoTexture;
uniform float     B_AlphaCutoff;
uniform vec2      B_AlbedoUvOffset;
uniform vec2      B_AlbedoUvMultiply;
uniform bool      B_HasAlbedoTexture;
uniform sampler2D B_RoughnessTexture;
uniform sampler2D B_MetalnessTexture;
uniform sampler2D B_NormalMapTexture;
uniform vec2      B_NormalMapUvOffset;
uniform vec2      B_NormalMapUvMultiply;
uniform bool      B_HasNormalMapTexture;
uniform float     B_NormalMapMultiplyFactor;
uniform sampler2D B_BRDF_LUT;
// ///////////////////////////////////////

// Renderer related //////////////////////
uniform bool B_ReceivesShadows;
//////////////////////////////////////////


// GBuffer textures //////////////////////
uniform sampler2D B_GTex_Normal;
uniform sampler2D B_GTex_AlbedoColor;
uniform sampler2D B_GTex_Misc;
uniform sampler2D B_GTex_Color;
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

#if defined(BANG_FRAGMENT)
vec2 B_GetViewportPos() { return gl_FragCoord.xy - B_Viewport_MinPos; }
vec2 B_GetViewportUv()  { return B_GetViewportPos() / B_Viewport_Size; }
vec3 B_ComputeWorldPosition(float depth)
{
    return B_ComputeWorldPosition(depth, B_GetViewportUv());
}
#endif

//

// GBuffer Samplers //////////////////////
#if defined(BANG_FRAGMENT)

// #if defined(BANG_DEFERRED_RENDERING)
vec4  B_SampleColor(vec2 uv) { return texture(B_GTex_Color, uv); }
vec3  B_SampleNormal(vec2 uv)
{
    return normalize( texture(B_GTex_Normal, uv).xyz * 2.0f - 1.0f );
}
vec4  B_SampleAlbedoColor(vec2 uv)      { return texture(B_GTex_AlbedoColor, uv); }
bool  B_SampleReceivesLight (vec2 uv)   { return texture(B_GTex_Misc, uv).r > 0.1; }
bool  B_SampleReceivesShadows (vec2 uv) { return texture(B_GTex_Misc, uv).r > 0.5; }
float B_SampleRoughness (vec2 uv)       { return texture(B_GTex_Misc, uv).g; }
float B_SampleMetalness (vec2 uv)       { return texture(B_GTex_Misc, uv).b; }
float B_SampleDepth(vec2 uv)            { return texture(B_GTex_DepthStencil, uv).r; }
float B_SampleStencil(vec2 uv)          { return texture(B_GTex_DepthStencil, uv).a; }
float B_SampleFlags(vec2 uv)            { return texture(B_GTex_Misc, uv).z; }

vec4  B_SampleColor()           { return B_SampleColor(B_GetViewportUv()); }
vec3  B_SampleNormal()          { return B_SampleNormal(B_GetViewportUv()); }
vec4  B_SampleAlbedoColor()     { return B_SampleAlbedoColor(B_GetViewportUv()); }
bool  B_SampleReceivesLight()   { return B_SampleReceivesLight(B_GetViewportUv()); }
bool  B_SampleReceivesShadows() { return B_SampleReceivesShadows(B_GetViewportUv()); }
float B_SampleRoughness ()      { return B_SampleRoughness(B_GetViewportUv()); }
float B_SampleMetalness ()      { return B_SampleMetalness(B_GetViewportUv()); }
float B_SampleDepth()           { return B_SampleDepth(B_GetViewportUv()); }
float B_SampleStencil()         { return B_SampleStencil(B_GetViewportUv()); }
float B_SampleFlags()           { return B_SampleFlags(B_GetViewportUv()); }

vec4  B_SampleColorOffset(vec2 pixOffset)
{ return B_SampleColor(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }
vec3  B_SampleNormalOffset(vec2 pixOffset)
{ return B_SampleNormal(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }
vec4  B_SampleAlbedoColorOffset(vec2 pixOffset)
{ return B_SampleAlbedoColor(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }
bool  B_SampleReceivesLightOffset(vec2 pixOffset)
{ return B_SampleReceivesLight(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }
bool  B_SampleReceivesShadowsOffset(vec2 pixOffset)
{ return B_SampleReceivesShadows(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }
float B_SampleRoughnessOffset(vec2 pixOffset)
{ return B_SampleRoughness(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }
float B_SampleMetalnessOffset(vec2 pixOffset)
{ return B_SampleMetalness(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }
float B_SampleDepthOffset(vec2 pixOffset)
{ return B_SampleDepth(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }
float B_SampleStencilOffset(vec2 pixOffset)
{ return B_SampleStencil(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }
float B_SampleFlagsOffset(vec2 pixOffset)
{ return B_SampleFlags(B_GetViewportUv() + B_GetViewportStep() * pixOffset); }

vec3 B_ComputeWorldPosition() { return B_ComputeWorldPosition( B_SampleDepth() ); }
// #endif

#endif
// ///////////////////////////////////////

#endif


