#include "Terrain.h"
#include "utils/glError.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#define SHADER_DIR "../shader/"

Terrain::Terrain(HeightMap *h):
    vertexDebug(SHADER_DIR"/basicShader.vert",GL_VERTEX_SHADER),
    fragmentDebug(SHADER_DIR"/basicShader.frag",GL_FRAGMENT_SHADER),
    shaderDebug({vertexDebug,fragmentDebug}),
    vertexTerrain(SHADER_DIR"/terrainShader.vert",GL_VERTEX_SHADER),
    fragmentTerrain(SHADER_DIR"/terrainShader.frag",GL_FRAGMENT_SHADER),
    shaderTerrain({vertexTerrain,fragmentTerrain}),
    vertexArea(SHADER_DIR"/areaShader.vert",GL_VERTEX_SHADER),
    fragmentArea(SHADER_DIR"/areaShader.frag",GL_FRAGMENT_SHADER),
    shaderArea({vertexArea,fragmentArea}),
    fullResMesh(32,32),
    halfResMesh(16,16),
    simpleMesh(1,1)
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

    fullResMesh.bind();
    shaderDebug.setAttribute("position", 3, sizeof(glm::vec3), 0);
    glBindVertexArray(0);
    halfResMesh.bind();
    shaderDebug.setAttribute("position", 3, sizeof(glm::vec3), 0);
    glBindVertexArray(0);

    fullResMesh.bind();
    shaderTerrain.setAttribute("position", 3, sizeof(glm::vec3), 0);
    glBindVertexArray(0);
    halfResMesh.bind();
    shaderTerrain.setAttribute("position", 3, sizeof(glm::vec3), 0);
    glBindVertexArray(0);

    simpleMesh.bind();
    shaderArea.setAttribute("position", 3, sizeof(glm::vec3), 0);
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

void Terrain::debugRender(Camera *camera) {

    // Use our compiled shader program
    shaderDebug.use();

    // Load texture
    GLuint Texture = heightMap->getTexture();
    // Use texture position 0 on GPU
    int texturePosition = 0;
    glActiveTexture(GL_TEXTURE0 + texturePosition);
    // Bind loaded texture to this texture position
    glBindTexture(GL_TEXTURE_2D, Texture);

    // Tell shader to sample from this texture position
    shaderDebug.setUniform("myTextureSampler", texturePosition);


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

    glm::vec3 cameraPos = camera->getPosition();
    shaderDebug.setUniform("projection",projection);
    shaderDebug.setUniform("view",view);
    shaderDebug.setUniform("cameraPos", cameraPos);

    //TODO! actually drawing stuff!
    while( !drawStack.empty() ) {
        Node* current = drawStack.top();
        drawStack.pop();

        float s = current->getSize();
        float l = current->isFullResolution();
        glm::vec4 color = glm::vec4(0.5, 0.5, 0.5, 1.0);
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
        } else if(s < 32.0) {
            color = glm::vec4(0.8,0.5,0.2,1.0);
            if(!l) color = glm::vec4(1.0,0.0,0.0,1.0);
        }

        float scale = current->getSize();
        float range = current->getRange(); //used for interpolation between resolutions

        glm::vec3 translation = glm::vec3(current->getXPos(),0.0,current->getZPos());
        shaderDebug.setUniform("color",color);
        shaderDebug.setUniform("translation",translation);
        shaderDebug.setUniform("scale", scale);
        shaderDebug.setUniform("range", range);
        glCheckError(__FILE__,__LINE__);

        if ( current->isFullResolution() ) {
            drawMesh(&fullResMesh,GL_LINES);
        } else {
            drawMesh(&halfResMesh,GL_LINES);
        }

        glBindVertexArray(0);

    }
    shaderDebug.unuse();
}

void Terrain::areaRender(Camera *camera) {
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Use our compiled shader program
    shaderArea.use();
    //build renderlist
    std::stack<Node*> drawStack;
    for (unsigned int i = 0; i < grid.size(); i++) {
        for (unsigned int j = 0; j < grid[0].size(); j++) {
            grid[i][j]->LODSelect( ranges, lodDepth-1, camera, drawStack);
        }
    }

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glm::vec3 cameraPos = camera->getPosition();
    shaderArea.setUniform("projection",projection);
    shaderArea.setUniform("view",view);

    //TODO! actually drawing stuff!
    while( !drawStack.empty() ) {
        Node* current = drawStack.top();
        drawStack.pop();

        float s = current->getSize();
        float l = current->isFullResolution();
        glm::vec4 color = glm::vec4(0.5, 0.5, 0.5, 1.0);
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
        } else if(s < 32.0) {
            color = glm::vec4(0.8,0.5,0.2,1.0);
            if(!l) color = glm::vec4(1.0,0.0,0.0,1.0);
        }

        float scale = current->getSize();
        float range = current->getRange(); //used for interpolation between resolutions

        glm::vec3 translation = glm::vec3(current->getXPos(),0.0,current->getZPos());
        shaderArea.setUniform("color",color);
        shaderArea.setUniform("translation",translation);
        shaderArea.setUniform("scale", scale);
        shaderArea.setUniform("mnmx", current->getMaxHeight());
        glCheckError(__FILE__,__LINE__);

        drawMesh(&simpleMesh,GL_TRIANGLES);
        glBindVertexArray(0);
        shaderArea.setUniform("mnmx", current->getMinHeight());
        glCheckError(__FILE__,__LINE__);
        drawMesh(&simpleMesh,GL_TRIANGLES);
        glBindVertexArray(0);

    }
    shaderArea.unuse();
    glDisable (GL_BLEND);
}

void Terrain::render(Camera *camera) {

    // Use our compiled shader program
    shaderTerrain.use();

    // Load texture
    GLuint Texture = heightMap->getTexture();
    // Use texture position 0 on GPU
    int texturePosition = 0;
    glActiveTexture(GL_TEXTURE0 + texturePosition);
    // Bind loaded texture to this texture position
    glBindTexture(GL_TEXTURE_2D, Texture);

    // Tell shader to sample from this texture position
    shaderTerrain.setUniform("myTextureSampler", texturePosition);


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

    glm::vec3 cameraPos = camera->getPosition();
    shaderTerrain.setUniform("projection",projection);
    shaderTerrain.setUniform("view",view);
    shaderTerrain.setUniform("cameraPos", cameraPos);

    //TODO! actually drawing stuff!
    while( !drawStack.empty() ) {
        Node* current = drawStack.top();
        drawStack.pop();

        float s = current->getSize();
        float l = current->isFullResolution();

        float scale = current->getSize();
        float range = current->getRange(); //used for interpolation between resolutions

        glm::vec3 translation = glm::vec3(current->getXPos(),0.0,current->getZPos());
        shaderTerrain.setUniform("translation",translation);
        shaderTerrain.setUniform("scale", scale);
        shaderTerrain.setUniform("range", range);
        glCheckError(__FILE__,__LINE__);

        if ( current->isFullResolution() ) {
            drawMesh(&fullResMesh,GL_TRIANGLES);
        } else {
            drawMesh(&halfResMesh,GL_TRIANGLES);
        }

        glBindVertexArray(0);

    }
    shaderTerrain.unuse();
}



