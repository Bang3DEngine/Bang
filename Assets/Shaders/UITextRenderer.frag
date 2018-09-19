#include "UI.frag"

void main()
{
    float texA = texture(B_AlbedoTexture, B_FIn_AlbedoUv).a;
    B_GIn_Color  = vec4(B_MaterialAlbedoColor.rgb,
                        B_MaterialAlbedoColor.a * texA);
}
