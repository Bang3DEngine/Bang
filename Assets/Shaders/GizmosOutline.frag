#include "ScreenPass.frag"

uniform vec4 B_OutlineColor;
uniform int B_OutlineThickness;

void main()
{
    float depth = B_SampleDepth();
    if (depth < 1.0f) { discard; }

    int MinThickness = -B_OutlineThickness;
    int MaxThickness =  B_OutlineThickness;
    for (int i = MinThickness; i <= MaxThickness; ++i)
    {
        for (int j = MinThickness; j <= MaxThickness; ++j)
        {
            float neighborDepth = B_SampleDepthOffset( vec2(i,j) );
            if (neighborDepth < 1.0f) { B_GIn_Color = B_OutlineColor; return; }
        }
    }
    discard;
}
