#define BANG_FRAGMENT

uniform float B_GammaCorrection;
uniform sampler2D B_RenderTexture_Texture;

in vec2 B_FIn_AlbedoUv;

layout(location = 0) out vec4 B_GIn_Color;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;
    vec3 color = texture(B_RenderTexture_Texture, uv).rgb;

    float gammaCorrection = B_GammaCorrection;
    vec3 gammaCorrectedColor = pow(color.rgb, vec3(gammaCorrection));
    B_GIn_Color = vec4(gammaCorrectedColor, 1);
}
