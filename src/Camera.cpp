#include "Camera.h"

#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#define PI 3.14159265359

Camera::Camera() {
}

Camera::Camera(glm::vec3 position, glm::vec3 direction) {
    this->position = position;
    this->direction = direction;

    farPlane;
    farPlane.point = position + direction*far;
    farPlane.normal = -direction;

    leftPlane;
    leftPlane.point = position;
    float angle = PI/2.0f + FoV/2.0f + 0.1f;
    leftPlane.normal = glm::vec3(glm::rotate(glm::mat4(),angle,up)*glm::vec4(direction,1.0f));

    rightPlane;
    rightPlane.point = position;
    rightPlane.normal = glm::vec3(glm::rotate(glm::mat4(),-angle,up)*glm::vec4(direction,1.0f));

    topPlane;
    topPlane.point = position;
    topPlane.normal = glm::vec3(glm::rotate(glm::mat4(),angle,right)*glm::vec4(direction,1.0f));

    bottomPlane;
    bottomPlane.point = position;
    bottomPlane.normal = glm::vec3(glm::rotate(glm::mat4(),-angle,right)*glm::vec4(direction,1.0f));
}

Camera::Camera(const Camera& orig) {
}

Camera::~Camera() {
}

bool Camera::AABBInsidePlane(Plane plane, glm::vec3 max_v, glm::vec3 min_v) {
    plane.normal = modelMatrix*plane.normal;

    glm::vec3 p = min_v;

    if (plane.normal.x >= 0)
		p.x = max_v.x;
	if (plane.normal.y >=0)
		p.y = max_v.y;
	if (plane.normal.z >= 0)
		p.z = max_v.z;

    float dotProd = glm::dot(p - plane.point, plane.normal);
    if(dotProd < 0)
        return false;
    else
        return true;
}

//http://www2.in.tu-clausthal.de/~zach/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
bool Camera::AABBInsideFrustum(glm::vec3 max_v, glm::vec3 min_v) {
    modelMatrix = glm::mat3(right,up,-direction);

    nearPlane.point = position - direction*near;
    if (!AABBInsidePlane(nearPlane,max_v,min_v))
        return false;

    farPlane.point = position - direction*far;
    if (!AABBInsidePlane(farPlane,max_v,min_v))
        return false;

    leftPlane.point = position;
    if (!AABBInsidePlane(leftPlane,max_v,min_v))
        return false;

    rightPlane.point = position;
    if (!AABBInsidePlane(rightPlane,max_v,min_v))
        return false;

    topPlane.point = position;
    if (!AABBInsidePlane(topPlane,max_v,min_v))
        return false;

    bottomPlane.point = position;
    if (!AABBInsidePlane(bottomPlane,max_v,min_v))
        return false;

    return true;
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

