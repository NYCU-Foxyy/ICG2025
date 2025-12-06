#version 330 core
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
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	vec3 ambientIntensity = light.ambient * material.ambient;
	vec3 diffuseIntensity = light.diffuse * material.diffuse * max(0.0, dot(lightDir, Normal));
	vec3 specularIntensity = light.specular * material.specular * max(0.0, dot(Normal, halfwayDir));

	FragColor = vec4((ambientIntensity + diffuseIntensity + specularIntensity) * color, 1.0);
}
