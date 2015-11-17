#include "Game.h"

Game::Game() {
    //init
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

void Game::run() {
    //Loop here
    terrain->render(camera);
}
