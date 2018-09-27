#define BANG_FRAGMENT
#define BANG_DEFERRED_RENDERING
#include "DefaultFragCommon.glsl"

/*
#include "Common.glsl"
#include "LightCommon.glsl"
#include "MaterialPBRUniforms.glsl"

in vec3 B_FIn_Position;

layout(location = 0) out vec4 B_GIn_Color;
layout(location = 1) out vec4 B_GIn_Albedo;
layout(location = 2) out vec4 B_GIn_Normal;
layout(location = 3) out vec4 B_GIn_Misc;

void main()
{
    B_GIn_Albedo = vec4(1, 0, 0, 1);
    B_GIn_Normal = vec4(0, 1, 0, 0);
    B_GIn_Misc   = vec4(0, 0, 0, 0);
    B_GIn_Color  = B_GIn_Albedo;
}
*/
