#include "PP.frag"

void main()
{
    float depth = B_SampleDepth();
    if (depth > 0.9999f)
    {
        vec4 color;
        if (B_Camera_Has_SkyBox)
        {
            vec3 texCoords = B_ViewInv[2].xyz;
            color = texture(B_Camera_SkyBox, texCoords);
        }
        else { color = B_Camera_ClearColor; }

        B_GIn_Color = color;
    }
    else { discard; }
}
