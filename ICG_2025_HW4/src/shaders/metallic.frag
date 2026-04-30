#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 SkyboxTexCoord;

uniform sampler2D ourTexture;
uniform samplerCube skybox;

struct light_t {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform light_t light;
uniform vec3 cameraPos;

void main() 
{
	vec3 color = texture(ourTexture, TexCoord).rgb;
	vec3 reflectColor = texture(skybox, SkyboxTexCoord).rgb;

	vec3 lightDir = normalize(light.position - FragPos);
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float intensity = 1;
	float bias = 0.2;
	//float alpha = 0.4;
	float alpha = 0.0; // model color should not be mixed for this hw
	
	float brightness = max(dot(lightDir, Normal), 0) * intensity + bias;
	
	FragColor = vec4(alpha * brightness * color + (1 - alpha) * reflectColor, 1.0);
}
