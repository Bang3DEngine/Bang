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
        if (pixelDistance > B_LightRange) { return 0.0f; }

        // If facing away, complete shadow directly
        vec3 pixelDirWorld = (pixelPosWorld - B_LightPositionWorld);
        if (dot(pixelNormalWorld, pixelDirWorld) >= 0) { return 0.0f; }

        // Get shadow map distance
        float shadowMapDistance = texture(B_LightShadowMap, pixelDirWorld).r;

        float biasedPixelDistance = (pixelDistance - B_LightShadowBias);
        if (B_LightShadowType == SHADOW_HARD)
        {
            float depthAlbedo = (biasedPixelDistance - shadowMapDistance);
            return (depthAlbedo > 0.0) ? 0.0 : 1.0;
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

vec3 GetPointLightColorApportation(vec3  pixelPosWorld,
                                   vec3  pixelNormalWorld,
                                   vec3  pixelAlbedoColor,
                                   float pixelRoughness,
                                   vec3  lightPosWorld,
                                   float lightIntensity,
                                   float lightRange,
                                   vec3  lightColor,
                                   vec3  camPosWorld)
{
    vec3 dir = normalize(lightPosWorld - pixelPosWorld);
    float lightDot = max(0.0, dot(pixelNormalWorld, dir));

    // Linear Attenuation
    float d = distance(lightPosWorld, pixelPosWorld) ;
    float linearAtt = 1.0 - d / lightRange; // 0 is light's pos, 1 is max range
    linearAtt = clamp(linearAtt, 0.0, 1.0);

    float intensityAtt = lightIntensity * linearAtt;

    // DIFFUSE
    vec3 lightAlbedo = pixelAlbedoColor * lightDot * intensityAtt * lightColor;

    // SPECULAR
    vec3 worldCamPos     = camPosWorld;
    vec3 pointToCamDir   = normalize(worldCamPos - pixelPosWorld);
    vec3 lightToPointDir = normalize(lightPosWorld - pixelPosWorld);
    vec3 reflected       = -reflect(lightToPointDir, pixelNormalWorld);
    float specDot        = max(0.0, dot(reflected, pointToCamDir));

    float specShin = pow(specDot, pixelRoughness);
    specShin = min(specShin, 1.0);

    vec3 lightSpecular = specShin * lightDot * intensityAtt * lightIntensity * lightColor;
    lightSpecular *= 0.5f;

    return lightAlbedo + lightSpecular;
}
