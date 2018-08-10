#include "UI.frag"

void main()
{
    if (B_HasAlbedoTexture)
    {
        B_GIn_Color = B_MaterialAlbedoColor * texture(B_AlbedoTexture,
                                                      B_FIn_AlbedoUv);
    }
    else
    {
        B_GIn_Color = B_MaterialAlbedoColor;
    }
}
