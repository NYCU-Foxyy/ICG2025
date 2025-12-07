#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 ReflectTexCoord;

in float n1;
in float n2;

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

vec3 refract(vec3 I, vec3 N, float etaRatio)
{
	float cosI = dot(-I, N);
	float cosT2 = 1.0 - etaRatio * etaRatio * (1.0 - cosI * cosI);
	vec3 T = etaRatio * I + ((etaRatio * cosI - sqrt(abs(cosT2))) * N);

	if (cosT2 > 0)
		return T;
	
	return reflect(I, N);
}

void main() 
{
	float n1 = 1.0;
	float n2 = 1.52;

	vec3 viewDir = normalize(FragPos - cameraPos);

	vec3 reflectTexCoord = reflect(viewDir, Normal);
	vec3 refractTexCoord = refract(viewDir, Normal, n2 / n1);

	vec3 reflectColor = texture(skybox, reflectTexCoord).rgb;
	vec3 refractColor = texture(skybox, refractTexCoord).rgb;

	vec3 lightDir = normalize(light.position - FragPos);
	
	float R0 = (n2 - n1) / (n2 + n1) * (n2 - n1) / (n2 + n1);
	float RT = R0 + (1 - R0) * pow(1 + dot(viewDir, Normal), 5);
	
	FragColor = vec4(R0 * reflectColor + (1 - R0) * refractColor, 1.0);
}
