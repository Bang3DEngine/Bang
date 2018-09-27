#define BANG_VERTEX
#define BANG_DEFERRED_RENDERING
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;
layout(location = 3) in vec3 B_ParticlePos;

out vec3 B_FIn_Position;
out vec3 B_FIn_Normal;
out vec2 B_FIn_AlbedoUv;

void main()
{
    vec4 modelPosition = vec4(B_VIn_Position, 1);
    vec4 worldPosition = B_Model * modelPosition;
    worldPosition.xyz += B_ParticlePos;
    // worldPosition.xyz += vec3(gl_InstanceID, 0, 0);
    // worldPosition.x += gl_InstanceID * cos(gl_InstanceID + B_TimeSeconds * 2) * 1;
    // worldPosition.y += gl_InstanceID * sin(gl_InstanceID + B_TimeSeconds) * 1;

    vec4 finalPos = (B_ProjectionView * worldPosition);
    B_FIn_Position = finalPos.xyz;
    B_FIn_Normal   = (B_Normal * vec4(B_VIn_Normal, 0)).xyz;
    B_FIn_AlbedoUv = (B_VIn_Uv * B_AlbedoUvMultiply + B_AlbedoUvOffset);

    gl_Position = finalPos;
}

