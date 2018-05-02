#define BANG_VERTEX
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;
layout(location = 3) in vec3 B_VIn_Tangent;

out vec3 B_FIn_Position;
out vec2 B_FIn_AlbedoUv;

uniform vec2 B_destRectMinCoord = vec2(-1, -1);
uniform vec2 B_destRectMaxCoord = vec2( 1,  1);

in vec3 B_In_PositionObject; // Plane positions

void main()
{
    vec2 rectSize   =  B_destRectMaxCoord - B_destRectMinCoord;
    vec2 rectCenter = (B_destRectMaxCoord + B_destRectMinCoord) * 0.5;
    B_FIn_Position = vec3(B_In_PositionObject.xy * rectSize + rectCenter, 0);
    B_FIn_AlbedoUv = B_VIn_Uv;
    gl_Position = vec4(B_FIn_Position, 1);
}
