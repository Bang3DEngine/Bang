#include "UI.frag"

void main()
{
    vec4 texColor = B_HasAlbedoTexture ? texture(B_AlbedoTexture,
                                                 B_FIn_AlbedoUv) : vec4(1);
    B_GIn_Color  = B_MaterialAlbedoColor * texColor;
}
