uniform int B_BlurRadius;
uniform bool B_BlurInX;
uniform float B_BlurKernel[256];

uniform vec2 B_InputTextureSize;

#ifdef BANG_BLUR_TEXTURE_CUBEMAP
uniform vec3 B_SampleDirection;
uniform samplerCube B_InputTexture;
#else
uniform sampler2D B_InputTexture;
#endif

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

        #ifdef BANG_BLUR_TEXTURE_CUBEMAP
        vec3 sampleCoords = B_SampleDirection;
        if (B_SampleDirection.x == 1)
        {
            sampleCoords.yz += vec2(1-sampleUvs.y, 1-sampleUvs.x) * 2.0f - 1.0f;
        }
        else if (B_SampleDirection.x == -1)
        {
            sampleCoords.yz += vec2(1-sampleUvs.y, sampleUvs.x) * 2.0f - 1.0f;
        }
        else if (B_SampleDirection.y == 1)
        {
            sampleCoords.xz += vec2(sampleUvs.x, sampleUvs.y) * 2.0f - 1.0f;
        }
        else if (B_SampleDirection.y == -1)
        {
            sampleCoords.xz += vec2(sampleUvs.x, 1-sampleUvs.y) * 2.0f - 1.0f;
        }
        else if (B_SampleDirection.z == 1)
        {
            sampleCoords.xy += vec2(sampleUvs.x, 1-sampleUvs.y) * 2.0f - 1.0f;
        }
        else if (B_SampleDirection.z == -1)
        {
            sampleCoords.xy += vec2(1-sampleUvs.x, 1-sampleUvs.y) * 2.0f - 1.0f;
        }

        vec4 sampleColor = texture(B_InputTexture, sampleCoords);
        #else
        vec4 sampleColor = texture(B_InputTexture, sampleUvs);
        #endif

        float sampleWeight = B_BlurKernel[j];
        meanColor += sampleWeight * sampleColor;
        ++j;
    }

    B_OutColor = meanColor.rgba;
}
