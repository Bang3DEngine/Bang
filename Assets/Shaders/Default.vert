#define BANG_VERTEX
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;
layout(location = 3) in vec3 B_VIn_Tangent;

out vec3 B_FIn_Position;
out vec3 B_FIn_Normal;
out vec2 B_FIn_AlbedoUv;
out vec2 B_FIn_NormalMapUv;
out vec3 B_FIn_Tangent;
out mat3 B_TBN;

void main()
{
    B_FIn_Position    = ( B_Model * vec4(B_VIn_Position, 1) ).xyz;
    B_FIn_Normal      = (B_Normal * vec4(B_VIn_Normal, 0)).xyz;
    B_FIn_AlbedoUv    = B_VIn_Uv * B_AlbedoUvMultiply + B_AlbedoUvOffset;
    B_FIn_NormalMapUv = B_VIn_Uv * B_NormalMapUvMultiply + B_NormalMapUvOffset;
    gl_Position       = B_PVM * vec4(B_VIn_Position, 1);


    // Calculate TBN for normal mapping
    if (B_HasNormalMapTexture)
    {
        B_FIn_Tangent = normalize(B_Normal * vec4(B_VIn_Tangent, 0)).xyz;
        vec3 T = (B_FIn_Tangent);
        vec3 N = (B_FIn_Normal);
        vec3 B = cross(N, T);
        if (dot(cross(N, T), B) < 0.0) { T = T * -1.0; } // Ensure RH coord. system
        B_TBN = mat3(T, B, N);
    }
    else
    {
        B_FIn_Tangent = vec3(0);
        B_TBN = mat3(0);
    }
}
