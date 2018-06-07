#ifndef LIGHT_COMMON_GLSL
#define LIGHT_COMMON_GLSL

const int SHADOW_NONE = 0;
const int SHADOW_HARD = 1;
const int SHADOW_SOFT = 2;

const int LIGHT_TYPE_DIRECTIONAL = 0;
const int LIGHT_TYPE_POINT       = 1;

#if defined(BANG_DEFERRED_RENDERING)

    uniform float B_LightRange;
    uniform vec4  B_LightColor;
    uniform float B_LightIntensity;
    uniform vec3  B_LightForwardWorld;
    uniform vec3  B_LightPositionWorld;

    uniform int B_LightShadowType;
    uniform float B_LightShadowBias;

#elif defined(BANG_FORWARD_RENDERING) // Forward lighting uniforms

    #define BANG_MAX_FORWARD_LIGHTS 128
    uniform int B_ForwardRenderingLightNumber;
    uniform vec4[BANG_MAX_FORWARD_LIGHTS]   B_ForwardRenderingLightColors;
    uniform vec3[BANG_MAX_FORWARD_LIGHTS]   B_ForwardRenderingLightPositions;
    uniform vec3[BANG_MAX_FORWARD_LIGHTS]   B_ForwardRenderingLightForwardDirs;
    uniform float[BANG_MAX_FORWARD_LIGHTS]  B_ForwardRenderingLightIntensities;
    uniform float[BANG_MAX_FORWARD_LIGHTS]  B_ForwardRenderingLightRanges;
    uniform int[BANG_MAX_FORWARD_LIGHTS]    B_ForwardRenderingLightTypes;

#endif

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

#endif
