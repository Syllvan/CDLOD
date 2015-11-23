#include "Game.h"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "utils/glError.hpp"

#include <GLFW/glfw3.h>

Game::Game()
    : Application()
{
    //init
    glCheckError(__FILE__,__LINE__);

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f,0.0f,1.0f);

    camera = new Camera(position, direction);
    heightMap = new HeightMap("heightMap.bmp");
    terrain = new Terrain(heightMap);
}

Game::Game(const Game& orig) {
}

Game::~Game() {
    delete camera;
    delete heightMap;
    delete terrain;
}

void Game::loop() {
    if (glfwWindowShouldClose(getWindow()))
        exit();

    terrain->render(camera);
}
