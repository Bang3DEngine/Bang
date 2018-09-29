#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING

#include "Common.glsl"

in vec3 B_FIn_Position;
in vec3 B_FIn_Normal;
in vec2 B_FIn_AlbedoUv;
in vec4 B_FIn_ParticleColor;

layout(location = 0) out vec4 B_GIn_Color;

void main()
{
    vec4 texColor = vec4(1);
    if (B_HasAlbedoTexture)
    {
        texColor = texture(B_AlbedoTexture, B_FIn_AlbedoUv);
    }
    vec4 finalAlbedo = B_FIn_ParticleColor * texColor;

    B_GIn_Color = finalAlbedo;
}
