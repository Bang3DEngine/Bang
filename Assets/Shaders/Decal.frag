#define BANG_FRAGMENT
#include "Common.glsl"

uniform sampler2D B_DecalTexture;
uniform mat4 B_DecalProjectionMatrix;

in vec3 B_FIn_Position;

layout(location = 0) out vec4 B_GIn_Albedo;

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

    vec4 decalColor = texture(B_DecalTexture, uv);
    B_GIn_Albedo = vec4(1,1,0,1); // decalColor;
    //  B_GIn_Albedo = vec4(uv, 0, 1);
    // B_GIn_Albedo = vec4(uv.y > 0.9 ? vec3(1,0,0) : vec3(0,1,0), 1);
    // B_GIn_Albedo = vec4(pixelCubePos.xyz, 1);
    // B_GIn_Albedo = vec4(1,0,0, 1);
}
