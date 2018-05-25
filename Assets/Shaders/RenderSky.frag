#include "ScreenPass.frag"

void main()
{
    float depth = B_SampleDepth();
    if (depth > 0.9999999f)
    {
        vec4 color;
        switch (B_Camera_ClearMode)
        {
            case CAMERA_CLEARMODE_COLOR:
                color = B_Camera_ClearColor;
            break;

            case CAMERA_CLEARMODE_SKYBOX:
            {
                vec2 uv = B_GetViewportUv();
                vec3 rayDestPosWorld = B_ComputeWorldPosition(0.0f, uv);
                vec3 camPosWorld = B_GetCameraPositionWorld();
                vec3 rayDirWorld = (rayDestPosWorld - camPosWorld);
                color = vec4( pow(texture(B_SkyBox, rayDirWorld).rgb, vec3(2.2)), 1);
                // color = vec4(textureLod(B_SkyBoxSpecular, rayDirWorld, 1.0).rgb, 1);
                // color = vec4(texture(B_SkyBoxDiffuse, rayDirWorld).rgb, 1);
            }
            break;

            default:
                color = vec4(1,0,1,1);
            break;
        }

        B_GIn_Color = color;
    }
    else { discard; }
}
