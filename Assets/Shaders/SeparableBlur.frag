uniform int B_BlurRadius;
uniform bool B_BlurInX;
uniform float B_BlurKernel[256];

uniform vec2 B_InputTextureSize;
uniform sampler2D B_InputTexture;

in vec2 B_FIn_AlbedoUv;

layout (location = 0) out vec4 B_OutColor;

void main()
{
    vec2 uv = B_FIn_AlbedoUv;

    vec2 stepSize = (1.0f / B_InputTextureSize);
    vec2 offMult = (B_BlurInX ? vec2(1,0) : vec2(0,1)) * stepSize;

    int j = 0;
    vec4 meanColor = vec4(0);
    int blurRadius = B_BlurRadius;
    for (int i = -blurRadius; i <= blurRadius; ++i)
    {
        vec2 sampleUvOffset = (offMult * i);
        vec2 sampleUvs = (uv + sampleUvOffset);
        vec4 sampleColor = texture(B_InputTexture, sampleUvs);
        float sampleWeight = B_BlurKernel[j];
        meanColor += sampleWeight * sampleColor;
        j += 1;
    }

    B_OutColor = vec4( vec3(meanColor.r), 1);
}
