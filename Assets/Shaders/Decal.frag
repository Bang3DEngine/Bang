#define BANG_FRAGMENT

#define BANG_NO_REFLECTION_PROBES

#include "Common.glsl"
#include "LightCommon.glsl"

uniform sampler2D B_DecalTexture;
uniform mat4 B_DecalProjectionMatrix;

in vec3 B_FIn_Position;

layout(location = 0) out vec4 B_GIn_Albedo;
layout(location = 1) out vec4 B_GIn_Color;

void main()
{
    vec3 pixelWorldPos = B_ComputeWorldPosition();

    vec4 pixelCubePos = B_ModelInv * vec4(pixelWorldPos, 1);
    if (pixelCubePos.x < -0.5 || pixelCubePos.x > 0.5 ||
        pixelCubePos.y < -0.5 || pixelCubePos.y > 0.5 ||
        pixelCubePos.z < -0.5 || pixelCubePos.z > 0.5)
    {
        discard;
    }

    vec4 pixelProj = B_DecalProjectionMatrix * (pixelCubePos);
    pixelProj.xyz /= pixelProj.w;

    vec2 uv = pixelProj.xy * 0.5 + 0.5;
    uv = vec2(uv.x, 1.0f - uv.y);

    vec4 pixelAlbedo = texture(B_DecalTexture, uv);
    vec3 pixelNormal = B_SampleNormal();
    float pixelRoughness = B_SampleRoughness();
    float pixelMetalness = B_SampleMetalness();
    vec4 finalColor = GetIBLAmbientColor(true,
                                         pixelWorldPos, pixelNormal, pixelAlbedo,
                                         pixelRoughness, pixelMetalness);

    B_GIn_Albedo = pixelAlbedo;
    B_GIn_Color = finalColor;
}
