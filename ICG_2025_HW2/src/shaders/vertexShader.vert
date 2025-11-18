#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float squeezeFactor;

void main()
{
    // TODO: Implement squeeze effect
	//   1. Adjust the vertex position to create a squeeze effect based on squeezeFactor.
	//   2. Set gl_Position to be the multiplication of the perspective matrix (projection),
	//		view matrix (view), model matrix (model) and the adjusted vertex position.
	//   3. Set TexCoord to aTexCoord.
	// Note: Ensure to handle the squeeze effect for both x and z coordinates.
	vec3 newPos = aPos;
	
	newPos.x += aPos.z * sin(squeezeFactor) / 2;
	newPos.z += aPos.x * sin(squeezeFactor) / 2;
	gl_Position = projection * view * model * vec4(newPos, 0.0);
	TexCoord = aTexCoord;
}
