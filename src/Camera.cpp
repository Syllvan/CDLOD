#include "Camera.h"

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

glm::vec3 Camera::getPosition() {
    return position;
};

glm::vec3 Camera::getDirection() {
    return direction;
};

