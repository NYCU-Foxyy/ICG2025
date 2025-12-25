#version 330 core
out vec4 FragColor;

in GS_OUT {
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPos;
} fs_in;

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
	vec3 color = texture(ourTexture, fs_in.TexCoord).rgb;

	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	vec3 viewDir = normalize(cameraPos - fs_in.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	vec3 ambientIntensity = light.ambient * material.ambient;
	vec3 diffuseIntensity = light.diffuse * material.diffuse * max(0.0, dot(lightDir, fs_in.Normal));
	vec3 specularIntensity = light.specular * material.specular * max(0.0, dot(fs_in.Normal, halfwayDir));

	FragColor = vec4((ambientIntensity + diffuseIntensity + specularIntensity) * color, 1.0);
}
