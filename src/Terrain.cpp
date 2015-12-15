#include "Terrain.h"
#include "utils/glError.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#define SHADER_DIR "../shader/"
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

Terrain::Terrain(HeightMap *h):
    vertexDebug(SHADER_DIR"/debugShader.vert",GL_VERTEX_SHADER),
    vertexShader(SHADER_DIR"/basicShader.vert",GL_VERTEX_SHADER),
    fragmentShader(SHADER_DIR"/basicShader.frag",GL_FRAGMENT_SHADER),
    shaderProgram({vertexShader,fragmentShader}),
    debugShader({vertexDebug,fragmentShader}),
    vertexTerrain(SHADER_DIR"/terrainShader.vert",GL_VERTEX_SHADER),
    fragmentTerrain(SHADER_DIR"/terrainShader.frag",GL_FRAGMENT_SHADER),
    terrainShader({vertexTerrain,fragmentTerrain}),
    fullResMesh(32,32),
    halfResMesh(16,16)
{
    heightMap = h;

    //should probably be defined in a settings class or be different?
    //see page 9 in the paper http://www.vertexasylum.com/downloads/cdlod/cdlod_latest.pdf
    leafNodeSize = 1.0f;
    lodDepth = 6;

    //construct level of detail ranges
    ranges.push_back(2);
    for (int i = 1; i < lodDepth; i++) {
        ranges.push_back(ranges[i-1]*3);
        std::cout << ranges[i-1] << std::endl;
    }

    //construct grid of cdlod quadtrees
    float rootNodeSize = leafNodeSize*pow(2,lodDepth-1);


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

    halfResMesh.bind();
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

void drawMesh(FlatMesh *mesh, GLenum type) {
    mesh->bind();

    static int inc = 1;
    inc *= 2;
    glCheckError(__FILE__,__LINE__);
        glDrawElements(
             type,      // mode
             mesh->getPointCount(),         // count
             GL_UNSIGNED_INT,   // type
             NULL               // element array buffer offset
         );
}

void Terrain::render(Camera *camera) {

    // Use our compiled shader program
    shaderProgram.use();

    // Load texture
    GLuint Texture = heightMap->getTexture();
    // Use texture position 0 on GPU
    int texturePosition = 0;
    glActiveTexture(GL_TEXTURE0 + texturePosition);
    // Bind loaded texture to this texture position
    glBindTexture(GL_TEXTURE_2D, Texture);

    // Tell shader to sample from this texture position
    shaderProgram.setUniform("myTextureSampler", texturePosition);


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

        float s = current->getSize();
        float l = current->isFullResolution();
        glm::vec4 color = glm::vec4(1.0, 0.0, 0.0, 1.0);
        if(s < 2.0) {
            color = glm::vec4(1.0);
            if(!l) color = glm::vec4(0.5,1.0,0.5,1.0);
        } else if(s < 4.0) {
            color = glm::vec4(0.0,1.0,0.0,1.0);
            if(!l) color = glm::vec4(0.0,1.0,1.0,1.0);
        } else if(s < 8.0) {
            color = glm::vec4(0.0,0.0,1.0,1.0);
            if(!l) color = glm::vec4(1.0,0.0,1.0,1.0);
        } else if(s < 16.0) {
            color = glm::vec4(1.0,1.0,0.0,1.0);
            if(!l) color = glm::vec4(1.0,0.5,0.0,1.0);
        }

        float scale = current->getSize();
        float range = current->getRange(); //used for interpolation between resolutions
        glm::vec3 cameraPos = camera->getPosition();

        glm::vec3 translation = glm::vec3(current->getXPos(),0.0,current->getZPos());
        shaderProgram.use();
        shaderProgram.setUniform("projection",projection);
        shaderProgram.setUniform("view",view);
        shaderProgram.setUniform("color",color);
        shaderProgram.setUniform("translation",translation);
        shaderProgram.setUniform("scale", scale);
        shaderProgram.setUniform("range", range);
        shaderProgram.setUniform("cameraPos", cameraPos);
        glCheckError(__FILE__,__LINE__);

        if ( current->isFullResolution() ) {
            drawMesh(&fullResMesh,GL_TRIANGLES);
        } else {
            drawMesh(&halfResMesh,GL_TRIANGLES);
        }

        glBindVertexArray(0);
        shaderProgram.unuse();
/*
        debugShader.use();
        debugShader.setUniform("projection",projection);
        debugShader.setUniform("view",view);
        debugShader.setUniform("color",color);
        debugShader.setUniform("translation",translation);
        debugShader.setUniform("scale", scale);
        debugShader.setUniform("range", range);
        debugShader.setUniform("max", current->getMaxHeight());
        glCheckError(__FILE__,__LINE__);
        drawMesh(&halfResMesh,GL_TRIANGLES);
        glBindVertexArray(0);
        debugShader.unuse();
*/

    }

}


