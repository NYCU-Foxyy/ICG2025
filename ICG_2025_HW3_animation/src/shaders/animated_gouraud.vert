#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[MAX_BONES];

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
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(aBoneIDs[i] == -1) 
            continue;
        if(aBoneIDs[i] >= MAX_BONES) 
        {
            totalPosition = vec4(aPos, 1.0f);
            totalNormal = aNormal;
            break;
        }
        vec4 localPosition = finalBonesMatrices[aBoneIDs[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeights[i];
        vec3 localNormal = mat3(finalBonesMatrices[aBoneIDs[i]]) * aNormal;
        totalNormal += localNormal * aWeights[i];
    }
    
    // If no bones affect this vertex, use original position and normal
    if(length(totalPosition) == 0.0)
    {
        totalPosition = vec4(aPos, 1.0f);
        totalNormal = aNormal;
    }

	// Use totalPosition as vertex's input pos (aPos)
    // totalNormal as vertex's input normal (aNormal)
	
	gl_Position = projection * view * model * totalPosition;
	
	vec3 vertexPos = vec3(model * totalPosition);

	TexCoord = aTexCoord;

	vec3 lightDir = normalize(light.position - vertexPos);
	vec3 viewDir = normalize(cameraPos - vertexPos);
	vec3 norm = normalize(totalNormal);
	vec3 reflectDir = 2 * dot(lightDir, norm) * norm - lightDir;
	
	vec3 ambientIntensity = light.ambient * material.ambient;
	vec3 diffuseIntensity = light.diffuse * material.diffuse * max(0.0, dot(lightDir, totalNormal));
	vec3 specularIntensity = light.specular * material.specular * max(0.0, dot(reflectDir, viewDir));

	Intensity = ambientIntensity + diffuseIntensity + specularIntensity;
}
