#define BANG_FRAGMENT
#include "Common.glsl"

uniform sampler2D B_RenderTexture_Texture;

in vec2 B_FIn_AlbedoUv;

layout(location = 0) out vec4 B_GIn_Color;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;
    vec3 color = texture(B_RenderTexture_Texture, uv).rgb;
    B_GIn_Color = vec4(color.rgb, 1);
}
