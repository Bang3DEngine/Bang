#define BANG_VERTEX
#include "Common.glsl"

#ifdef BANG_HAS_ANIMATIONS
const int B_MAX_NUM_BONES = 128;
uniform int B_BoneAnimationMatrices[B_MAX_NUM_BONES];
#endif

layout(location = 0) in vec3  B_VIn_Position;
layout(location = 1) in vec3  B_VIn_Normal;
layout(location = 2) in vec2  B_VIn_Uv;
layout(location = 3) in vec3  B_VIn_Tangent;
// #ifdef BANG_HAS_ANIMATIONS
// layout(location = 4) in ivec4 B_VIn_VertexBones; // Max 4 bones per vertex
// layout(location = 5) in  vec4 B_VIn_VertexWeights;
// #endif

out vec3 B_FIn_Position;
out vec3 B_FIn_Normal;
out vec2 B_FIn_AlbedoUv;
out vec2 B_FIn_NormalMapUv;
out vec3 B_FIn_Tangent;
out mat3 B_TBN;

void main()
{
    vec2 uv = vec2(B_VIn_Uv.x, 1.0 - B_VIn_Uv.y);

    #ifdef BANG_HAS_ANIMATIONS
    float x = 0.0;
    for (int i = 0; i < B_MAX_NUM_BONES; ++i)
    {
        x += B_BoneAnimationMatrices[i];
    }
    uv.x += x;
    #endif
    // ivec4 vids = ivec4(0); // B_VIn_VertexBones;
    // mat4 boneTransform =
    //    (B_BoneAnimationMatrices[vids[0]])+ // * B_VIn_VertexWeights[vids[0]]) +
    //    (B_BoneAnimationMatrices[vids[1]])+ // * B_VIn_VertexWeights[vids[1]]) +
    //    (B_BoneAnimationMatrices[vids[2]])+ // * B_VIn_VertexWeights[vids[2]]) +
    //    (B_BoneAnimationMatrices[vids[3]]); //  * B_VIn_VertexWeights[vids[3]]);
    // #else
    const mat4 boneTransform = mat4(1.0);
    // #endif

    B_FIn_Position    = (B_Model  * boneTransform * vec4(B_VIn_Position, 1)).xyz;
    B_FIn_Normal      = (B_Normal * boneTransform * vec4(B_VIn_Normal, 0)).xyz;
    B_FIn_AlbedoUv    = uv * B_AlbedoUvMultiply    + B_AlbedoUvOffset;
    B_FIn_NormalMapUv = uv * B_NormalMapUvMultiply + B_NormalMapUvOffset;
    gl_Position       = B_PVM * boneTransform * vec4(B_VIn_Position, 1);

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
