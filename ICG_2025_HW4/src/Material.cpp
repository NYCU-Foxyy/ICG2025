#include "header/Material.h"

Material::Material():
	ambient(glm::vec3(0.5)), diffuse(glm::vec3(0.5)), specular(glm::vec3(0.5)) {}

Material::Material(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular):
	ambient(_ambient), diffuse(_diffuse), specular(_specular) {}
