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
    GLuint loadDDS(const char * imagepath);

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

    glm::vec3 position = glm::vec3( 200, 1, 200 );
    // Initial horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 45.0f;

    float speed = 7.0f; // 3 units / second
    float mouseSpeed = 0.2f;

    bool poslock = false;
    bool hidden_cursor = true;
    bool render_debug_mode = false;
};

#endif	/* GAME_H */
