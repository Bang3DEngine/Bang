#include "G.frag"

uniform samplerCube B_SkyBoxDiffuse;   // Irradiance for diffuse  ambient
uniform samplerCube B_SkyBoxSpecular;  // Irradiance for specular ambient

void main()
{
    vec4 texColor = vec4(1);
    if (B_HasTexture)
    {
        texColor = texture(B_Texture0, B_FIn_Uv);
        if (texColor.a < B_AlphaCutoff) discard;
    }

    B_GIn_Albedo  = B_MaterialAlbedoColor * texColor;
    B_GIn_Normal   = vec4(B_FIn_Normal.xyz * 0.5f + 0.5f, 0);
    B_GIn_Misc     = vec4(B_MaterialReceivesLighting ? 1.0 : 0.0,
                          B_MaterialRoughness / 255.0, 0, 0);

    vec3 ambient = texture(B_SkyBoxDiffuse, B_FIn_Normal.xyz).rgb;
    B_GIn_Color = vec4(B_GIn_Albedo.rgb * ambient, B_GIn_Albedo.a);
}
