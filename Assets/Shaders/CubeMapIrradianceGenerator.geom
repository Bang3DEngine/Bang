#define BANG_GEOMETRY
#include "Common.glsl"

// This geometry shader will replicate each triangle it receives to all 6 faces
// of the cubemap, by using the gl_Layer variable
// (each layer is one of the faces)

// Also, it transforms the viewport plane coordinates to a cube in 3D, depending
// to which face we clone the triangle to

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

out vec4 B_FIn_Position;

void main()
{
    for (int face = 0; face < 6; ++face) // For each cubemap face
    {
        gl_Layer = face; // Which of the six faces we want to emit tri to
        for (int i = 0; i < 3; ++i) // For each tri vertex
        {
            vec4 pos = gl_in[i].gl_Position;
            B_FIn_Position = vec4(1,1,1,1);
            switch (face)
            {
                // case 0: B_FIn_Position = vec4(1, 1, 1, 1); break;
                case 0: B_FIn_Position = vec4(     1, -pos.y, -pos.x, 1); break;
                case 1: B_FIn_Position = vec4(    -1, -pos.y,  pos.x, 1); break;
                case 2: B_FIn_Position = vec4( pos.x,      1,  pos.y, 1); break;
                case 3: B_FIn_Position = vec4( pos.x,     -1, -pos.y, 1); break;
                case 4: B_FIn_Position = vec4( pos.x, -pos.y,      1, 1); break;
                case 5: B_FIn_Position = vec4(-pos.x, -pos.y,     -1, 1); break;
            }

            gl_Position = pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
