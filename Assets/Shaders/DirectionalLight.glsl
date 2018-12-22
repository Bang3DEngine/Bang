#ifndef DIRECTIONAL_LIGHT_GLSL
#define DIRECTIONAL_LIGHT_GLSL

#define BANG_DIRECTIONAL_LIGHT
#include "LightCommon.glsl"

float GetDirectionalLightFragmentLightness(const in vec3 pixelPosWorld,
                                           const in vec3 pixelNormalWorld,
                                           const in vec3 lightForwardWorld,
                                           const in vec3 camPosWorld)
{
    #if defined(BANG_DEFERRED_RENDERING)
    float ShadowDist = B_LightZFar * 2;

    // If facing away, complete shadow directly
    if (dot(pixelNormalWorld, lightForwardWorld) >= 0)
    {
        return 0.0f;
    }

    // If further than shadow distance, 1.0f lightness directly (no shadow)
    float distPixelToCamPosWorld = distance(pixelPosWorld, camPosWorld);
    if (distPixelToCamPosWorld > ShadowDist)
    {
        return 1.0f;
    }

    // Get uvs in shadow map, and sample the shadow map depth
    vec4 worldPosInLightSpace = (B_LightWorldToShadowMapMatrix * vec4(pixelPosWorld, 1));
    vec2 shadowMapUv = (worldPosInLightSpace.xy * 0.5f + 0.5f);

    // Get actual world pixel depth
    float worldPosDepthFromLightSpace = worldPosInLightSpace.z * 0.5f + 0.5f;
    float shadowMapDepth = texture(B_LightShadowMap, shadowMapUv).r;
    float biasedWorldDepth = (worldPosDepthFromLightSpace - B_LightShadowBias);

    float lightness = (shadowMapDepth * exp(-B_LightShadowExponentConstant * biasedWorldDepth));
    lightness = clamp(lightness, 0.0f, 1.0f);

    // Attenuate lightness if arriving to shadow distance boundary
    const float StartFadeCloseness = 0.95f;
    const float FadeWidth = (1.0f - StartFadeCloseness);
    float closenessToShadowDistanceBoundary = (distPixelToCamPosWorld /
                                               ShadowDist);
    if (closenessToShadowDistanceBoundary > StartFadeCloseness)
    {
        float relDistance = (1.0f - closenessToShadowDistanceBoundary);
        relDistance = max(relDistance, 0.0f);

        lightness += 1.0f - (relDistance / FadeWidth);
        lightness = min(lightness, 1);
    }
    return lightness;
    #endif

    return 1.0f;
}

vec3 GetDirectionalLightColorApportation(const vec3 lightForwardWorld,
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
    vec3 N = pixelNormalWorld;
    vec3 V = normalize(camPosWorld - pixelPosWorld);
    vec3 L = -lightForwardWorld;
    vec3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF         = DistributionGGX(N, H,  pixelRoughness);
    float G           = GeometrySmith(N, V, L, pixelRoughness);
    vec3  F0          = mix(vec3(0.04), pixelAlbedo, pixelMetalness);
    vec3  F           = FresnelSchlick(max(dot(H, V), 0.0), F0);
    vec3  nominator   = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3  specular    = nominator / denominator;

    float surfaceDotWithLight = max(dot(N, L), 0.0);
    vec3 radiance = lightColor * lightIntensity;

    vec3 kSpec   = F;
    vec3 kDiff   = (1.0 - kSpec) * (1.0 - pixelMetalness);
    vec3 diffuse = (kDiff * pixelAlbedo / PI);

    float lightness = 1.0f;
    if (B_LightCastsShadows && pixelReceivesShadows)
    {
        lightness = GetDirectionalLightFragmentLightness(pixelPosWorld,
                                                         pixelNormalWorld,
                                                         lightForwardWorld,
                                                         camPosWorld);
        float lightShadowStrength = 1.0f;
        #ifdef BANG_DEFERRED_RENDERING
        lightShadowStrength = B_LightShadowStrength;
        #endif
        lightness = min(1, lightness + (1-lightShadowStrength));
    }

    vec3 lightApport = (diffuse + specular) * radiance * surfaceDotWithLight;
    lightApport *= lightness;

    return lightApport;
}

#endif

