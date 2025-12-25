#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

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
uniform float spikeHeight;

void main() {
	vec3 centerPosition = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position).xyz / 3.0f;
	vec3 centerNormal = normalize(cross(vec3(gl_in[1].gl_Position - gl_in[0].gl_Position), vec3(gl_in[2].gl_Position - gl_in[0].gl_Position)));
	centerPosition += centerNormal * spikeHeight;
	vec2 centerTexCoord = (gs_in[0].TexCoord + gs_in[1].TexCoord + gs_in[2].TexCoord) / 3.0f;

	gl_Position = projection * view * gl_in[0].gl_Position;
	gs_out.TexCoord = gs_in[0].TexCoord;
	gs_out.Normal = gs_in[0].Normal;
	gs_out.FragPos = vec3(gl_Position);
	EmitVertex();

	gl_Position = projection * view * gl_in[1].gl_Position;
	gs_out.TexCoord = gs_in[1].TexCoord;
	gs_out.Normal = gs_in[1].Normal;
	gs_out.FragPos = vec3(gl_Position);
	EmitVertex();

	gl_Position = projection * view * vec4(centerPosition, 1.0f);
	gs_out.TexCoord = centerTexCoord;
	gs_out.Normal = centerNormal;
	gs_out.FragPos = vec3(gl_Position);
	EmitVertex();

	gl_Position = projection * view * gl_in[2].gl_Position;
	gs_out.TexCoord = gs_in[2].TexCoord;
	gs_out.Normal = gs_in[2].Normal;
	gs_out.FragPos = vec3(gl_Position);
	EmitVertex();

	gl_Position = projection * view * gl_in[0].gl_Position;
	gs_out.TexCoord = gs_in[0].TexCoord;
	gs_out.Normal = gs_in[0].Normal;
	gs_out.FragPos = vec3(gl_Position);
	EmitVertex();

	EndPrimitive();
}
