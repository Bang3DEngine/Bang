#define BANG_GEOMETRY
#include "Common.glsl"

// This geometry shader will replicate each triangle it receives to all 6 faces
// of the cubemap, by using the gl_Layer variable
// (each layer is one of the faces)

uniform mat4 B_WorldToShadowMapMatrices[6];

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

out vec4 B_FIn_Position;

void main()
{
    for(int face = 0; face < 6; ++face) // For each cubemap face
    {
        gl_Layer = face; // Which of the six faces we want to emit tri to
        for(int i = 0; i < 3; ++i) // For each tri vertex
        {
            B_FIn_Position = gl_in[i].gl_Position;
            gl_Position    = B_WorldToShadowMapMatrices[face] * B_FIn_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
