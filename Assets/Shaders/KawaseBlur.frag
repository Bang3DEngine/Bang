uniform int B_Iteration;
uniform vec2 B_InputTextureSize;
uniform sampler2D B_InputTexture;

in vec2 B_FIn_AlbedoUv;

layout (location = 0) out vec4 B_OutColor;

vec4 KawaseBlur(sampler2D inTexture, vec2 uv, vec2 textureSize, float iteration)
{
    vec2 texelSize = (1.0 / textureSize);
    vec2 uvOffset = texelSize.xy * vec2(iteration, iteration) + texelSize * 0.5;

    vec4 color;
    color  = texture(inTexture, (uv + vec2(-1, 1) * uvOffset));
    color += texture(inTexture, (uv + vec2( 1, 1) * uvOffset));
    color += texture(inTexture, (uv + vec2( 1,-1) * uvOffset));
    color += texture(inTexture, (uv + vec2(-1,-1) * uvOffset));

    return color * 0.25;
}

void main()
{
    vec2 uv = B_FIn_AlbedoUv;
    B_OutColor = KawaseBlur(B_InputTexture, uv, B_InputTextureSize, B_Iteration);
}
