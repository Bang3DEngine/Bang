#define MAX_BLUR_RADIUS 32
uniform int B_BlurRadius;
uniform bool B_BilateralEnabled = true;
uniform float B_BlurKernel[MAX_BLUR_RADIUS];

void main()
{
    float depth = B_SampleDepth();
    float DepthEpsilon = depth * 0.001;

    int j = 0;
    float meanOcclusion = 0.0f;
    float totalKernelWeight = 0.00001f;
    for (int i = -B_BlurRadius; i <= B_BlurRadius; ++i)
    {
        #ifdef SEPARATE_X
        vec2 offset = vec2(i,0);
        #else
        vec2 offset = vec2(0,i);
        #endif

        float ijDepth = B_SampleDepthOffset(offset);
        if ( !B_BilateralEnabled || abs(depth - ijDepth) < DepthEpsilon )
        {
            float blurKernel_i = B_BlurKernel[j];
            meanOcclusion     += blurKernel_i * B_SampleColorOffset(offset).r;
            totalKernelWeight += blurKernel_i;
        }
        j += 1;
    }
    B_GIn_Color = vec4(vec3(meanOcclusion / totalKernelWeight), 1);
}
