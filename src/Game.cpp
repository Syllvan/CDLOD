#include "Game.h"
#include <common/camera.h>

Game::Game() {
    //Create matrixes from camera
    computeMatricesFromInputs();
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::vec3 CamPos = getPosition(); 
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    //set id for MVP matrix
    GLuint mvp_id = glGetUniformLocation(program_id, "MVP");
    //send data with first position of MVP matrix to shader
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &MVP[0][0]);

    heightMap = new HeightMap("heightMap.bmp");
    terrain = new Terrain(heightMap);
}

Game::Game(const Game& orig) {
}

Game::~Game() {
    delete heightMap;
    delete terrain;
}

void Game::run() {
    //Loop here
    terrain->render(ViewMatrix, CamPos);
}
