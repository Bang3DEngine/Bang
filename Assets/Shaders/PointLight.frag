#include "ScreenPass.frag"
#include "LightCommon.glsl"

uniform float B_PointLightZFar;
uniform mat4 B_WorldToShadowMapMatrices[6];
uniform samplerCube B_LightShadowMap;
uniform samplerCubeShadow B_LightShadowMapSoft;

float GetFragmentLightness(const in vec3 pixelPosWorld,
                           const in vec3 pixelNormalWorld)
{
    if (B_LightShadowType == SHADOW_NONE) { return 1.0f; }
    else
    {
        // SHADOW_HARD or SHADOW_SOFT
        float pixelDistance = distance(pixelPosWorld, B_LightPositionWorld);
        pixelDistance /= B_PointLightZFar;
        // if (pixelDistance > B_LightRange) { return 0.0f; }

        // If facing away, complete shadow directly
        vec3 pixelDirWorld = (pixelPosWorld - B_LightPositionWorld);
        // if (dot(pixelNormalWorld, pixelDirWorld) >= 0) { return 0.0f; }

        float biasedPixelDistance = (pixelDistance - B_LightShadowBias);
        if (B_LightShadowType == SHADOW_HARD)
        {
            float shadowMapDistance = texture(B_LightShadowMap, pixelDirWorld).r;
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

vec3 GetLightColorApportation()
{
    vec3  pixelPosWorld    = B_ComputeWorldPosition();
    vec3  pixelNormalWorld = B_SampleNormal();
    vec3  pixelAlbedo      = B_SampleAlbedoColor().rgb;
    float pixelRoughness   = B_SampleRoughness();
    float pixelMetalness   = B_SampleMetalness();

    vec3 N = pixelNormalWorld;
    vec3 V = normalize(B_Camera_WorldPos - pixelPosWorld);
    vec3 L = normalize(B_LightPositionWorld - pixelPosWorld);
    vec3 H = normalize(V + L);

    // Linear Attenuation
    float d = distance(B_LightPositionWorld, pixelPosWorld) ;
    // float attenuation = clamp(1.0 - (d / B_LightRange), 0, 1);
    float attenuation = (B_LightRange * B_LightRange) / (d * d);
    float intensityAtt = B_LightIntensity * attenuation;
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

    float lightness = 1.0f;
    if (B_SampleReceivesShadows())
    {
        lightness = GetFragmentLightness(pixelPosWorld, pixelNormalWorld);
    }

    float surfaceDotWithLight = max(0.0, dot(N, L));

    vec3 lightApport = (diffuse + specular) * radiance * surfaceDotWithLight;
    lightApport *= lightness;

    return lightApport;
}

void main()
{
    if (B_SampleReceivesLight())
    {
        vec3 lightApport = GetLightColorApportation();
        B_GIn_Color = vec4(lightApport, 0);
    }
    else
    {
        B_GIn_Color = vec4(0);
    }
}


