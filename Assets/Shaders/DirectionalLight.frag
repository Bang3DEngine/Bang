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

void main()
{
    vec4 originalColor    = B_SampleColor();
    vec3 pixelPosWorld    = B_ComputeWorldPosition();
    vec3 pixelNormalWorld = B_SampleNormal();

    if (B_SampleReceivesLight())
    {
        // Get lightness of the pixel using shadow mapping
        vec3 camPosWorld = B_GetCameraPositionWorld();
        float lightness = GetFragmentLightness(pixelPosWorld,
                                               pixelNormalWorld,
                                               B_LightForwardWorld,
                                               camPosWorld);

        if (lightness > 0.0f)
        {
            vec4 diffColor = B_SampleAlbedoColor();
            vec3 dirLightApport = GetDirectionalLightColorApportation(
                                        pixelPosWorld,
                                        pixelNormalWorld,
                                        diffColor.rgb,
                                        B_SampleRoughness(),
                                        B_LightForwardWorld,
                                        B_LightIntensity,
                                        B_LightColor.rgb,
                                        camPosWorld);
            dirLightApport *= lightness;

            B_GIn_Color = vec4(originalColor.rgb + dirLightApport, diffColor.a);
        }
        else { discard; }
    }
    else
    {
        // Should not arrive here, because pixels that do not receive light
        // are stenciled
        // TODO: This seems not to be being stenciled, fix this
        // B_GIn_Color = vec4(1,0,0,1);
        discard;
    }
}
