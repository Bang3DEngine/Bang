#ifndef POINT_LIGHT_GLSL
#define POINT_LIGHT_GLSL

#define BANG_POINT_LIGHT
#include "LightCommon.glsl"

float GetPointLightFragmentLightness(const float pixelDistSq,
                                     const in vec3 pixelPosWorld,
                                     const in vec3 pixelNormalWorld)
{
    #if defined(BANG_DEFERRED_RENDERING)
    // If facing away, complete shadow directly
    vec3 pixelDirWorld = (pixelPosWorld - B_LightPositionWorld);
    if (dot(pixelNormalWorld, pixelDirWorld) >= 0)
    {
        return 0.0f;
    }

    if (pixelDistSq > (B_LightRange * B_LightRange))
    {
        return 0.0f;
    }

    float pixelDistance = sqrt(pixelDistSq);
    float pixelDistanceNorm = Map01(pixelDistance, B_LightZNear, B_LightZFar) * 2.0f - 1.0f;

    float biasedPixelDistanceNorm = (pixelDistanceNorm - B_LightShadowBias);
    float shadowMapDistance = texture(B_LightShadowMap, pixelDirWorld).r;


    float lightness = (shadowMapDistance * exp(-B_LightShadowExponentConstant * biasedPixelDistanceNorm));
    lightness = clamp(lightness, 0.0f, 1.0f);

    return lightness;
    #endif
    return 1.0f;
}

vec3 GetPointLightColorApportation(const vec3 lightPosWorld,
                                   const float lightRange,
                                   const float lightIntensity,
                                   const vec3 lightColor,
                                   const vec3 camPosWorld,
                                   const vec3 pixelPosWorld,
                                   const vec3 pixelNormalWorld,
                                   const vec3 pixelAlbedo,
                                   const bool pixelReceivesShadows,
                                   const float pixelRoughness,
                                   const float pixelMetalness)
{
    // Attenuation
    vec3 posDiff = (lightPosWorld - pixelPosWorld);
    float pixelDistSq = dot(posDiff, posDiff);
    float attenuation = ((lightRange * lightRange) / (pixelDistSq)) - 1.0;
    attenuation = min(attenuation, 1.0);
    float intensityAtt = lightIntensity * attenuation;
    if (intensityAtt <= 0.0)
    {
        return vec3(0);
    }

    float lightness = 1.0f;
    if (B_LightCastsShadows && pixelReceivesShadows)
    {
        lightness = GetPointLightFragmentLightness(pixelDistSq,
                                                   pixelPosWorld,
                                                   pixelNormalWorld);
        float lightShadowStrength = 1.0f;
        #ifdef BANG_DEFERRED_RENDERING
        lightShadowStrength = B_LightShadowStrength;
        #endif
        lightness = min(1, lightness + (1-lightShadowStrength));
    }

    vec3 N = pixelNormalWorld;
    vec3 V = normalize(camPosWorld - pixelPosWorld);
    vec3 L = normalize(lightPosWorld - pixelPosWorld);
    vec3 H = normalize(V + L);

    vec3 radiance = lightColor * intensityAtt;

    // Cook-Torrance BRDF
    float NDF         = DistributionGGX(N, H,  pixelRoughness);
    float G           = GeometrySmith(N, V, L, pixelRoughness);
    vec3  F0          = mix(vec3(0.04), pixelAlbedo, pixelMetalness);
    vec3  F           = FresnelSchlick(max(dot(H, V), 0.0), F0);
    vec3  nominator   = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3  specular    = nominator / denominator;

    vec3 kSpec   = F;
    vec3 kDiff   = (1.0 - kSpec) * (1.0 - pixelMetalness);
    vec3 diffuse = (kDiff * pixelAlbedo / PI);

    float surfaceDotWithLight = max(0.0, dot(N, L));

    vec3 lightApport = (diffuse + specular) * radiance * surfaceDotWithLight;
    lightApport *= lightness;

    return lightApport;
}

#endif
