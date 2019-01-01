#define BANG_VERTEX
#define BANG_NORMAL_MAPPING
#include "Common.glsl"
#include "MaterialPBRUniforms.glsl"

const int B_MAX_NUM_BONES = 128;
uniform bool B_HasBoneAnimations;
uniform mat4[B_MAX_NUM_BONES] B_BoneAnimationMatrices;

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;
layout(location = 3) in vec3 B_VIn_Tangent;
layout(location = 4) in vec4 B_VIn_VertexBonesIds; // Max 4 bones per vertex
layout(location = 5) in vec4 B_VIn_VertexBonesWeights;

#ifndef ONLY_OUT_MODEL_POS_VEC4
    out vec3 B_FIn_Position;
    out vec3 B_FIn_Normal;
    out vec2 B_FIn_AlbedoUv;
    #ifdef BANG_NORMAL_MAPPING
        out vec2 B_FIn_NormalMapUv;
        out mat3 B_TBN;
    #endif
#endif

vec4 GetWorldVertexNormal()
{
    vec4 modelNormal = vec4(B_VIn_Normal, 0);

    ivec4 boneIds = ivec4(B_VIn_VertexBonesIds);
    vec4 boneWeights = B_VIn_VertexBonesWeights;

    if (B_HasBoneAnimations)
    {
        vec4 bonedNormal = vec4(0);
        bonedNormal += (B_BoneAnimationMatrices[boneIds[0]] * modelNormal) * boneWeights[0];
        bonedNormal += (B_BoneAnimationMatrices[boneIds[1]] * modelNormal) * boneWeights[1];
        bonedNormal += (B_BoneAnimationMatrices[boneIds[2]] * modelNormal) * boneWeights[2];
        bonedNormal += (B_BoneAnimationMatrices[boneIds[3]] * modelNormal) * boneWeights[3];
        modelNormal = vec4(bonedNormal.xyz, 0);
    }

    return B_Normal * modelNormal;
}

vec4 GetModelVertexPosition()
{
    vec4 modelPosition = vec4(B_VIn_Position, 1);

    ivec4 boneIds = ivec4(B_VIn_VertexBonesIds);
    vec4 boneWeights = B_VIn_VertexBonesWeights;

    if (B_HasBoneAnimations)
    {
        vec4 bonedPosition = vec4(0);
        bonedPosition += (B_BoneAnimationMatrices[boneIds[0]] * modelPosition) * boneWeights[0];
        bonedPosition += (B_BoneAnimationMatrices[boneIds[1]] * modelPosition) * boneWeights[1];
        bonedPosition += (B_BoneAnimationMatrices[boneIds[2]] * modelPosition) * boneWeights[2];
        bonedPosition += (B_BoneAnimationMatrices[boneIds[3]] * modelPosition) * boneWeights[3];
        modelPosition = vec4(bonedPosition.xyz, 1);
    }

    return modelPosition;
}
vec4 GetWorldVertexPosition(vec4 vertexModelPosition)
{
    return B_Model * vertexModelPosition;
}
vec4 GetProjectedVertexPosition(vec4 vertexModelPosition)
{
    return B_PVM * vertexModelPosition;
}

#ifndef BANG_NO_MAIN
void main()
{
    vec2 uv = vec2(B_VIn_Uv.x, 1.0 - B_VIn_Uv.y);

    vec4 modelPosition = GetModelVertexPosition();
    vec4 worldPosition = GetWorldVertexPosition(modelPosition);
    vec4 worldNormal = GetWorldVertexNormal();

    #ifndef ONLY_OUT_MODEL_POS_VEC4
        B_FIn_Position    = worldPosition.xyz;
        B_FIn_Normal      = worldNormal.xyz;
        B_FIn_AlbedoUv    = uv * B_AlbedoUvMultiply    + B_AlbedoUvOffset;
        B_FIn_NormalMapUv = uv * B_NormalMapUvMultiply + B_NormalMapUvOffset;
        gl_Position       = GetProjectedVertexPosition(modelPosition);

        // Calculate TBN for normal mapping
        if (B_HasNormalMapTexture)
        {
            vec3 tangent = normalize(B_Normal * vec4(B_VIn_Tangent, 0)).xyz;
            vec3 T = (tangent);
            vec3 N = (B_FIn_Normal);
            vec3 B = cross(N, T);
            if (dot(cross(N, T), B) >= 0.0)
            {
                T *= -1; // Ensure RH coord. system
            }
            B_TBN = mat3(T, B, N);
        }
    #else
        gl_Position = worldPosition;
    #endif
}
#endif
