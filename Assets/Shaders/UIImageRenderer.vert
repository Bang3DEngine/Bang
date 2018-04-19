#define BANG_VERTEX
#include "Common.glsl"

const int MODE_TEXTURE         = 0;
const int MODE_TEXTURE_INV_UVY = 1;
const int MODE_SLICE_9         = 2;

uniform int B_ImageMode;
uniform vec2 B_Slice9BorderStrokePx;

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;

out vec3 B_FIn_Position;
out vec3 B_FIn_Normal;
out vec2 B_FIn_Uv;

void main()
{
    B_FIn_Normal   = normalize( (B_Normal * vec4(B_VIn_Normal, 0)).xyz );
    B_FIn_Uv       = B_VIn_Uv * B_UvMultiply + B_UvOffset;

    vec3 localPos = B_VIn_Position;
    switch (B_ImageMode)
    {
        case MODE_TEXTURE: break;
        case MODE_TEXTURE_INV_UVY: break; // Uvs being changed by mesh in CPU now

        case MODE_SLICE_9:
        {
            // Modify vertex position if needed ========
            // In x
            vec2 strokeSizeLocal = (B_ModelInv * vec4(B_Slice9BorderStrokePx, 0, 0)).xy;
            int vIdX = int(B_VIn_Uv.x * 4); // 0: left, 3: right
            switch (vIdX)
            {
                case 1: localPos.x = (-1.0 + strokeSizeLocal.x); break;
                case 2: localPos.x = ( 1.0 - strokeSizeLocal.x); break;
            }

            // In y
            int vIdY = int(B_VIn_Uv.y * 4); // 0: bot,  3: top
            switch (vIdY)
            {
                case 1: localPos.y = (-1.0 + strokeSizeLocal.y); break;
                case 2: localPos.y = ( 1.0 - strokeSizeLocal.y); break;
            }
            // ==========================================
        }
        break;
    }

    B_FIn_Position = ( B_Model * vec4(localPos, 1) ).xyz;
    gl_Position = B_PVM * vec4(localPos, 1);
}
