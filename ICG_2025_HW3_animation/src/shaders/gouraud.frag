#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Intensity;

uniform sampler2D ourTexture;

void main()
{
	FragColor = vec4(Intensity * texture(ourTexture, TexCoord).rgb, 1.0);
}
