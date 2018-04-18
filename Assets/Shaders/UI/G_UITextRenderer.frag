#include "UI.frag"

void main()
{
    vec4 texColor = texture(B_Texture0, B_FIn_Uv);

    B_GIn_Normal = B_GIn_Misc = vec4(0);
    B_GIn_Color  = B_GIn_Albedo = vec4(B_MaterialAlbedoColor.rgb,
                                        B_MaterialAlbedoColor.a * texColor.a);
}
