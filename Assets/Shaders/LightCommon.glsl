#ifndef LIGHT_COMMON_GLSL
#define LIGHT_COMMON_GLSL

#include "Common.glsl"

const int LIGHT_TYPE_DIRECTIONAL = 0;
const int LIGHT_TYPE_POINT       = 1;

uniform bool B_LightCastsShadows;

#if defined(BANG_DEFERRED_RENDERING)

    uniform float B_LightZNear;
    uniform float B_LightZFar;

    uniform float B_LightRange;
    uniform vec4  B_LightColor;
    uniform float B_LightIntensity;
    uniform vec3  B_LightForwardWorld;
    uniform vec3  B_LightPositionWorld;

    uniform float B_LightShadowBias;
    uniform float B_LightShadowStrength;
    uniform float B_LightShadowExponentConstant;
    uniform mat4 B_LightWorldToShadowMapMatrix;

    #if defined(BANG_POINT_LIGHT)
    uniform samplerCube B_LightShadowMap;
    #elif defined(BANG_DIRECTIONAL_LIGHT)
    uniform sampler2D B_LightShadowMap;
    #endif


#elif defined(BANG_FORWARD_RENDERING) // Forward lighting uniforms

    const int BANG_MAX_FORWARD_LIGHTS = 128;
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

vec3 GetCameraSkyBoxSampleLod(const samplerCube cubeMap,
                              const vec3 direction,
                              const float lod)
{
    vec3 color;
    switch (B_Camera_ClearMode)
    {
        case CAMERA_CLEARMODE_COLOR:
            color = B_Camera_ClearColor.rgb;
        break;

        case CAMERA_CLEARMODE_SKYBOX:
            color = textureLod(cubeMap, direction, lod).rgb;
        break;

        default: color = vec3(1,0,1); break;
    }
    return color;
}

vec3 GetCameraSkyBoxSample(const samplerCube cubeMap, const vec3 direction)
{
    return GetCameraSkyBoxSampleLod(cubeMap, direction, 1.0);
}

vec4 GetIBLAmbientColor(const bool receivesLighting,
                        const vec3 pixelPosWorld,
                        const vec3 pixelNormalWorld,
                        const vec4 pixelAlbedo,
                        const float pixelRoughness,
                        const float pixelMetalness)
{
    vec4 finalColor = vec4(0);
    if (receivesLighting)
    {
        vec3 N = pixelNormalWorld.xyz;
        vec3 V = normalize(B_Camera_WorldPos.xyz - pixelPosWorld);
        vec3 R = reflect(-V, N);

        bool useReflectionProbeAsCubeMap = false;
        #ifdef BANG_USE_REFLECTION_PROBES
        bool isBoxed = (B_ReflectionProbeSize.x > 0);
        if (B_UseReflectionProbe)
        {
            if (isBoxed)
            {
                vec3 halfSkyBoxSize = B_ReflectionProbeSize * 0.5f;
                vec3 boxMax = (B_ReflectionProbeCenter + halfSkyBoxSize);
                vec3 boxMin = (B_ReflectionProbeCenter - halfSkyBoxSize);

                bool isPixelInsideBox = (pixelPosWorld.x >= boxMin.x &&
                                         pixelPosWorld.y >= boxMin.y &&
                                         pixelPosWorld.z >= boxMin.z &&
                                         pixelPosWorld.x <= boxMax.x &&
                                         pixelPosWorld.y <= boxMax.y &&
                                         pixelPosWorld.z <= boxMax.z);
                if (isPixelInsideBox)
                {
                    // Skybox is boxed, intersect with it!
                    float intersectionRightDist = (boxMax.x - pixelPosWorld.x) / R.x;
                    float intersectionLeftDist  = (boxMin.x - pixelPosWorld.x) / R.x;
                    float intersectionTopDist   = (boxMax.y - pixelPosWorld.y) / R.y;
                    float intersectionBotDist   = (boxMin.y - pixelPosWorld.y) / R.y;
                    float intersectionFrontDist = (boxMax.z - pixelPosWorld.z) / R.z;
                    float intersectionBackDist  = (boxMin.z - pixelPosWorld.z) / R.z;

                    if (intersectionRightDist <= 0) { intersectionRightDist = 9999999.9f; }
                    if (intersectionLeftDist  <= 0) { intersectionLeftDist  = 9999999.9f; }
                    if (intersectionTopDist   <= 0) { intersectionTopDist   = 9999999.9f; }
                    if (intersectionBotDist   <= 0) { intersectionBotDist   = 9999999.9f; }
                    if (intersectionFrontDist <= 0) { intersectionFrontDist = 9999999.9f; }
                    if (intersectionBackDist  <= 0) { intersectionBackDist  = 9999999.9f; }

                    float minIntersectionDist = min(intersectionRightDist,
                                                min(intersectionLeftDist,
                                                min(intersectionTopDist,
                                                min(intersectionBotDist,
                                                min(intersectionFrontDist,
                                                    intersectionBackDist)))));

                    vec3 intersectionPoint = pixelPosWorld + (minIntersectionDist * R);

                    useReflectionProbeAsCubeMap = true;
                    R = normalize(intersectionPoint - B_ReflectionProbeCenter);
                    R.z *= -1;
                }
            }
            else
            {
                useReflectionProbeAsCubeMap = true;
            }
        }
        #endif

        // Calculate ambient color
        float dotNV = max(dot(N, V), 0.0);
        vec3 F0  = mix(vec3(0.04), pixelAlbedo.rgb, pixelMetalness);
        vec3 FSR = FresnelSchlickRoughness(dotNV, F0, pixelRoughness);
        // vec3 FSR = FresnelSchlick(max(dot(N, V), 0.0), F0);

        vec3 specK = FSR;
        vec3 diffK = (1.0 - specK) * (1.0 - pixelMetalness);

        const float LOD_MAX_REFLECTION = 8.0;
        float lod = pixelRoughness * LOD_MAX_REFLECTION;

        vec3 diffuseCubeMapSample;
        vec3 specularCubeMapSample;
        #ifdef BANG_USE_REFLECTION_PROBES
        if (useReflectionProbeAsCubeMap)
        {
            diffuseCubeMapSample = GetCameraSkyBoxSample(B_ReflectionProbeDiffuse, N).rgb;
            specularCubeMapSample = GetCameraSkyBoxSampleLod(B_ReflectionProbeSpecular, R, lod).rgb;
        }
        else // SkyBox sampled directly
        #endif
        {
            diffuseCubeMapSample = GetCameraSkyBoxSample(B_SkyBoxDiffuse, N).rgb;
            specularCubeMapSample = GetCameraSkyBoxSampleLod(B_SkyBoxSpecular, R, lod).rgb;
        }

        vec3 diffuseAmbient = diffuseCubeMapSample * pixelAlbedo.rgb;
        vec3 specularAmbient = specularCubeMapSample;
        // vec2 envBRDF  = texture(B_BRDF_LUT, vec2(dotNV, pixelRoughness)).rg;

        vec3 diffuse  = diffuseAmbient * diffK;
        vec3 specular = specularAmbient * specK; // (specK * envBRDF.x + envBRDF.y);

        vec3 ambient = (diffuse) + (specular);
        finalColor = vec4(ambient, pixelAlbedo.a);
    }
    else
    {
        finalColor = pixelAlbedo;
    }
    return finalColor;
}

#endif
