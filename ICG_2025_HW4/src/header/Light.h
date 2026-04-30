#pragma once
#include <glm/glm.hpp>

struct Light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	Light(glm::vec3 _position,
	      glm::vec3 _ambient,
		  glm::vec3 _diffuse,
		  glm::vec3 specular);
};
