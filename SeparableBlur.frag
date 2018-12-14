#define MAX_BLUR_RADIUS 32
uniform int B_BlurRadius;
uniform bool B_BlurInX;
uniform bool B_BilateralEnabled = true;
uniform float B_BlurKernel[MAX_BLUR_RADIUS];

void main()
{
    float depth = B_SampleDepth();
    vec3 normal = B_SampleNormal();
    float DepthEpsilon = depth * depth * 0.001f;

    int j = 0;
    vec2 offMult = (B_BlurInX ? vec2(1,0) : vec2(0,1));
    float meanOcclusion = 0.0f;
    float totalKernelWeight = 0.00001f;
    for (int i = -B_BlurRadius; i <= B_BlurRadius; ++i)
    {
        vec2 offset = offMult * i;
        bool takeIntoAccount = true;
        float ijOcclusion = B_BlurKernel[j] * B_SampleColorOffset(offset).r;
        if (B_BilateralEnabled)
        {
            float ijDepth = B_SampleDepthOffset(offset);
            vec3 ijNormal = B_SampleNormalOffset(offset);
            if (distance(depth, ijDepth) > DepthEpsilon ||
                dot(normal, ijNormal) < 0.5f)
            {
                takeIntoAccount = false;
            }
        }

        if (takeIntoAccount)
        {
            meanOcclusion     += ijOcclusion;
            totalKernelWeight += B_BlurKernel[j];
        }
        j += 1;
    }
    B_GIn_Color = vec4(vec3(meanOcclusion / totalKernelWeight), 1);
}
