#include "ScreenPass.frag"

void main()
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
        }
        break;

        default:
            color = vec4(1,0,1,1);
        break;
    }

    B_GIn_Color = color;
}
