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
    float t = clock()/100000.0f; //qucik and dirty time
    glm::mat4 view = glm::lookAt(
                    glm::vec3(20.0*sin(t),20.0*cos(t),20.0),
                    glm::vec3(0.0,0.0,0.0),
                    glm::vec3(0.0,0.0,1.0)
                );
    return view;
}

glm::vec3 Camera::getPosition() {
    return position;
};

glm::vec3 Camera::getDirection() {
    return direction;
};

