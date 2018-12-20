#define BANG_VERTEX
#include "Common.glsl"

layout(location = 0) in vec3 B_VIn_Position;

out vec3 B_FIn_Position;

void main()
{
    B_FIn_Position = (B_Model * vec4(B_VIn_Position, 1)).xyz;
    gl_Position = B_ProjectionView * vec4(B_FIn_Position, 1);
}
