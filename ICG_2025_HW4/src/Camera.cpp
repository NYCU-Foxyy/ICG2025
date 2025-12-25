#include "header/Camera.h"

Camera::Camera(glm::vec3 _position, glm::vec3 _target, glm::vec3 _worldUp):
	position(_position), target(_target), worldUp(_worldUp) {}

glm::vec3 Camera::getFront() {
	return glm::normalize(target - position);
}

glm::vec3 Camera::getRight() {
	return glm::normalize(glm::cross(getFront(), worldUp));
}

glm::vec3 Camera::getUp() {
	return glm::normalize(glm::cross(getRight(), getFront()));
}

glm::vec3 Camera::getPosition() {
	return position;
}

glm::vec3 Camera::getTarget() {
	return target;
}

void Camera::setPosition(glm::vec3 newPosition) {
	position = newPosition;
}

void Camera::setTarget(glm::vec3 newTarget) {
	target = newTarget;
}
