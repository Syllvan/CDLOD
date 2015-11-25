#ifndef CAMERA_H
#define	CAMERA_H

#include "glm/glm.hpp"

class Camera {
public:
    Camera();
    Camera(glm::vec3 position, glm::vec3 direction);
    Camera(const Camera& orig);
    virtual ~Camera();

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    glm::vec3 getPosition();

    glm::vec3 getDirection();

    glm::vec3 direction;
    glm::vec3 position;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

private:
};

#endif	/* CAMERA_H */

