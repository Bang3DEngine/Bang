#define BANG_VERTEX
#include "Common.glsl"

#ifdef BANG_HAS_ANIMATIONS
const int B_MAX_NUM_BONES = 128;
uniform mat4[B_MAX_NUM_BONES] B_BoneAnimationMatrices;
#endif

layout(location = 0) in vec3  B_VIn_Position;
layout(location = 1) in vec3  B_VIn_Normal;
layout(location = 2) in vec2  B_VIn_Uv;
layout(location = 3) in vec3  B_VIn_Tangent;
#ifdef BANG_HAS_ANIMATIONS
layout(location = 4) in vec4 B_VIn_VertexBonesIds; // Max 4 bones per vertex
layout(location = 5) in vec4 B_VIn_VertexBonesWeights;
#endif

out vec3 B_FIn_Position;
out vec3 B_FIn_Normal;
out vec2 B_FIn_AlbedoUv;
out vec2 B_FIn_NormalMapUv;
out vec3 B_FIn_Tangent;
out mat3 B_TBN;

void main()
{
    vec2 uv = vec2(B_VIn_Uv.x, 1.0 - B_VIn_Uv.y);

    vec4 modelPosition = vec4(B_VIn_Position, 1);
    vec4 modelNormal   = vec4(B_VIn_Normal,   0);

    #ifdef BANG_HAS_ANIMATIONS
    ivec4 boneIds = ivec4(B_VIn_VertexBonesIds);
    vec4 boneWeights = B_VIn_VertexBonesWeights;

    vec4 bonedPosition = vec4(0);
    bonedPosition += (B_BoneAnimationMatrices[boneIds[0]] * modelPosition) * boneWeights[0];
    bonedPosition += (B_BoneAnimationMatrices[boneIds[1]] * modelPosition) * boneWeights[1];
    bonedPosition += (B_BoneAnimationMatrices[boneIds[2]] * modelPosition) * boneWeights[2];
    bonedPosition += (B_BoneAnimationMatrices[boneIds[3]] * modelPosition) * boneWeights[3];
    modelPosition = vec4(bonedPosition.xyz, 1);

    vec4 bonedNormal = vec4(0);
    bonedNormal += (B_BoneAnimationMatrices[boneIds[0]] * modelNormal) * boneWeights[0];
    bonedNormal += (B_BoneAnimationMatrices[boneIds[1]] * modelNormal) * boneWeights[1];
    bonedNormal += (B_BoneAnimationMatrices[boneIds[2]] * modelNormal) * boneWeights[2];
    bonedNormal += (B_BoneAnimationMatrices[boneIds[3]] * modelNormal) * boneWeights[3];
    modelNormal = vec4(bonedNormal.xyz, 0);
    #endif

    B_FIn_Position    = (B_Model  * modelPosition).xyz;
    B_FIn_Normal      = (B_Normal * modelNormal).xyz;
    B_FIn_AlbedoUv    = uv * B_AlbedoUvMultiply    + B_AlbedoUvOffset;
    B_FIn_NormalMapUv = uv * B_NormalMapUvMultiply + B_NormalMapUvOffset;
    gl_Position       = B_PVM * modelPosition;

    // Calculate TBN for normal mapping
    if (B_HasNormalMapTexture)
    {
        B_FIn_Tangent = normalize(B_Normal * vec4(B_VIn_Tangent, 0)).xyz;
        vec3 T = (B_FIn_Tangent);
        vec3 N = (B_FIn_Normal);
        vec3 B = cross(N, T);
        if (dot(cross(N, T), B) >= 0.0) { T *= -1; } // Ensure RH coord. system
        B_TBN = mat3(T, B, N);
    }
    else
    {
        // B_FIn_Tangent = vec3(0);
        // B_TBN = mat3(0);
    }
}
