#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 SkyboxTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPos;

void main() {
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

	TexCoord = aTexCoord;
	Normal = normalize(totalNormal);
	FragPos = vec3(model * totalPosition);
	SkyboxTexCoord = reflect(FragPos - cameraPos, Normal);
}
