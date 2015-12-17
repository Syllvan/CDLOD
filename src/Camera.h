#ifndef CAMERA_H
#define	CAMERA_H

#include "glm/glm.hpp"

class Camera {

struct Plane {
    glm::vec3 point;
    glm::vec3 normal;
};

public:
    Camera();
    Camera(glm::vec3 position, glm::vec3 direction);
    Camera(const Camera& orig);
    virtual ~Camera();

    bool AABBInsideFrustum(glm::vec3 max_v, glm::vec3 min_v);
    bool AABBInsidePlane(Plane plane, glm::vec3 max_v, glm::vec3 min_v);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    glm::vec3 getPosition();

    glm::vec3 getDirection();

    glm::vec3 direction = glm::vec3(0.0f,0.0f,-1.0f);
    glm::vec3 position;
    glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
    glm::vec3 right = glm::vec3(1.0f,0.0f,0.0f);
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::mat3 modelMatrix;

    Plane nearPlane;
    Plane farPlane;
    Plane leftPlane;
    Plane rightPlane;
    Plane topPlane;
    Plane bottomPlane;

    float FoV = 0.785398f;
    float cosFoV;
    float far = 100.0f;
    float near = 0.1f;

private:
};

#endif	/* CAMERA_H */

