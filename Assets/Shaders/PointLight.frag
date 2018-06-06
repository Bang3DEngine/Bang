#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING
#include "ScreenPass.frag"
#include "LightCommon.glsl"

uniform float B_PointLightZFar;
uniform mat4 B_WorldToShadowMapMatrices[6];
uniform samplerCube B_LightShadowMap;
uniform samplerCubeShadow B_LightShadowMapSoft;

float GetPointLightFragmentLightness(const float pixelDistSq,
                                     const in vec3 pixelPosWorld,
                                     const in vec3 pixelNormalWorld)
{
    if (B_LightShadowType == SHADOW_NONE) { return 1.0f; }
    else
    {
        // SHADOW_HARD or SHADOW_SOFT

        // If facing away, complete shadow directly
        vec3 pixelDirWorld = (pixelPosWorld - B_LightPositionWorld);
        if (dot(pixelNormalWorld, pixelDirWorld) >= 0) { return 0.0f; }

        if (pixelDistSq > B_LightRange*B_LightRange) { return 0.0f; }
        float pixelDistance = sqrt(pixelDistSq);
        float pixelDistanceNorm = pixelDistance / B_PointLightZFar;

        float biasedPixelDistance = (pixelDistanceNorm - B_LightShadowBias);
        if (B_LightShadowType == SHADOW_HARD)
        {
            float shadowMapDistance = texture(B_LightShadowMap, pixelDirWorld).r;
            if (shadowMapDistance == 1.0f) { return 1.0f; }
            float depthDiff = (biasedPixelDistance - shadowMapDistance);
            return (depthDiff > 0.0) ? 0.0 : 1.0;
        }
        else // SHADOW_SOFT
        {
            // Get the PCF value from 0 to 1
            float lightness = texture(B_LightShadowMapSoft,
                                      vec4(pixelDirWorld, biasedPixelDistance));
            return lightness;
        }
    }
    return 1.0f;
}

vec3 GetPointLightColorApportation(const vec3 pixelPosWorld,
                                   const vec3 pixelNormalWorld,
                                   const vec3 pixelAlbedo,
                                   const float pixelRoughness,
                                   const float pixelMetalness)
{
    // Attenuation
    vec3 posDiff = (B_LightPositionWorld - pixelPosWorld);
    float pixelDistSq = dot(posDiff, posDiff);
    float attenuation = ((B_LightRange*B_LightRange) / (pixelDistSq)) - 1.0;
    attenuation = min(attenuation, 1.0);
    float intensityAtt = B_LightIntensity * attenuation;
    if (intensityAtt <= 0.0) { return vec3(0); }

    float lightness = 1.0f;
    if (B_SampleReceivesShadows())
    {
        lightness = GetPointLightFragmentLightness(pixelDistSq,
                                                   pixelPosWorld,
                                                   pixelNormalWorld);
    }

    vec3 N = pixelNormalWorld;
    vec3 V = normalize(B_Camera_WorldPos - pixelPosWorld);
    vec3 L = normalize(B_LightPositionWorld - pixelPosWorld);
    vec3 H = normalize(V + L);


    vec3 radiance = B_LightColor.rgb * intensityAtt;

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

vec3 GetPointLightColorApportation()
{
    vec3  pixelPosWorld    = B_ComputeWorldPosition();
    vec3  pixelNormalWorld = B_SampleNormal();
    vec3  pixelAlbedo      = B_SampleAlbedoColor().rgb;
    float pixelRoughness   = B_SampleRoughness();
    float pixelMetalness   = B_SampleMetalness();

    return GetPointLightColorApportation(pixelPosWorld,
                                         pixelNormalWorld,
                                         pixelAlbedo,
                                         pixelRoughness,
                                         pixelMetalness);
}

void main()
{
    if (B_SampleReceivesLight())
    {
        vec3 lightApport = GetPointLightColorApportation();
        B_GIn_Color = vec4(lightApport, 0);
    }
    else
    {
        B_GIn_Color = vec4(0);
    }
}


