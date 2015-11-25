#include "Camera.h"

#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Camera::Camera() {
}

Camera::Camera(glm::vec3 position, glm::vec3 direction) {
    this->position = position;
    this->direction = direction;
}

Camera::Camera(const Camera& orig) {
}

Camera::~Camera() {
}

glm::mat4 Camera::getViewMatrix() {
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() {
    return projectionMatrix;
}

glm::vec3 Camera::getPosition() {
    return position;
};

glm::vec3 Camera::getDirection() {
    return direction;
};

