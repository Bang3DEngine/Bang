#include "ScreenPass.frag"

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
            vec3 rayDestPosWorld = B_ComputeWorldPosition(0.0f, uv);
            vec3 camPosWorld = B_GetCameraPositionWorld();
            vec3 rayDirWorld = (rayDestPosWorld - camPosWorld);
            color = texture(B_Camera_SkyBox, rayDirWorld);
            color.a = 1.0f;
        }
        else { color = B_Camera_ClearColor; }

        B_GIn_Color = color;
    }
    else { discard; }
}
