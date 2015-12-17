#include "Game.h"
#include <iostream>
#include <string>

int main(int argc, const char *argv[])
{
    Game *game;
    std::cout << "Have " << argc << " arguments:" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }
    if(argc == 2) {
        std::string imagepath = argv[1];
        game = new Game(imagepath);
    } else {
        std::cout << argc << std::endl;
        game = new Game("../Textures/fractalnoise.bmp");
    }
    game->run();
    delete game;
    return 0;
}
