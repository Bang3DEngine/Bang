#define BANG_FRAGMENT
#include "Common.glsl"

uniform sampler2D B_RenderTexture_Texture;

in vec2 B_FIn_AlbedoUv;

layout(location = 0) out vec4 B_GIn_Color;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;
    vec4 color = texture(B_RenderTexture_Texture, uv);
    B_GIn_Color = color;
}
