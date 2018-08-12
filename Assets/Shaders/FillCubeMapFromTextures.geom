#define BANG_GEOMETRY
#include "Common.glsl"

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

in TGIn
{
    vec2 B_GIn_Uv;
}
B_VertexIn[];

out TFIn
{
    float B_Face;
    vec2 B_FIn_Uv;
}
B_VertexOut;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        for (int i = 0; i < 3; ++i)
        {
            gl_Layer = face;

            vec4 pos = gl_in[i].gl_Position;
            gl_Position = pos;

            B_VertexOut.B_Face = float(face);
            B_VertexOut.B_FIn_Uv = B_VertexIn[i].B_GIn_Uv;

            EmitVertex();
        }
        EndPrimitive();
    }
}
