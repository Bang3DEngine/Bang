#define BANG_VERTEX
#define BANG_DEFERRED_RENDERING
#include "Common.glsl"

layout(location = 0) in vec3  B_VIn_Position;
layout(location = 1) in vec3  B_VIn_Normal;
layout(location = 2) in vec2  B_VIn_Uv;
layout(location = 3) in vec3  B_ParticlePos;
layout(location = 4) in float B_ParticleSize;
layout(location = 5) in vec4  B_ParticleColor;

out vec3 B_FIn_Position;
out vec3 B_FIn_Normal;
out vec2 B_FIn_AlbedoUv;
out vec4 B_FIn_ParticleColor;

void main()
{
    vec4 modelPosition = vec4(B_VIn_Position, 1);
    vec4 worldPosition = B_Model *
                         (vec4(vec3(B_ParticleSize), 1) *
                          modelPosition);
    worldPosition.xyz += B_ParticlePos;

    vec4 finalPos = (B_ProjectionView * worldPosition);
    B_FIn_Position = finalPos.xyz;
    B_FIn_Normal   = (B_Normal * vec4(B_VIn_Normal, 0)).xyz;
    B_FIn_AlbedoUv = (B_VIn_Uv * B_AlbedoUvMultiply + B_AlbedoUvOffset);
    B_FIn_ParticleColor = B_ParticleColor;

    gl_Position = finalPos;
}

