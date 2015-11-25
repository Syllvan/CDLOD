#include "Terrain.h"
#include "utils/glError.hpp"

#include <iostream>

#define SHADER_DIR "../shader/"

Terrain::Terrain(HeightMap *h):
    vertexShader(SHADER_DIR"/basicShader.vert",GL_VERTEX_SHADER),
    fragmentShader(SHADER_DIR"/basicShader.frag",GL_FRAGMENT_SHADER),
    shaderProgram({vertexShader,fragmentShader}),
    fullResMesh(32,32),
    halfResMesh(16,16)
{
    heightMap = h;

    //should probably be defined in a settings class or be different?
    //see page 9 in the paper http://www.vertexasylum.com/downloads/cdlod/cdlod_latest.pdf
    leafNodeSize = 1.0f;
    lodDepth = 4;

    //construct level of detail ranges
    for (int i = 0; i < lodDepth; i++) {
        ranges.push_back(leafNodeSize*pow(2,i+1));
    }

    //construct grid of cdlod quadtrees
    float rootNodeSize = leafNodeSize*pow(2,lodDepth);


    int gridWidth = floor(heightMap->getWidth()/rootNodeSize);
    int gridHeight = floor(heightMap->getHeight()/rootNodeSize);

    grid.resize(gridWidth);
    for (int i = 0; i < gridWidth; i++) {
        grid[i].resize(gridHeight);
        for (int j = 0; j < gridHeight; j++) {
            float xPos = i*rootNodeSize;
            float zPos = j*rootNodeSize;
            grid[i][j] = new Node(heightMap, rootNodeSize, lodDepth, xPos, zPos);
        }
    }

    //opengl stuff
    fullResMesh.bind();
    shaderProgram.setAttribute("position", 3, sizeof(glm::vec3), 0);
    glBindVertexArray(0);
}

Terrain::~Terrain() {
    for (unsigned int i = 0; i < grid.size(); i++) {
        for (unsigned int j = 0; j < grid[0].size(); j++) {
            delete grid[i][j];
        }
    }
}

void drawMesh(FlatMesh *mesh) {
    mesh->bind();

    static int inc = 1;
    inc *= 2;
    glCheckError(__FILE__,__LINE__);
        glDrawElements(
             GL_LINES,      // mode
             mesh->getPointCount(),         // count
             GL_UNSIGNED_INT,   // type
             NULL               // element array buffer offset
         );
}

void Terrain::render(Camera *camera) {
    //build renderlist
    std::stack<Node*> drawStack;
    for (unsigned int i = 0; i < grid.size(); i++) {
        for (unsigned int j = 0; j < grid[0].size(); j++) {
            grid[i][j]->LODSelect( ranges, lodDepth-1, camera, drawStack);
        }
    }

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    // clear
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //TODO! actually drawing stuff!
    while( !drawStack.empty() ) {
        Node* current = drawStack.top();
        drawStack.pop();


        glm::vec4 color = glm::vec4(1.0, 0.0, 0.0, 1.0);
        if(current->getSize() < 4.0) {
            color = glm::vec4(1.0);
        } else if(current->getSize() < 8.0) {
            color = glm::vec4(0.0,1.0,0.0,1.0);
        } else if(current->getSize() < 16.0) {
            color = glm::vec4(0.0,0.0,1.0,1.0);
        } else {
            color = glm::vec4(1.0,0.0,0.0,1.0);
        }
        std::cout << current->getSize() << std::endl;

        float scale = current->getSize();

        glm::vec3 translation = glm::vec3(current->getXPos(),0.0,current->getZPos());
        shaderProgram.use();
        shaderProgram.setUniform("projection",projection);
        shaderProgram.setUniform("view",view);
        shaderProgram.setUniform("color",color);
        shaderProgram.setUniform("translation",translation);
        shaderProgram.setUniform("scale", scale);
        glCheckError(__FILE__,__LINE__);
        if ( current->isFullResolution() ) {
            drawMesh(&fullResMesh);
        } else {
            drawMesh(&halfResMesh);
        }

        glBindVertexArray(0);
        shaderProgram.unuse();

    }

}

