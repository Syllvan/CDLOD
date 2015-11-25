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

Game::~Game() {
    delete camera;
    delete heightMap;
    delete terrain;
}

void Game::loop() {
    glm::mat4 projection = glm::perspective(float(2.0*atan(getHeight()/1920.f)), getWindowRatio(), 0.1f, 100.f);
    if (glfwWindowShouldClose(getWindow()))
        exit();

    terrain->render(camera, projection);
}
