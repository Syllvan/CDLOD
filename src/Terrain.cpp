#include "Terrain.h"
#include "utils/glError.hpp"

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
    leafNodeSize = 32.0f;
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

void Terrain::render(Camera *camera, glm::mat4 projection) {
    //build renderlist
    std::stack<Node*> drawStack;
    for (unsigned int i = 0; i < grid.size(); i++) {
        for (unsigned int j = 0; j < grid[0].size(); j++) {
            grid[i][j]->LODSelect( ranges, lodDepth-1, camera, drawStack);
        }
    }

    //TODO! actually drawing stuff!
    while( !drawStack.empty() ) {
        Node* current = drawStack.top();
        drawStack.pop();

        if ( current->isFullResolution() ) {
            //use 32x32 mesh
        } else {
            //use 16x16 mesh
        }

        //get xPos, zPos and size of current, send to shader as uniforms
        //maxHeight minHeight only used for debug rendering
    }

    glm::mat4 view = camera->getViewMatrix();
    glm::vec4 color = glm::vec4(1.0f,0.0f,0.0f,1.0f);

    // clear
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.use();

        // send uniforms
        shaderProgram.setUniform("projection",projection);
        shaderProgram.setUniform("view",view);

        glCheckError(__FILE__,__LINE__);

        fullResMesh.bind();

        static int inc = 1;
        inc *= 2;
        glCheckError(__FILE__,__LINE__);
            glDrawElements(
                 GL_LINES,      // mode
                 fullResMesh.getPointCount(),         // count
                 GL_UNSIGNED_INT,   // type
                 NULL               // element array buffer offset
             );

        glBindVertexArray(0);

    shaderProgram.unuse();
}

