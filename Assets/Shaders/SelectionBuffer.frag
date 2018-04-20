#define BANG_FRAGMENT
#include "Common.glsl"

in vec3 B_FIn_Position;
in vec3 B_FIn_Normal;
in vec2 B_FIn_AlbedoUv;

layout(location = 0) out vec4 B_GIn_Color;

uniform vec4 selectionColor;

out vec4 fragColor;

void main()
{
    if (B_HasAlbedoTexture)
    {
        if (texture(B_AlbedoTexture, B_FIn_AlbedoUv).a < B_AlphaCutoff) { discard; }
    }
    fragColor = selectionColor;
}
