#ifndef CAMERA_H
#define	CAMERA_H

    void computeMatricesFromInputs(GLFWwindow* window);
    glm::vec3 getPosition();
	glm::mat4 getView(); 
	glm::mat4 getPerspective(); 

#endif	

