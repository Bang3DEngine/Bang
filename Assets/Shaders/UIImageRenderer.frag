#include "UI.frag"

void main()
{
    vec4 color = B_MaterialAlbedoColor;
    if (B_HasAlbedoTexture)
    {
        color *= texture(B_AlbedoTexture, B_FIn_AlbedoUv);
    }

    if (color.a <= B_AlphaCutoff)
    {
        discard;
    }
    B_GIn_Color = color;
}
