#include "PP.frag"

uniform bool B_Camera_Has_SkyBox;
uniform samplerCube B_Camera_SkyBox;

void main()
{
    float depth = B_SampleDepth();
    if (depth > 0.9999999f)
    {
        vec4 color;
        if (B_Camera_Has_SkyBox)
        {
            vec2 uv = B_GetViewportUv();
            color = texture(B_Camera_SkyBox, vec3(uv * 2.0f - 1.0f, 10));
            color = texture(B_Camera_SkyBox, vec3(uv.x, 1, uv.y));
            color.a = 1.0f;
            // color = vec4(uv,0,1);
        }
        else { color = B_Camera_ClearColor; }

        B_GIn_Color = color;
    }
    else { discard; }
}
