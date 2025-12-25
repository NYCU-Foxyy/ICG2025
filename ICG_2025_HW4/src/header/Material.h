#pragma once
#include <glm/glm.hpp>

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	
	Material();
	Material(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
};
