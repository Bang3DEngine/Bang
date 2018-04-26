#include "UI.frag"

void main()
{
    vec4 texColor = texture(B_AlbedoTexture, B_FIn_AlbedoUv);
    B_GIn_Color  = vec4(B_MaterialAlbedoColor.rgb,
                        B_MaterialAlbedoColor.a * texColor.a);
}
