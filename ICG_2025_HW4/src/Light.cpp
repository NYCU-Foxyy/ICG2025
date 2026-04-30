#include "header/Light.h"

Light::Light(glm::vec3 _position,
             glm::vec3 _ambient,
			 glm::vec3 _diffuse,
			 glm::vec3 _specular):
			 position(_position),
			 ambient(_ambient),
			 diffuse(_diffuse),
			 specular(_specular) {}
