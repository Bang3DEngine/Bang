#define BANG_FRAGMENT
#include "Common.glsl"

uniform sampler2D B_Test2;
uniform samplerCube B_InputCubeMap;

in vec4 B_FIn_Position;

out vec4 B_Out_IrradianceColor;

// Sample copied from:
// https://learnopengl.com/PBR/IBL/Diffuse-irradiance
// Thank you!

void main()
{
    const float PI = 3.1415926535f;

    vec3 irradiance = vec3(0.0);

    vec3 rayDirWorld = (B_FIn_Position.xyz);
    vec3 normal = normalize(rayDirWorld);

    // Orthonormalize
    vec3 up = vec3(0, 1, 0);
    // if ( abs( dot(normal, up) ) > 0.99 ) { up = vec3(1,0,0); }
    vec3 right = cross(up, normal);
    up = cross(normal, right);

    float sampleDelta = 0.025;
    float nrSamples   = 0.0;
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
            irradiance += texture(B_InputCubeMap, sampleVec).rgb *
                          cosTheta * sinTheta;

            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    B_Out_IrradianceColor = vec4(irradiance, 1);
}
