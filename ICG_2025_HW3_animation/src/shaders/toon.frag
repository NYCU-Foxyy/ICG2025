#version 330 core

#define PI 3.1415926538

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;

struct light_t {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct material_t {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float gloss;
};

uniform light_t light;
uniform material_t material;
uniform vec3 cameraPos;

void main()
{
	vec3 color = texture(ourTexture, TexCoord).rgb;

	vec3 lightDir = normalize(light.position - FragPos);
	float cosT = dot(lightDir, Normal);

	float intensity;

	if (cosT > cos(PI / 9)) {
		intensity = 1.0;
	} else if (cosT > cos(PI / 9 * 2)) {
		intensity = 0.9;
	} else if (cosT > cos(PI / 9 * 3)) {
		intensity = 0.8;
	} else if (cosT > cos(PI / 9 * 5)) {
		intensity = 0.65;
	} else if (cosT > cos(PI / 9 * 7.5)) {
		intensity = 0.5;
	} else if (cosT > 0) {
		intensity = 0.4;
	} else {
		intensity = 0.35;
	}

	FragColor = vec4(intensity * color, 1.0);
}
