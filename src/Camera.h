#ifndef CAMERA_H
#define	CAMERA_H

#include "glm/glm.hpp"

class Camera {
public:
    //Camera();
    //Camera(glm::vec3 position, glm::vec3 direction);
    //Camera(const Camera& orig);
    //virtual ~Camera();

    void computeMatricesFromInputs();
    glm::vec3 getPosition();
	glm::mat4 getView(); 
	glm::mat4 getPerspective(); 

private:
    glm::vec3 direction;
    glm::vec3 position;
};

#endif	/* CAMERA_H */

