#ifndef B_KAWASE_NUM_COMPS
#define B_KAWASE_NUM_COMPS 4
#endif

#if (B_KAWASE_NUM_COMPS == 4)
#define B_KAWASE_SWIZZLE
#define B_KAWASE_OUT_TYPE vec4
#elif (B_KAWASE_NUM_COMPS == 3)
#define B_KAWASE_SWIZZLE .xyz
#define B_KAWASE_OUT_TYPE vec3
#elif (B_KAWASE_NUM_COMPS == 2)
#define B_KAWASE_SWIZZLE .xy
#define B_KAWASE_OUT_TYPE vec2
#elif (B_KAWASE_NUM_COMPS == 1)
#define B_KAWASE_SWIZZLE .x
#define B_KAWASE_OUT_TYPE float
#endif

uniform int B_Iteration;
uniform vec2 B_InputTextureSize;
uniform sampler2D B_InputTexture;

in vec2 B_FIn_AlbedoUv;

layout (location = 0) out B_KAWASE_OUT_TYPE B_OutColor;

B_KAWASE_OUT_TYPE KawaseBlur(sampler2D inTexture, vec2 uv, vec2 textureSize, float iteration)
{
    vec2 texelSize = (1.0 / textureSize);
    vec2 uvOffset = texelSize.xy * vec2(iteration, iteration) + texelSize * 0.5;

    B_KAWASE_OUT_TYPE color;
    color  = texture(inTexture, (uv + vec2(-1, 1) * uvOffset)) B_KAWASE_SWIZZLE;
    color += texture(inTexture, (uv + vec2( 1, 1) * uvOffset)) B_KAWASE_SWIZZLE;
    color += texture(inTexture, (uv + vec2( 1,-1) * uvOffset)) B_KAWASE_SWIZZLE;
    color += texture(inTexture, (uv + vec2(-1,-1) * uvOffset)) B_KAWASE_SWIZZLE;

    return color * 0.25;
}

void main()
{
    vec2 uv = B_FIn_AlbedoUv;
    B_OutColor = KawaseBlur(B_InputTexture, uv, B_InputTextureSize, B_Iteration);
}
