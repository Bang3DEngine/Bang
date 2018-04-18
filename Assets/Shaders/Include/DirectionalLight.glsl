#include "LightCommon.glsl"

uniform mat4 B_WorldToShadowMapMatrix;
uniform sampler2D B_LightShadowMap;
uniform sampler2DShadow B_LightShadowMapSoft;

float GetFragmentLightness(const in vec3 pixelPosWorld,
                           const in vec3 pixelNormalWorld,
                           const in vec3 lightForwardWorld)
{
    if (B_LightShadowType == SHADOW_NONE) { return 1.0f; }
    else
    {
        // SHADOW_HARD or SHADOW_SOFT

        // If facing away, complete shadow directly
        if (dot(pixelNormalWorld, -lightForwardWorld) < 0) { return 0.0f; }

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

        if (B_LightShadowType == SHADOW_HARD)
        {
            float shadowMapDepth = texture(B_LightShadowMap, shadowMapUv).r;
            if (shadowMapDepth == 1.0f) { return 1.0f; }
            float depthAlbedo = (shadowMapDepth - biasedWorldDepth);
            return (depthAlbedo > 0.0) ? 1.0 : 0.0;
        }
        else // SHADOW_SOFT
        {
            // Get the PCF value from 0 to 1
            float lightness = texture(B_LightShadowMapSoft,
                                      vec3(shadowMapUv, biasedWorldDepth));
            return lightness;
        }
    }
}

vec3 GetDirectionalLightColorApportation(const in vec3  pixelPosWorld,
                                         const in vec3  pixelNormalWorld,
                                         const in vec3  pixelAlbedoColor,
                                         const in float pixelRoughness,
                                         const in vec3  lightForwardWorld,
                                         const in float lightIntensity,
                                         const in vec3  lightColor,
                                         const in vec3  camPosWorld)
{
    float lightDot     = max(0.0, dot(pixelNormalWorld, -lightForwardWorld));

    // DIFFUSE
    vec3 lightAlbedo  = pixelAlbedoColor * lightDot * lightIntensity * lightColor;

    // SPECULAR
    vec3 worldCamPos     = camPosWorld;
    vec3 pointToCamDir   = normalize(worldCamPos - pixelPosWorld);
    vec3 reflected       = -reflect(-lightForwardWorld, pixelNormalWorld);
    float specDot        = max(0.0, dot(reflected, pointToCamDir));
    float specShin       = min(pow(specDot, pixelRoughness), 1.0);
    vec3 lightSpecular   = specShin * lightDot * lightIntensity * lightColor;
    lightSpecular *= 0.5f;

    return lightAlbedo + lightSpecular;
}
