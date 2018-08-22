#define BANG_FRAGMENT
#include "Common.glsl"

const int IBL_TYPE_DIFFUSE  = 0;
const int IBL_TYPE_SPECULAR = 1;

uniform int B_IBLType;
uniform float B_InputRoughness;
uniform samplerCube B_InputCubeMap;

uniform int B_SampleCount; // The lowest,  the more quality

in vec4 B_FIn_Position;

out vec4 B_Out_IrradianceColor;

// Based on:
// https://learnopengl.com/PBR/IBL/Diffuse-irradiance
// https://learnopengl.com/PBR/IBL/Specular-irradiance
// Thank you!

vec2 Hammersley(int i, int N);
float VanDerCorpus(int n, int base);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);

void main()
{
    float Gamma = 2.2f;
    const float PI = 3.1415926535f;

    vec3 irradiance = vec3(0.0);
    vec3 rayDirWorld = (B_FIn_Position.xyz);

    if (B_IBLType == IBL_TYPE_DIFFUSE)
    {
        vec3 normal = normalize(rayDirWorld);

        // Orthonormalize
        vec3 up = vec3(0, 1, 0);
        vec3 right = cross(up, normal);
        up = cross(normal, right);

        float nrSamples   = 0.0;
        float sampleDelta = 1.0f / B_SampleCount;
        for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
        {
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
            {
                float sinTheta = sin(theta);
                float cosTheta = cos(theta);

                // Spherical to cartesian (in tangent space)
                vec3 tangentSample = vec3(sinTheta * cosPhi,
                                          sinTheta * sinPhi,
                                          cosTheta);

                // Tangent space to world
                vec3 sampleVec = tangentSample.x * right +
                                 tangentSample.y * up +
                                 tangentSample.z * normal;

                // Add to irradiance
                vec3 cubeMapColor = pow(texture(B_InputCubeMap, sampleVec).rgb,
                                        vec3(Gamma));
                irradiance += cubeMapColor * cosTheta * sinTheta;

                nrSamples++;
            }
        }

        irradiance = PI * irradiance * (1.0 / float(nrSamples));
    }
    else // SPECULAR
    {
        vec3 normal = rayDirWorld;
        vec3      R = normal;
        vec3      V = R;

        float totalWeight = 0.0;
        vec3 prefilteredColor = vec3(0.0);
        for(int i = 0; i < B_SampleCount; ++i)
        {
            vec2 Xi = Hammersley(i, B_SampleCount);
            vec3 H  = ImportanceSampleGGX(Xi, normal, B_InputRoughness);
            vec3 L  = normalize(2.0 * dot(V, H) * H - V);

            float NdotL = max(dot(normal, L), 0.0);
            if(NdotL > 0.0)
            {
                vec3 cubeMapColor = pow(texture(B_InputCubeMap, L).rgb,
                                        vec3(Gamma));
                prefilteredColor += cubeMapColor * NdotL;
                totalWeight      += NdotL;
            }
        }

        irradiance = prefilteredColor / totalWeight;
    }

    // B_Out_IrradianceColor = vec4(pow(irradiance, vec3(Gamma)), 1);
    B_Out_IrradianceColor = vec4(irradiance, 1);
}


// =============================================================

// Helper functions
float VanDerCorpus(int n, int base)
{
    float invBase = 1.0 / float(base);
    float denom   = 1.0;
    float result  = 0.0;

    for(int i = 0; i < 32; ++i)
    {
        if(n > 0)
        {
            denom   = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n       = int(float(n) / 2.0);
        }
    }

    return result;
}
vec2 Hammersley(int i, int N)
{
    return vec2(float(i)/float(N), VanDerCorpus(i, 2));
}
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}
// ----------------------------------------------------------------------------


