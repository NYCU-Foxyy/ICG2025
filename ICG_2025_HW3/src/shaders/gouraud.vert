#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Intensity;

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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	
	vec3 vertexPos = vec3(model * vec4(aPos, 1.0));

	TexCoord = aTexCoord;

	vec3 lightDir = normalize(light.position - vertexPos);
	vec3 viewDir = normalize(cameraPos - vertexPos);
	vec3 norm = normalize(aNormal);
	vec3 reflectDir = 2 * dot(lightDir, norm) * norm - lightDir;
	
	vec3 ambientIntensity = light.ambient * material.ambient;
	vec3 diffuseIntensity = light.diffuse * material.diffuse * max(0.0, dot(lightDir, aNormal));
	vec3 specularIntensity = light.specular * material.specular * max(0.0, dot(reflectDir, viewDir));

	Intensity = ambientIntensity + diffuseIntensity + specularIntensity;
}
