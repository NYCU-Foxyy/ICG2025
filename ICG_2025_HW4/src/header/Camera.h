#pragma once
#include <glm/glm.hpp>

class Camera {
public:
	Camera(glm::vec3 _position, glm::vec3 _target, glm::vec3 _worldUp);

	glm::vec3 getFront();
	glm::vec3 getUp();
	glm::vec3 getRight();
	glm::vec3 getPosition();
	glm::vec3 getTarget();
	
	void setPosition(glm::vec3 newPosition);
	void setTarget(glm::vec3 newTarget);
	
private:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 worldUp;
};
