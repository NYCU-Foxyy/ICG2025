#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in float GradientLevel;

uniform sampler2D ourTexture;
uniform vec3 breathingColor;
uniform float intensity;
uniform bool useGradient;

void main()
{
    // Done: Implement Breathing Light Effect
	//   1. Retrieve the color from the texture at texCoord.
	//   2. Set FragColor to be the texture color * (breathingColor * intensity)
	//   Note: Ensure FragColor is appropriately set for both breathing light and normal cases.
	vec3 textureColor = texture(ourTexture, TexCoord).rgb;
	if (intensity >= 0)
		FragColor = vec4(textureColor * (breathingColor * intensity), 1.0);
	else
		FragColor = vec4(textureColor, 1.0);
	if (useGradient)
		FragColor.g = FragColor.g * GradientLevel;
} 
