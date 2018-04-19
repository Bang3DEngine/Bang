#define BANG_VERTEX
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;
layout(location = 1) in vec3 B_VIn_Normal;
layout(location = 2) in vec2 B_VIn_Uv;

out vec3 B_FIn_Position;
out vec3 B_FIn_Normal;
out vec2 B_FIn_Uv;

uniform vec2 B_destRectMinCoord = vec2(-1, -1);
uniform vec2 B_destRectMaxCoord = vec2( 1,  1);

in vec3 B_In_PositionObject; // Plane positions

void main()
{
    vec2 rectSize   =  B_destRectMaxCoord - B_destRectMinCoord;
    vec2 rectCenter = (B_destRectMaxCoord + B_destRectMinCoord) * 0.5;
    gl_Position = vec4( (B_In_PositionObject * 2.0f).xy *
                        rectSize  * 0.5 + rectCenter,
                        0, 1);
}
