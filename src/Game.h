#ifndef GAME_H
#define	GAME_H

#include <string>

#include "glm/glm.hpp"
#include "system/Application.hpp"

#include "Camera.h"
#include "HeightMap.h"
#include "Terrain.h"
#include "FlatMesh.h"

class Game : public Application {
public:
    Game();
    virtual ~Game();

protected:
    virtual void loop();
    bool handleUserInput(GLFWwindow* window, Camera *camera);

private:
    Camera *camera;
    HeightMap *heightMap;
    Terrain *terrain;



    //camera stuff
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;

    glm::vec3 position = glm::vec3( 0, 0, 5 );
    // Initial horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 45.0f;

    float speed = 10.0f; // 3 units / second
    float mouseSpeed = 0.2f;
};

#endif	/* GAME_H */
