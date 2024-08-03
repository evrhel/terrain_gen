#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 36) out;

uniform mat3 uViews[6];

in VS_OUT
{
    vec3 Position;
} gs_in[];

out GS_OUT
{
    vec3 Position;
} gs_out;

void main()
{
    for (int i = 0; i < 6; i++)
    {
        gl_Layer = i;

        mat3 invView = inverse(uViews[i]);
        
        gl_Position = vec4(gs_in[0].Position, 1.0);
        gs_out.Position = uViews[i] * vec3(gs_in[0].Position.xy, 1.0);
        EmitVertex();

        gl_Position = vec4(gs_in[1].Position, 1.0);
        gs_out.Position = uViews[i] * vec3(gs_in[1].Position.xy, 1.0);
        EmitVertex();

        gl_Position = vec4(gs_in[2].Position, 1.0);
        gs_out.Position = uViews[i] * vec3(gs_in[2].Position.xy, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}