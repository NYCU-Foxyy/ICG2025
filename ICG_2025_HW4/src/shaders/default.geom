#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec2 TexCoord;
	vec3 Normal;
} gs_in[];

out GS_OUT {
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
} gs_out;

uniform mat4 projection;
uniform mat4 view;

void main() {
	for (int i = 0; i < 3; i++) {
		gl_Position = projection * view * gl_in[i].gl_Position;
		gs_out.TexCoord = gs_in[i].TexCoord;
		gs_out.Normal = gs_in[i].Normal;
		gs_out.FragPos = gl_Position.xyz;
		EmitVertex();
	}
	EndPrimitive();
}
