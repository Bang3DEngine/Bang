#include "ScreenPass.frag"
#include "LightCommon.glsl"

uniform float B_ShadowDistance;
uniform mat4 B_WorldToShadowMapMatrix;
uniform sampler2D B_LightShadowMap;
uniform sampler2DShadow B_LightShadowMapSoft;

float GetFragmentLightness(const in vec3 pixelPosWorld,
                           const in vec3 pixelNormalWorld,
                           const in vec3 lightForwardWorld,
                           const in vec3 camPosWorld)
{
    float ShadowDist = B_ShadowDistance * 2;
    if (B_LightShadowType == SHADOW_NONE) { return 1.0f; }
    else
    {
        // SHADOW_HARD or SHADOW_SOFT

        // If facing away, complete shadow directly
        if (dot(pixelNormalWorld, -lightForwardWorld) < 0) { return 0.0f; }

        // If further than shadow distance, 1.0f lightness directly (no shadow)
        vec3 camPosWorld = B_GetCameraPositionWorld();
        float distPixelToCamPosWorld = distance(pixelPosWorld, camPosWorld);
        if (distPixelToCamPosWorld > ShadowDist) { return 1.0f; }

        // Get uvs in shadow map, and sample the shadow map depth
        vec2 shadowMapUv = (B_WorldToShadowMapMatrix * vec4(pixelPosWorld,1)).xy;
        shadowMapUv = shadowMapUv * 0.5f + 0.5f;

        // Get actual world pixel depth
        vec3 worldPosInLightSpace = (B_WorldToShadowMapMatrix * vec4(pixelPosWorld,1)).xyz;
        float worldPosDepthFromLightSpace = worldPosInLightSpace.z * 0.5f + 0.5f;

        // Bias it, taking into account slope
        float MinBias = B_LightShadowBias;
        float MaxBias = B_LightShadowBias * 2;
        float bias = tan(acos(dot(pixelNormalWorld, lightForwardWorld)));
        bias = MinBias; // + clamp(bias, 0, MaxBias);
        float biasedWorldDepth = (worldPosDepthFromLightSpace - B_LightShadowBias);

        float lightness;
        if (B_LightShadowType == SHADOW_HARD)
        {
            float shadowMapDepth = texture(B_LightShadowMap, shadowMapUv).r;
            if (shadowMapDepth == 1.0f) { return 1.0f; }
            float depthAlbedo = (shadowMapDepth - biasedWorldDepth);
            lightness = (depthAlbedo > 0.0) ? 1.0 : 0.0;
        }
        else // SHADOW_SOFT
        {
            // Get the PCF value from 0 to 1
            lightness = texture(B_LightShadowMapSoft, vec3(shadowMapUv,
                                                           biasedWorldDepth));
        }

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
    }
}

vec3 GetLightColorApportation()
{
    vec3  pixelPosWorld    = B_ComputeWorldPosition();
    vec3  pixelNormalWorld = B_SampleNormal();
    vec3  pixelAlbedo      = B_SampleAlbedoColor().rgb;
    float pixelRoughness   = B_SampleRoughness();
    float pixelMetalness   = B_SampleMetalness();

    vec3 N = pixelNormalWorld;
    vec3 V = normalize(B_Camera_WorldPos - pixelPosWorld);
    vec3 L = -B_LightForwardWorld;
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
    vec3 radiance = B_LightColor.rgb * B_LightIntensity;

    vec3 kSpec   = F;
    vec3 kDiff   = (1.0 - kSpec) * (1.0 - pixelMetalness);
    vec3 diffuse = (kDiff * pixelAlbedo / PI);

    float lightness = GetFragmentLightness(pixelPosWorld,
                                           pixelNormalWorld,
                                           B_LightForwardWorld,
                                           B_Camera_WorldPos);

    vec3 lightApport = (diffuse + specular) * radiance * surfaceDotWithLight;
    lightApport *= lightness;

    return lightApport;
}

void main()
{
    if (B_SampleReceivesLight())
    {
        vec4 originalColor = B_SampleColor();
        vec3 lightApport   = GetLightColorApportation();
        B_GIn_Color = vec4(originalColor.rgb + lightApport, originalColor.a);
    }
    else
    {
        discard;
    }
}

